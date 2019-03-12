byte handshake =                      'Z';       // поздороваться
byte enableCheckVoltageFlag =         'v';  // активировать проверку напряжений

// Action. Обследование/беседа с проверяемым
byte action =                         'U';  // провести беседу
byte SetActionMaxQuontity =           'H';  // Установить максимально количество бесед в сутки
byte GetActionMaxQuontity =           'G';  // Получить максимально количество бесед в сутки    
byte SetActionTimeCorrector =         'Q';  // Установить корректор времени для часового пояса
byte GetActionTimeCorrector =         'J';  // Получить корректор времени для часового пояса
byte GetRemainCurrentQtyAction =      'R';  // Получить текущее оставшееся количество бесед

// Время лицензии
byte set_endLicenseTime =             'T';  // Установить время окончания лицензии в EEPROM
byte get_endLicenseTime =             'E';  // Получить время окончания лицензии из EEPROM

// Серийный номер
byte writeSerNum =                    'W';  // Записать серийный номер в EEPROM
byte getSerNum =                      'S';  // Получить серийный номер из EEPROM

// Пароль
byte set_pasword =                    'P';  // Установить пароль
byte send_password =                  'A';  // Авторизация
byte use_default_key =                'Y';  // Использовать ключ по умолчанию.

// Время
byte timing =                         '+';  // синхронизировать время МК с ПК
byte get_time =                       '?';  // получить время МК
byte GetPlusTimeMistake =             'F';  // получить положительную накрутку
byte SendMinusTimeMistake =           'M';  // Отправить на ПК сумму отрицательных накруток в секундах
byte GetSynchroTimeFromEEPROM =       'I';  // получить время синхронизации из EEPROM  
byte SendMaxPositive =                'D';  // получить максимально возможную положительную накрутку
byte SendMaxNegative =                'K';  // получить максимально возможную отрицательную накрутку

// Шифрование
byte GenerateKey =                    '4';  // получить сгенерированный рандомный ключ

// Флэш
byte FORMAT_FLASH =                   'N';  // Отформатировать flash

// База данных
byte DB_Read_All =                    '5';  // Получить всю БД
byte DB_SetOneVal =                   '6';  // Записать 1 байт по ключу
byte DB_GetOneVal =                   '7';  // Прочитать 1 байт по ключу

// Посчитать по формуле
byte compute_f_wzh =                  'V';

// Контроль напряжений
byte setHallWay =                     'h';  // записать коридор в память EEPROM 
byte getHallWay =                     'g';  // считать коридор

// Дистанционное продление лицензии
byte setLFSRinitArr =                 'b';  // Инициализировать LFSR  

// Работа с ip адресом и портом
byte getIpAddress =                   't';  // получить ip адрес
byte getPort =                        'w';  // Получить порт

// Логика
byte getLogic_cmd =                   '!';  // Получить логику

// Блокировка
byte block_cmd =                      '#';  // Заблокировать ключ
byte setLogic_cmd =                   '$';  // Установить логику на фабрике

///////// Удаленные команды
byte remoteCMD =                      'r';  // Отправить удаленную команду

byte setIpAddress_cmd =               1;    // Установить ip адрес
byte setPort_cmd =                    2;    // Установить порт
byte unlock_cmd =                     3;    // Разблокировать ключ
byte changeLogic_cmd =                4;    // Изменить логику. дни/обследования
byte prolongation_cmd =               5;    // Продлить ключ на количество дней/обследований
byte getActionsOrDays_cmd =           6;    // Получить количество дней/обследований
byte getBlockReason_cmd =             7;    // Получить причину блокировки

//______________________________________________________________________________________________________
//__________________________________  Кнопки  __________________________________________________________
//______________________________________________________________________________________________________

public void btn_handshake_test_z()
{
  boolean result = SendHandshake();
  if (result)
    println("ok");
  else
    println("bad");

  if (!PortWait(100)) return;

  byte incomingByte = (byte)myPort.read();
  println("TimeOutError  = " + hex(incomingByte));
}


////////////// работа с паролем
public void btn_set_password_P()
{
  String pswd = tbx_password.getText();
  char[] pswdArr = new char[pswd.length()];

  for (int i = 0; i < pswd.length(); i++)
  {  
    String oneChar = pswd.substring(i, i+1);
    pswdArr[i] = (char)oneChar.charAt(0);
  }

  send_package(set_pasword, pswdArr, "Установка пароля");

  keyArr_test = GenerateDefaultKey(tbx_password.getText());
}

