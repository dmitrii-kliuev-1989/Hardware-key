public class packageMC //<>//
{
  public char Size;
  public char Response;
  public char[] Data;
}

byte g_command = 0;

//__________________________
//_____ Отправка на МК  ____
//__________________________
// Команда, данные
public packageMC send_package(byte command, char data[], String info)
{
  g_command = command;
  d_println("");
  d_println("");
  boolean lfsrSettings = true;
  for (int i = 0; i < 8; i++)
  {
    if (g_LFSRshiftArr[i] == 0)
    {
      lfsrSettings = false;
      break;
    }
  }

  if (!lfsrSettings)
    d_println("                                                       ЗАВОДСКИЕ НАСТРОЙКИ LFSR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

  int keySum = 0;
  for (int i = 0; i < 8; i++)
  {
    keySum += keyArr_test[i];
  }

  if (keySum == 0)
    d_println("                                                       ЗАВОДСКИЕ НАСТРОЙКИ BDC КЛЮЧА !!!!!!!!!!!!!!!!!!!!!!!!!!");

  d_println(info);
  String currentDateTime = day() + "." + month() + "." + year() + " " + hour() + ":" + minute() + ":" + second();
  d_println("ProgramCounter: " + ProgramCounter + " ______ " + currentDateTime + " _______unix TimeStamp: " +  System.currentTimeMillis()/1000);

  ProgramCounter++;

  // Шифруемый пакет
  // не шифруется,   не шифруется     шифруется
  // команда(1байт), размер(3байта),  шифрованная последовательность по размеру

  // Размер данных
  byte dataSize = 0;
  if (data != null) dataSize = (byte)data.length;

  d_println("     outCmd: " + (int)command);
  d_println("outDataSize: " + (int)dataSize);
  if (data != null)
  {
    d_print("    outData: ");
    printCharArrayINT(data);
  }

  // зашифрование
  char encryptDataPackageSize = 0;
  char[] encryptDataPackage = new char[0];
  if (data != null && dataSize != 0)  // шифруем данные если они есть 
  {
    d_print("            CurrentKey: ");
    printCharArrayINT(keyArr_test);
    encryptionData encryptedData = encrypt(data, keyArr_test);
    d_println("    encryptedData.Size: " + (int)encryptedData.Size);
    d_print("    encryptedData.Data: ");
    printCharArrayINT(encryptedData.Data);

    encryptDataPackageSize = (char)(encryptedData.Size);
    encryptDataPackage = new char[encryptDataPackageSize]; 

    d_println("encryptDataPackageSize: " + (int)encryptDataPackageSize);  

    for (int i = 0; i < encryptedData.Size; i++)
      encryptDataPackage[i] = encryptedData.Data[i];

    d_print("    encryptDataPackage: ");
    printCharArrayINT(encryptDataPackage);
  }

  int PacketSize = encryptDataPackageSize + 4; // Размер всего пакета. 1байт команда, 1й, 2й, 3й размер, данные по размеру
  char[] packageOut = new char[PacketSize];
  packageOut[0] = (char)command;

  // дописать размер
  char[] dsArray = new char[3];  
  dsArray = SplitArray(encryptDataPackageSize, 3);
  for (int i = 0; i < 3; i++)
    packageOut[i+1] = dsArray[i]; 

  // дописать зашифрованные данные
  for (int i = 0; i < encryptDataPackageSize; i++)
    packageOut[i + 4] = encryptDataPackage[i]; 


  d_print(" packageOut: ");
  printCharArrayINT(packageOut);  

  if (SendHandshake()) 
    d_println("ok handshake");
  else 
  { 
    d_println("bad handshake"); 
    return null;
  }

  d_println("__________________________________________________________________________");

  myPort.write(byte(packageOut));  // отправка

  // ждем ответа.
  if (!PortWait(1000)) // Если вышли по таймауту 
  { 
    d_println("Команда '" + command +"'. Ответ не получен по истечению таймаута"); 
    return null;
  }
  packageMC pack  = recieve();
  return pack;
}

public packageMC recieve()
{
  // 0й байт овтет
  byte response = -1;
  if (myPort.available() > 0) response = (byte)(myPort.read());

  // 1й, 2й, 3й - размер
  char[] dsArray = new char[3];
  for (char i = 0; i < 3; i++)
    dsArray[i] = (char)myPort.read();

  d_print("inDataSize: ");
  printCharArrayINT(dsArray);

  int inEncryptedDataSize = JoinArray(dsArray, (byte)3);

  // данные по размеру
  char[] inDataEncrypted = new char[inEncryptedDataSize];
  for (int i = 0; i < inEncryptedDataSize; i++) inDataEncrypted[i] = (char)myPort.read();

  if (inEncryptedDataSize != 0)
  {
    d_println("inEncryptedDataSize: " + (int)inEncryptedDataSize);
    d_print("    inDataEncrypted: ");
    printCharArrayINT(inDataEncrypted);

    d_print("        inByteArray: ");
    for (int i =0; i< inDataEncrypted.length; i++) d_print(String.format("%8s", Integer.toBinaryString(inDataEncrypted[i])).replace(' ', '0') + " ");
    d_println("");
  }

  encryptionData mcEnc__Data = new encryptionData();

  mcEnc__Data.Data = inDataEncrypted; 
  mcEnc__Data.Size = (char)inDataEncrypted.length;

  encryptionData decrData = decrypt(mcEnc__Data, keyArr_test);

  if (decrData.Size != 0)
  {
    d_print("    inDataDecrypted: ");
    printCharArrayINT(decrData.Data);
  }

  d_println("  response: " + (int)response);
  d_println("inDataSize: " + (int)decrData.Size);
  d_print("    inData: ");
  if (decrData.Size != 0)
    printCharArrayINT(decrData.Data);
  else
    d_println("NO_DATA");

  packageMC pack = new  packageMC();
  pack.Response = (char)response;
  pack.Size = decrData.Size;
  pack.Data = decrData.Data;

  return pack;
}

// Ожидание ответа от МК. Если не дождались ответа false
public boolean PortWait(int waitQty)
{
  // ждем ответа
  int waitCounter = 0;
  boolean wait = true;
  while (myPort.available() == 0 && wait)
  {
    delay(10);
    waitCounter++;
    if (waitCounter > waitQty) wait = false;
  }

  return wait;
}