///////////////////////////////////////////// //<>//
/////////////////// LFSR ////////////////////
/////////////////////////////////////////////
int g_key = 1;
int g_takt = 0;
byte[] g_LFSRshiftArr = { 0, 0, 0, 0, 0, 0, 0, 0 };  // заводские настройки
int g_LFSR_codeSize = 8;

public void btn_LFSR_Equally_TEST()
{
  DEBUG = false;
  
  LFSRinit();
  
  
  int isEqu = 0;
  int noEqu = 0;

   
  //int i = 28;

  for (int i = 0; i < 150; i++)
  {
    g_key = (int)(Math.random() * 32767+1);
    for (int j = 0; j < g_LFSRshiftArr.length; j++) g_LFSRshiftArr[j] = (byte)(Math.random() * 30+1);
    char[] pc_init_arr = LFSRinitArrToCharArray(g_key, g_LFSRshiftArr);
    packageMC init = send_package((byte)',', pc_init_arr, "тестовая инициализация LFSR без записи в EEPROM");
        
    print("MCinitArr: ");
    for (int j = 0; j < init.Data.length; j++) print((int)init.Data[j] + " "); println();
    
    print("PCinitArr: ");
    for (int j = 0; j < pc_init_arr.length; j++) print((int)pc_init_arr[j] + " "); println();
    

    char[] ProKey_PC = GenerateLFSRcodeChar(i);
    char[] ProKey_MC = send_package((byte)'.', new char[]{(char)i}, "Тестовая генерация LFSR кода").Data;

    print("PC:");
    for (int j = 0; j < ProKey_PC.length; j++) print((int)ProKey_PC[j] + " "); println();
    print("MC:");
    for (int h = 0; h < ProKey_MC.length; h++) print((int)ProKey_MC[h] + " "); println();

    boolean isEqually =  Arrays.equals(ProKey_PC, ProKey_MC);

    if (isEqually)
      isEqu++;
    else
      noEqu++;

    String infoStr = "Итерация №" + i +" Равны:" + isEqu + " Не равны:" + noEqu;
    println(infoStr);
    println();
  }
  
  DEBUG = true;
}

public void btn_Generate_LFSR_code()
{
  byte[] PC_code = GenerateLFSRcode(28);
  for (int j = 0; j < PC_code.length; j++) print((byte)PC_code[j] + " ");
  println();
}

public void printProKey(byte[] proKey)
{
  for (int i = 1; i < proKey.length + 1; i++)
  {
    print((char)proKey[i - 1]);
    if (i % 4 == 0)
      print(" ");
  }
}

/*public void ProlongationArrInit()
 {
 String testStr = tbx_remote_cmd.getText();
 String[] parts = testStr.split(";");    
 if (parts.length != 10)
 println("Не верное количество параметров !!!!!!!!!!!!!!!!!");
 
 g_key = Integer.parseInt(parts[0].trim());
 g_takt = Integer.parseInt(parts[1].trim());
 
 byte[] initArr = {(byte)Integer.parseInt(parts[2].trim()), 
 (byte)Integer.parseInt(parts[3].trim()), 
 (byte)Integer.parseInt(parts[4].trim()), 
 (byte)Integer.parseInt(parts[5].trim()), 
 (byte)Integer.parseInt(parts[6].trim()), 
 (byte)Integer.parseInt(parts[7].trim()), 
 (byte)Integer.parseInt(parts[8].trim()), 
 (byte)Integer.parseInt(parts[9].trim())};
 
 for (int i = 0; i < initArr.length; i++)
 g_LFSRshiftArr[i] = initArr[i];
 }*/

public void btn_set_LFSR_init_Arr_b()
{  
  LFSRinit();
}

public char[] LFSRinitArrToCharArray(int key_in, byte[] LFSRshiftArr_in)
{
  char[] keyArr = SplitArray(key_in, 3);
  char[] initArr = new char[11];
  for (int i = 0; i < 3; i++)
    initArr[i] = keyArr[i];

  for (int i = 0; i < 8; i++)
    initArr[i + 3] = (char)LFSRshiftArr_in[i];

  return initArr;
}