// Проверить пароль в МК
public void btn_Send_true_Password_A()
{    
  String pswd = tbx_password.getText();
  char[] pswdArr = new char[pswd.length()];

  for (int i = 0; i < pswd.length(); i++)
  {  
    String oneChar = pswd.substring(i, i+1);
    pswdArr[i] = (char)oneChar.charAt(0);
  }

  printCharArrayINT(send_package(send_password, pswdArr, "Авторизация").Data);
}



public void btn_Send_bad_Password_A()
{
  char AdminPassword[] = {1, 71, 12, 58, 33, 11, 22, 12, 25, 8, 
    23, 98, 11, 41, 12, 2, 33, 85, 12, 12, 
    12, 15, 1, 23, 17, 14, 78, 2, 78, 12, 
    11, 33, 34, 12, 28, 23, 42, 42, 21, 2};
  printCharArrayINT(send_package(send_password, AdminPassword, "Авторизация").Data);
}

public void btn_use_default_key_Y()
{
  send_package(use_default_key, null, "Сброс секретного ключа на дефолтный");
  keyArr_test = GenerateDefaultKey(tbx_password.getText());

  println("Используем дефолтный ключ");
}

// Работа с флеш

public void btn_FLASH_FORMAT_F()
{
  printCharArrayINT(send_package(FORMAT_FLASH, null, "Форматирование").Data);
}

////////////////////// Время ////////////////////////////
public void btn_Timing_i()
{
  int pc_uts = (int)(System.currentTimeMillis()/1000);
  char[] USTarr = SplitArray(pc_uts, 5);
  send_package(timing, USTarr, "Синхронизация времени");
}

public void btn_get_mc_time()
{
  int pc_utc = PC_UTS();
  int mc_ust = MC_UTS();
  println("PC time: " + pc_utc + "\nMC time: " + mc_ust);
}

public void btn_GetSynchroTimeFromEEPROM_i()
{
  char[] timeFromEEPROM = send_package(GetSynchroTimeFromEEPROM, null, "Получить время синхронизации из EEPROM").Data;
  if (timeFromEEPROM != null)
    printCharArrayINT(timeFromEEPROM);
}

public void btn_SendMinusTimeMistake_m()
{
  char[] MinusTimeMistakeArr = send_package(SendMinusTimeMistake, null, "Отрицательные накрутки времени").Data;

  int mc_time = JoinArray(MinusTimeMistakeArr, (byte)MinusTimeMistakeArr.length); 
  println(mc_time);
}

public void btn_GetPlusTimeMistake_F()
{
  char[] PlusTimeMistakeArr =  send_package(GetPlusTimeMistake, null, "Положительные накрутки времени").Data;
  int mc_time = JoinArray(PlusTimeMistakeArr, (byte)PlusTimeMistakeArr.length); 
  println(mc_time);
}

public void btn_SendMaxPositive_d()
{
  packageMC maxPositivePack = send_package(SendMaxPositive, null, "Получить максимально возможную положительную накрутку");
  if (maxPositivePack.Data != null)
  {
    int maxPositive = JoinArray(maxPositivePack.Data, (byte)maxPositivePack.Data.length);
    println(maxPositive);
  }
}

public void byte_SendMaxNegative_k()
{
  packageMC maxNegativePack = send_package(SendMaxNegative, null, "Получить максимально возможную отрицательную накрутку");
  if (maxNegativePack.Data != null)
  {
    int maxNegative = JoinArray(maxNegativePack.Data, (byte)maxNegativePack.Data.length);
    println(maxNegative);
  }
}

////////////////////// Шифрование /////////////////
public void btn_GenerateKey_4()
{
  char[] keyArr = send_package(GenerateKey, null, "Получить ключ шифрования").Data;
  if (keyArr.length != 0)
  {
    printCharArrayINT(keyArr);
    for (int i = 0; i < keyArr_test.length; i++)
      keyArr_test[i] = keyArr[i];
  }
}

///////////////////////////////////////////////////

public void btn_get_formatting_reason_r()
{
  byte get_formatting_reason = 'r';
  send_package(get_formatting_reason, null, "Получить причину форматирования");
}

