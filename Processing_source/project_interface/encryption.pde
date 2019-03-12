public class encryptionData
{
  public char[] Data;
  public char Size;
}

public char[] keyArr_test = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0};
                              
/*public char[] keyArr_test = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                              1, 1};*/
                                
// 
/*public char[] keyArr_test =  {1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 
                              0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 
                              0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 
                              0, 0};*/

char[] GenerateDefaultKey(String pswd)
{
  char[] password = GetPswdArray(pswd);
  
  // Формирование ключа по умолчанию из первых 32 элементов пароля
  char defKeySize = 32;
  char[] defaultKey = new char[defKeySize];
       
  for (int i = 0; i < defKeySize; i++)
    defaultKey[i] = char(password[i] & 1);

  printCharArrayINT(defaultKey);
  
  return defaultKey;
}

char[] GetPswdArray(String pswd)
{
  char[] pswdArr = new char[pswd.length()];
  
  for (int i = 0; i < pswd.length(); i++)
  {  
    String oneChar = pswd.substring(i, i+1);
    pswdArr[i] = (char)oneChar.charAt(0); 
  }
  
  return pswdArr;
}

public void btn_encryption_test()
{ 
  //encTest();
  //encTest2();
    //GenerateDefaultKey();
  
  
  /*
      outData: 14 62 56 18 36 
            CurrentKey: 1 1 1 0 0 0 1 0 0 0 0 0 0 1 1 0 0 1 1 0 0 1 1 1 0 1 0 0 1 0 0 0 
    encryptedData.Size: 6
    encryptedData.Data: 15 31 28 12 133 64
  */

  
    char[] inputData = {5, 43, 14, 62, 56, 46, 19};
    //char[] inputData = {};
    for (char i = 0; i < inputData.length; i++)  print(int(inputData[i]) + "\t");
    println();
    for(int i =0;i< inputData.length; i++)
      print(String.format("%8s", Integer.toBinaryString(inputData[i])).replace(' ', '0') + " ");
    println();
    println();

    encryptionData encryptedData = encrypt(inputData, keyArr_test);    
    for (char i = 0; i < encryptedData.Data.length; i++) print(hex(encryptedData.Data[i]) + "\t");
    println();
    for(int i =0;i< encryptedData.Data.length; i++)
      print(String.format("%8s", Integer.toBinaryString(encryptedData.Data[i])).replace(' ', '0') + " ");
    println();
    println();

    encryptionData decryptedData = decrypt(encryptedData, keyArr_test);
    for (char i = 0; i < decryptedData.Size; i++) print(int(decryptedData.Data[i]) + "\t");
    println();
    for(int i =0;i< decryptedData.Size; i++)
      print(String.format("%8s", Integer.toBinaryString(decryptedData.Data[i])).replace(' ', '0') + " ");
    println();
}

public void encTest()
{
  char encryptTestSize = 5;
  char[] encryptTest = new char[encryptTestSize];
  
  for(char i = 0; i < encryptTestSize; i++)
  {
    for(char j = 0; j < 10; j++)
    {
      println();
      encryptTest[i] = (char)j;
      printCharArrayINT(encryptTest);
      encryptionData encryptedData = encrypt(encryptTest, keyArr_test);
      
      for (char t = 0; t < encryptedData.Data.length; t++) print(int(encryptedData.Data[t]) + " ");
      println();
      
      encryptionData decryptedData = decrypt(encryptedData, keyArr_test);
      for (char k = 0; k < decryptedData.Size; k++) print(int(decryptedData.Data[k]) + " ");
      println();
    }
  }

  println();
}

public void encTest2()
{
  char encryptTestSize = 5;
  char[] encryptTest = new char[encryptTestSize];
    
  for(char i = 0; i < 10; i++)
  {
      println();
      encryptTest[4] = i;
      printCharArrayINT(encryptTest);
      encryptionData encryptedData = encrypt(encryptTest, keyArr_test);
      
      for (char t = 0; t < encryptedData.Data.length; t++) print(hex(encryptedData.Data[t]) + " ");
      println();
      
      encryptionData decryptedData = decrypt(encryptedData, keyArr_test);
      for (char k = 0; k < decryptedData.Size; k++) print(int(decryptedData.Data[k]) + " ");
      println();
  }

  println();
}