public void btn_LFSR_encrypt()
{
  for (int i = 1; i < 128; i++)
  {
    byte[] encArr = GenerateLFSRcode(i);
    printProKey(encArr);

    g_takt = 0;

    long current = System.currentTimeMillis();
    int decryptNum = LFSR_decrypt(encArr);
    print("num = " + decryptNum + " ");
    print(" " + (System.currentTimeMillis() - current) + " мс.");
    if (g_takt != decryptNum)
      print(" " + i + " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    println();
  }
}


// Расшифровать число. Перебрать такты LFSR на заданных параметрах
// Принимает параметром 8 значный массив.
int LFSR_decrypt(byte[] encArr)
{
  for (int i = 0; i < 128; i++)
  {
    byte[] keyArr = GenerateLFSRcode(i);

    boolean isEqually = true;
    for (int k = 0; k < keyArr.length; k++)
    {
      if (keyArr[k] != encArr[k])
      {
        isEqually = false;
        break;
      }
    }

    if (isEqually) return i;
  }
  return -1;
}


public void btn_LFSR_decrypt()
{
}

// зашифровать число, вернуть 8значный массив
char[] LFSR_encrypt()
{
  return null;
}

// функция возвращает значение LFSR при данном ключе и на заданном такте
long LFSR(int key, int takt)
{
  long ShiftRegister = key;             // Все, кроме 0.

  for (int i = 0; i < takt; i++)
  {
    ShiftRegister = ((
      ((ShiftRegister >> 31)     // 32
      ^ (ShiftRegister >> 6)     // 7
      ^ (ShiftRegister >> 4)     // 5
      ^ (ShiftRegister >> 2)     // 3
      ^ (ShiftRegister >> 1)     // 2
      ^ (ShiftRegister))         // 1
      & 0x00000001)
      << 31) | (ShiftRegister >> 1);
  }

  return ShiftRegister;
}

// функция возвращает значение LFSR при данном ключе и на заданном такте
int LFSR_myShift(int key, int takt)
{
  int ShiftRegister = key;
  for (int i = 0; i < takt; i++)
  {
    ShiftRegister = ((
      (( ShiftRegister >> g_LFSRshiftArr[0])
      ^ (ShiftRegister >> g_LFSRshiftArr[1])
      ^ (ShiftRegister >> g_LFSRshiftArr[2])
      ^ (ShiftRegister >> g_LFSRshiftArr[3])
      ^ (ShiftRegister >> g_LFSRshiftArr[4])
      ^ (ShiftRegister >> g_LFSRshiftArr[5])
      ^ (ShiftRegister >> g_LFSRshiftArr[6])
      ^ (ShiftRegister >> g_LFSRshiftArr[7])
      ^ (ShiftRegister))
      & 0x00000001)
      << 30) | (ShiftRegister >> 1);
  }

  if (takt == 0)
    ShiftRegister = 0;

  return ShiftRegister;
}

byte[] GenerateLFSRcode(int takt)
{
  byte[] prolongationKey = new byte[g_LFSR_codeSize];

  int pkIndex = 0;

  for (int i = 0; i < 1; i++)
  {
    long num = LFSR_myShift(g_key, i + takt);
    byte[] charArr = split_int(num);

    for (int k = 0; k < charArr.length; k++)
    {
      if (pkIndex > g_LFSR_codeSize - 1)
        break;

      prolongationKey[pkIndex] = charArr[k];
      pkIndex++;
    }
  }

  if (pkIndex < g_LFSR_codeSize)
    for (int i = pkIndex; i < g_LFSR_codeSize; i++)
      prolongationKey[i] = 0;

  return prolongationKey;
}

char[] GenerateLFSRcodeChar(int takt)
{
  char[] prolongationKey = new char[g_LFSR_codeSize];

  int pkIndex = 0;

  for (int i = 0; i < 1; i++)
  {
    long num = LFSR_myShift(g_key, i + takt);
    byte[] charArr = split_int(num);

    for (int k = 0; k < charArr.length; k++)
    {
      if (pkIndex > g_LFSR_codeSize - 1)
        break;

      prolongationKey[pkIndex] = (char)charArr[k];
      pkIndex++;
    }
  }

  if (pkIndex < g_LFSR_codeSize)
    for (int i = pkIndex; i < g_LFSR_codeSize; i++)
      prolongationKey[i] = 0;

  return prolongationKey;
}

char[] LFSRencArr(char[] arr)
{
  char[] encArr = new char[arr.length * g_LFSR_codeSize];

  int x = 0;
  for (int i = 0; i < arr.length; i++) 
  {
    byte[] currCode = GenerateLFSRcode(arr[i]);

    if (i != 0) x += g_LFSR_codeSize;
    for (int k = 0; k < g_LFSR_codeSize; k++)
    {
      encArr[k + x] = (char)currCode[k];
    }
  }

  return encArr;
}

char[] LFSRdecryptArr(char[] encArr)
{
  char[] decryptArr = new char[encArr.length / g_LFSR_codeSize];

  int x = 0;
  for (int i = 0; i < decryptArr.length; i++)
  {
    byte[] tmp = new byte[g_LFSR_codeSize];

    if (i != 0)
      x += g_LFSR_codeSize;

    for (int j = 0; j < g_LFSR_codeSize; j++)
      tmp[j] = (byte)encArr[j + x];

    int res = LFSR_decrypt(tmp);
    decryptArr[i] = (char)res;
  }

  return decryptArr;
}