public void btn_time_test_t()
{
  output = createWriter("log" + PC_UTS() + ".txt");   // файл
  boolean isSync = false;
  int counter = 0;

  output.println("start: " + PC_UTS());
  output.flush();

  // sync while PCtime != MCtime
  while (!isSync)
  {
    println("     sync " + counter);
    btn_Timing_i();

    println("PC time" + PC_UTS());
    println("MC time" + MC_UTS());

    // check MCtime == PCtime
    // if MCtime != PCtime then sync    
    if (MC_UTS() != PC_UTS())
      isSync = false;
    else 
    isSync = true;

    delay(1000);
    counter++;
  }

  println("              syncEND ");

  while (true)
  {
    // if MCtime < PCtime then mc_Sec++;
    // if MCtime > PCtime then mc_Sec--;
    int pc_utc = PC_UTS();
    int mc_ust = MC_UTS();
    println("PC time____________________: " + pc_utc + "\nMC time____________________: " + mc_ust);
    println();

    output.println("pc_uts: " + pc_utc + " mc_uts: " + mc_ust + " result (pc_uts - mc_time): " + (pc_utc - mc_ust) );
    output.flush();

    delay(10000);
  }
}

// Отправить группу команд без даннх
public void btn_Send_Command_Line()
{
  String command;
  command = tbx_cmdLine.getText();
  for (int i = 0; i < command.length(); i++)
  {
    String oneCommand = command.substring(i, i+1);
    char[] res = send_package((byte)oneCommand.charAt(0), null, "").Data;

    for (int j = 0; j < res.length; j++)
      print(res[j]);
  }
}

// Серийный номер
public void btn_set_serialNum_w()
{
  char serNumArr[] = tbx_serial_num.getText().toCharArray();
  if (serNumArr.length == 0)
  {
    println("Серийный номер не заполнен.");
    return;
  }

  send_package(writeSerNum, serNumArr, "Установка серийного номера");
}

public void btn_get_serialNum_s()
{
  char[] sNum = send_package(getSerNum, null, "Получить серийный номер").Data;

  if (sNum != null && sNum.length > 1)
  {
    for (int i = 0; i < 10; i++)
    {
      print(sNum[i]);
    }
  }
}

// Время лицензии
public void btn_set_endLicenseTime_T()
{    
  int endLicenseTime = Integer.parseInt(tbx_end_license_time.getText());
  char[] licTimeArr = SplitArray_new(endLicenseTime);    
  packageMC licTimePack = send_package(set_endLicenseTime, licTimeArr, "Установка даты окончания лицензии " + endLicenseTime);
  if (licTimePack.Response != 0) println("Время окончания лицензии не установлено!!!!!!!!");
}

public void btn_get_endLicenseTime_e()
{
  char[] licTimeArr = send_package(get_endLicenseTime, null, "Получить время окончания лицензии").Data;

  if (licTimeArr != null && licTimeArr.length > 1)
  {
    int res = JoinArray(licTimeArr, (byte)licTimeArr.length);
    println(res);
  }
}

public void btn_Action_u()
{
  char[] resultArr = send_package(action, null, "Действие").Data;
  if (resultArr != null)
    printCharArrayINT(resultArr);
}

public void btn_SetActionMaxQuontity_h()
{
  int maxActionQty = 752;
  char[] MaxQuontity = SplitArray(maxActionQty, 2);
  packageMC maxActionQtyPack = send_package(SetActionMaxQuontity, MaxQuontity, "Установка максимального количества бесед в сутки " + maxActionQty);
  if (maxActionQtyPack.Response != 0) println("Максимальное количество бесед не установлено!!!!!!!!");
}

public void btn_GETActionMaxQuontity_g()
{
  char[] maxQtyArr = send_package(GetActionMaxQuontity, null, "Получить максимальное число действий в сутки").Data;
  int maxQty = JoinArray(maxQtyArr, (byte)2);
  println(maxQty);
}

public void btn_SetActionTimeCorrector_q()
{
  int timeCorrector = 14400;  // 14400 +4   часа. GMT+3 для Москвы
  char[] timeCorrectorArr = SplitArray_new(timeCorrector);
  send_package(SetActionTimeCorrector, timeCorrectorArr, "Установить корректор времени");
}

public void btn_GetActionTimeCorrector_j()
{
  char[] TimeCorrectorArr = send_package(GetActionTimeCorrector, null, "Получить корректор времени").Data;
  int TimeCorrector = JoinArray(TimeCorrectorArr, (byte)TimeCorrectorArr.length);
  println(TimeCorrector);
}

public void btn_RemainCurrentQtyAction_r()
{
  char[] remainQtyArr = send_package(GetRemainCurrentQtyAction, null, "Получить оставшееся количество действий на текущие сутки").Data;
  int remainQty = JoinArray(remainQtyArr, (byte)remainQtyArr.length);
  println(remainQty);
}

// База данных
public void btn_DB_Read_All_5()
{
  char[] dataBase = send_package(DB_Read_All, null, "Считать всю БД").Data;
  if (dataBase != null)
    printCharArrayINT(dataBase);
}