/////////////
encryptionData encrypt(char[] inputData, char[] Table_)
{
  encryptionData encData = new encryptionData();

  char parser = 7; // Bit parser

  char EncryptDataSize_l = 0;

  char tNumb = 0;
  char uNumb = 0;
  char tens = 0;
  char units = 0;

  // Shifted tens & units
  char stens = 0;
  char sunits = 0;

  char tens_n;
  char stens_n;
  char units_n;
  char sunits_n;
  char[] EncryptData_n = new char[255];

  // Go through all bytes
  for (char k = 0; k < inputData.length; k++)  // global size
  {
    // Translate data to char
    tens = char(inputData[k] / 16); // 7
    units = char(inputData[k] % 16); // 4

    // Tens dance
    if ( Table_[tens] == 1)
    {  // 5 bits
      stens =  char((tens << 1) + 1);//add 1 to tens
      tNumb = 5;
    } else 
    {
      //4b bits
      stens = tens;
      tNumb = 4;
    }
    // Units dance
    if ( Table_[Table_.length/2 + units] == 1)
    {  // 5 bits
      sunits =  char(units << 1); // add 0 to units
      uNumb = 5;
    } 
    else
    {
      // 4b bits
      sunits = units;
      uNumb = 4;
    }

    tens_n = stens;
    stens_n = tNumb;
    units_n = sunits;
    sunits_n = uNumb;

    char templ = 1; // 1 = 0000 0001

    // Tens
    for (char i = char(stens_n - 1); i >= 0; i--)
    {
      // break: variable overflow
      if (i > stens_n) break;

      templ = 1;
      if (parser > 7)
      {
        EncryptDataSize_l++;
        parser = 7;
      }
      EncryptData_n[EncryptDataSize_l] = char(EncryptData_n[EncryptDataSize_l] | (  (( tens_n & ( templ <<  i)) >>  i) <<  parser  ));
      parser--;
    }

    // Единицы
    for (char i = char(sunits_n - 1); i >= 0; i--)
    {
      if (i > sunits_n) break;

      templ = 1;
      if (parser > 7)
      {
        EncryptDataSize_l++;
        parser = 7;
      }
      EncryptData_n[EncryptDataSize_l] = char(EncryptData_n[EncryptDataSize_l] | (  ((units_n & (templ <<  i)) >> i) <<  parser  ));
      parser--;
    }
  }

  char[] EncryptDataNormal = new char[EncryptDataSize_l + 1];
  for (int i = 0; i < EncryptDataSize_l+1; i++)
    EncryptDataNormal[i] = EncryptData_n[i];

  encData.Size = char(EncryptDataSize_l+1);
  encData.Data = EncryptDataNormal; //EncryptData_n;
  return encData;
}

encryptionData decrypt(encryptionData encData, char[] Table_)
{
  char[] EncryptData = encData.Data;
  char EncryptDataSize = encData.Size;

  char[] DecryptData = new char[255];
  encryptionData decrData = new encryptionData();
  char selector = 128; // 1000 0000
  char DecrDigit = 0;
  char ShiftFlag = 0; // false
  char DecrTens = 0;
  char DecrUnit = 0;

  char ByteNumber = 0;
  char SizeCount = 0;
  char outByteNumb = 0;

  // EncryptDataSize++;
  while (ByteNumber < EncryptDataSize)
  { //Input Byte Bits Parser

    for (char i = 0; i < 8; i++)
    {        
      //Check situation: end of byte with decryption parser
      if (ShiftFlag == 255 && i == 0)
      {
        i++;
        ShiftFlag = 0;
      } else {
        ShiftFlag = 0;
      }
      //bit conversions
      char ShiftedSelector = char(selector >> i);
      ///Select bit
      char SelectedBit = char(EncryptData[ByteNumber] & ShiftedSelector);
      //Selected bit now is the first on the right
      SelectedBit = char(SelectedBit >> (7 - i));
      //Move bit to the correct position
      SelectedBit = char(SelectedBit << (7 - SizeCount));
      //Write new bit into decrypted byte
      DecrDigit = char((DecrDigit | SelectedBit));

      //Check table & counter" tenses
      if (SizeCount == 3)//Tens stuff
      {
        //Select tens
        DecrTens = char(DecrDigit & 240);//1111 0000
        DecrTens = char(DecrTens >> 4);//0000 1111
        //Check shifts: tenses
        if ( Table_[DecrTens] == 1) {
          //Shift bit position
          if (i < 8)
          {
            i++;
            //Check edge bit shift
            if (i == 8) {
              ShiftFlag = 255;
            }
          }
        }
      }
      //Check table & counter: units
      if (SizeCount == 7)
      {
        //Select unit
        DecrUnit = char(DecrDigit & 15);//0000 1111
        //Check shifts: units
        if (Table_[Table_.length/2 + DecrUnit] == 1)//??????????????????Rewrite  . Вот здесь что то не прет.. ок
        {
          if (i < 8)
          {
            i++;
            if (i == 8) {
              ShiftFlag = 255;
            }
          }
        }//end shift

        //Get output Byte
        DecryptData[outByteNumb] = char((DecrTens * 16) + DecrUnit);
        //Remove variables value
        DecrDigit = 0;
        DecrTens = 0;
        DecrUnit = 0;
        outByteNumb++;
      }//end size count

      //Check the output byte length
      if (SizeCount < 8) {
        SizeCount++;
        //Check byte overflow
        if (SizeCount == 8) {
          SizeCount = 0;
        }
      }
    }//end for

    ByteNumber++;
  }//end while

  char[] DecryptDataNormal = new char[outByteNumb];
  for (int i = 0; i < outByteNumb; i++)
    DecryptDataNormal[i] = DecryptData[i];


  decrData.Data = DecryptDataNormal;
  decrData.Size = outByteNumb;

  return decrData;
}