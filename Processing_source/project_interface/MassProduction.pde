public void btn_init()
{
  // 1)
  LFSRinit();

  // 2)  
  SetLogic(g_daysLogic);
  //SetLogic(g_actionsLogic);

  // 3)
  TimeSync();

  // 4)
  SetPassword();

  keyArr_test = GenerateDefaultKey(tbx_password.getText());

  // 5) 
  SetSerialNum();

  // 6)
  SetEndLicenseTime(Integer.parseInt(tbx_end_license_time.getText()));
  //SetEndLicenseTime(0);

  // 7)
  SetMaxActionQty();

  // 8)
  SetGMT_Time_Corrector();
}

public void LFSRinit()
{
  String LFSRinitStr = tbx_LFSR_init_arr.getText();
  String[] LFSRinitStrArr = LFSRinitStr.split(",");
  char[] LFSRshiftByteArr = new char[8];

  int l_key = Integer.parseInt(String.valueOf(LFSRinitStrArr[0]));
  println(l_key);

  for (int i = 0; i < LFSRshiftByteArr.length; i++) 
    LFSRshiftByteArr[i] = (char)(Integer.parseInt(String.valueOf(LFSRinitStrArr[i+1])));


  byte[] l_LFSRshiftArr = new byte[8];
  for (int i = 0; i < l_LFSRshiftArr.length; i++) l_LFSRshiftArr[i] = (byte)LFSRshiftByteArr[i]; 

  char[] initArrInChar = LFSRinitArrToCharArray(l_key, l_LFSRshiftArr);
  
  for (int i = 0; i < initArrInChar.length; i++) print((int)initArrInChar[i] + " ");

  println();
  println();

  packageMC initLFSR = send_package(setLFSRinitArr, initArrInChar, "Инициализировать LSFR");

  g_key = l_key;
  g_LFSRshiftArr = l_LFSRshiftArr;

  printByteArray(g_LFSRshiftArr);

  if (initLFSR.Response != 0) println("LFSR не инициализирован!!!!!!!!");
}

public void TimeSync()
{
  int pc_uts = (int)(System.currentTimeMillis()/1000);
  char[] USTarr = SplitArray(pc_uts, 5);
  packageMC timeSync = send_package(timing, USTarr, "Синхронизация времени. " + pc_uts);

  if (timeSync.Response != 0) println("Время не синхронизировано!!!!!!!!");

  // Проверка. Время должно быть +-1 сек.
}

public void  SetPassword()
{
  String pswd = tbx_password.getText();
  char[] pswdArr = new char[pswd.length()];

  for (int i = 0; i < pswd.length(); i++)
  {  
    String oneChar = pswd.substring(i, i+1);
    pswdArr[i] = (char)oneChar.charAt(0);
  }

  packageMC password = send_package(set_pasword, pswdArr, "Установка пароля");

  if (password.Response != 0) println("Пароль не установлен!!!!!!!!");
}


public void Autorization()
{
  String pswd = tbx_password.getText();
  char[] pswdArr = new char[pswd.length()];

  for (int i = 0; i < pswd.length(); i++)
  {  
    String oneChar = pswd.substring(i, i+1);
    pswdArr[i] = (char)oneChar.charAt(0);
  }

  packageMC autirozation = send_package(send_password, pswdArr, "Авторизация");
  printCharArrayINT(autirozation.Data);
  if (autirozation.Response != 0) println("Авторизация не пройдена!!!!!!!!");
}

public void SetSerialNum()
{
  println();

  char serNumArr[] = tbx_serial_num.getText().toCharArray();
  packageMC serNumPack = send_package(writeSerNum, serNumArr, "Установка серийного номера " + tbx_serial_num.getText());

  if (serNumPack.Response != 0) println("Серийный номер не установлен!!!!!!!!");
}

public void SetEndLicenseTime(int elt)
{
  int endLicenseTime = elt;
  char[] licTimeArr = SplitArray_new(endLicenseTime);    
  packageMC licTimePack = send_package(set_endLicenseTime, licTimeArr, "Установка даты окончания лицензии " + endLicenseTime);
  if (licTimePack.Response != 0) println("Время окончания лицензии не установлено!!!!!!!!");
}

public void SetMaxActionQty()
{
  int maxActionQty = Integer.parseInt(tbx_max_Action_Qty.getText());
  char[] MaxQuontity = SplitArray(maxActionQty, 2);
  packageMC maxActionQtyPack = send_package(SetActionMaxQuontity, MaxQuontity, "Установка максимального количества бесед в сутки " + maxActionQty);
  if (maxActionQtyPack.Response != 0) println("Максимальное количество бесед не установлено!!!!!!!!");
}

public void SetGMT_Time_Corrector()
{
  int GMT_Time_Corrector = Integer.parseInt(tbx_GMT_Time_Corrector.getText());
  char[] timeCorrectorArr = SplitArray_new(GMT_Time_Corrector);
  packageMC correctorPack = send_package(SetActionTimeCorrector, timeCorrectorArr, "Установка корректора времени " + GMT_Time_Corrector);
  if (correctorPack.Response != 0) println("Корректор времени не установлен!!!!!!!!");
}