public void btn_DB_SetOneVal_6()
{
  for (int i = 0; i < 32; i++)
  {
    send_package(DB_SetOneVal, new char[]{(char)i, (char)(i+100)}, "Записать значение в БД");
  }
}

public void btn_DB_GetOneVal_7()
{
  char[] dataBase = new char[32];
  for (int i = 0; i < 32; i++)
  {
    char[] inKeyVal = send_package(DB_GetOneVal, new char[]{(char)i}, "Считать значение из БД").Data;
    if (inKeyVal != null && inKeyVal.length != 0)
      dataBase[i] = inKeyVal[0];
  }
  printCharArrayINT(dataBase);
}

///////////// Посчитать по формуле  ////////////////////////////

public void btn_compute_f_wzh()
{
  // f0 = 0.2148 * 5 / 7    = 0.1534
  // f1 = 0.2571 * 5 / 7    = 0.1836
  // f3 = 0.4781 * 104 / 57 = 0.8723
  // 0.1534 + 0.1836 + 0.8723 = 1.2093
  char fCount = 3;
  char[][] outArr = new char[fCount][3];  // 3 группы по 3 числа. Где каждое число будет занимать

  outArr[0][0] = 2148;  //w = 0.2148 
  outArr[0][1] = 5;     //z
  outArr[0][2] = 7;     //h

  outArr[1][0] = 2571;  //w = 0.2571 
  outArr[1][1] = 5;     //z
  outArr[1][2] = 7;     //h

  outArr[2][0] = 4781;  //w = 0.4781 
  outArr[2][1] = 104;   //z
  outArr[2][2] = 57;    //h

  char[] wzhArr = prepareArray(outArr, fCount);
  d_println(wzhArr.length + "");
  printCharArrayINT(wzhArr);

  char[] resultArr = send_package(compute_f_wzh, wzhArr, "Посчитать wzh").Data;
  if (resultArr.length != 0)
  {
    int res = JoinArray(resultArr, (byte)5);
    //double fl = res/10000;
    d_println(res + "");
  }
}

char[] prepareArray(char[][] outArr, char fCount)
{
  char wzh_arraySize = (char)(fCount * 9);
  char[] wzh_array = new char[wzh_arraySize];

  for (char fc = 0; fc < fCount; fc++)
  {
    char[] massW = SplitArray(outArr[fc][0], 3);
    char[] massZ = SplitArray(outArr[fc][1], 3);
    char[] massH = SplitArray(outArr[fc][2], 3);

    wzh_array[0 + fc * 9] = massW[0];
    wzh_array[1 + fc * 9] = massW[1];
    wzh_array[2 + fc * 9] = massW[2];

    wzh_array[3 + fc * 9] = massZ[0];
    wzh_array[4 + fc * 9] = massZ[1];
    wzh_array[5 + fc * 9] = massZ[2];

    wzh_array[6 + fc * 9] = massH[0];
    wzh_array[7 + fc * 9] = massH[1];
    wzh_array[8 + fc * 9] = massH[2];
  }

  return wzh_array;
}

///////////////////////////////////////////////////////////

// Тест
public void btn_test()
{
  //send_package((byte)'*', null, "Запрос zeroUTS");
  
  //send_package((byte)'&', null, "Запрос addKey");
  
  send_package((byte)'@', null, "Запрос endDayTime");
  
  /*byte[] ProKey_PC_byteArr = GenerateLFSRcode(28);
  for(int i = 0; i < ProKey_PC_byteArr.length; i++)  {    print((int)ProKey_PC_byteArr[i]);  }
  println();*/
}


////////////////////////
// Контроль напряжений
void btn_SetHallWay_h()
{
  char[] hallWay = {  0, 40, 50, 60, 70, 80, 90, 100, // chargeMin
    70, 140, 150, 160, 170, 180, 180, 180, // chargeMax
    200, 200, 200, 200, 200, 200, 190, 190, // dischargeMin
    255, 255, 255, 255, 255, 255, 255, 255};  // dischargeMax
  send_package(setHallWay, hallWay, "Записать значения коридора в EEPROM");
}

void btn_GetHallWay_g()
{
  String val[] = {"chargeMin", "chargeMax", "dischargeMin", "dischargeMax"};
  char[] hallWay = send_package(getHallWay, null, "Считать значения коридора").Data;
  if (hallWay != null)
  {
    for (int i = 0; i < 4; i++)
    {
      print(val[i] + ": ");
      for (int j = 0; j < 8; j++)
      {
        print((int)hallWay[j+ (i * 8)] + " ");
      }
      println();
    }
  }
}

void btn_enable_Check_Voltage_Flag_v()
{
  liteDT(enableCheckVoltageFlag);
}