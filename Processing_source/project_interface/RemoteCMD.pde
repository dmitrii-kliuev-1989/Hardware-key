/***********************************************************************************************/
/**************************** ip адрес и порт **************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
void btn_Set_Ip_Address()
{
  char[] ipPart0 = SplitArray(192, 2);
  char[] ipPart1 = SplitArray(168, 2);
  char[] ipPart2 = SplitArray(52, 2);
  char[] ipPart3 = SplitArray(211, 2);

  char[] ipAddressArr = { (char)setIpAddress_cmd, 
    ipPart0[0], ipPart0[1], 
    ipPart1[0], ipPart1[1], 
    ipPart2[0], ipPart2[1], 
    ipPart3[0], ipPart3[1]};

  char[] ipAddressEnc = LFSRencArr(ipAddressArr);

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  char[] ipAddrArrEncWOcmd = new char[64];
  for (int i = 0; i < 64; i++)
    ipAddrArrEncWOcmd[i] = ipAddressEnc[i + 8]; 

  char[] ipAddressArr2 = LFSRdecryptArr(ipAddrArrEncWOcmd);

  char[] ipAddr = new char[4];

  int idx = 0;
  for (int i = 0; i < ipAddr.length; i++)
  {
    if (i != 0) idx += 2;
    char[] tmp = new char[2];
    tmp[0] = ipAddressArr2[idx];
    tmp[1] = ipAddressArr2[idx + 1];
    ipAddr[i] = (char)JoinArray(tmp, (byte)tmp.length);
  }

  println();
  println();
  for (int i = 0; i < ipAddressArr.length; i++) print((int)ipAddressArr[i] + " "); 
  println();
  for (int i = 0; i < ipAddrArrEncWOcmd.length; i++) print((int)ipAddrArrEncWOcmd[i] + " "); 
  println();
  for (int i = 0; i < ipAddressArr2.length; i++) print((int)ipAddressArr2[i] + " "); 
  println();
  for (int i = 0; i < ipAddr.length; i++) print((int)ipAddr[i] + " "); 
  println();
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  send_package(remoteCMD, ipAddressEnc, "Установить ip адрес");
}

void btn_Get_Ip_Address()
{
  char[] ipAddrArray = send_package(getIpAddress, null, "Запросить ip адрес").Data;
  String ipAddr = "";
  for (int i = 0; i < ipAddrArray.length; i++)
  {
    ipAddr += (int)ipAddrArray[i];
    if (i != 3)
      ipAddr += ".";
  }

  print(ipAddr);
}

void btn_Set_Port()
{
  int port = 15211;
  char[] portArr = SplitArray(port, 3);
  char[] portArrWithCMD = {char(setPort_cmd), 
    portArr[0], 
    portArr[1], 
    portArr[2]};

  char[] portEnc = LFSRencArr(portArrWithCMD);
  for (int j = 0; j < portEnc.length; j++) print((int)portEnc[j] + " ");

  send_package(remoteCMD, portEnc, "Установить порт");
}

void btn_Get_Port()
{
  char[] portArr = send_package(getPort, null, "Запросить порт").Data;
  int port = JoinArray(portArr, (byte)portArr.length);
  println(port);
}

/***********************************************************************************************/
/**************************** Логика ***********************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
char g_daysLogic = 1;
char g_actionsLogic = 2;

void btn_change_Logic_days()
{
  ChangeLogic(g_daysLogic);
}

void btn_change_Logic_actions()
{
  ChangeLogic(g_actionsLogic);
}

void ChangeLogic(char logic)
{
  if (logic == g_daysLogic)
  {
    // Команда изменения логики
    // changeLogic_cmd

    // логика
    // logic

    // текущее время
    int pc_uts = (int)(System.currentTimeMillis()/1000);
    char[] USTarr = SplitArray(pc_uts, 5);

    // время окончания лицензии
    int endLicenseTime = Integer.parseInt(tbx_end_license_time.getText());
    char[] licenseTimeArr = SplitArray(endLicenseTime, 5);

    // количество анализов в сутки
    int maxActionQty = Integer.parseInt(tbx_max_Action_Qty.getText());
    char[] maxActionQtyArr = SplitArray(maxActionQty, 2);

    char changeLogicDaysArr[] = new char[14];
    changeLogicDaysArr[0] = (char)changeLogic_cmd;
    changeLogicDaysArr[1] = (char)logic;

    for (int i = 0; i < 5; i++) changeLogicDaysArr[i + 2] = USTarr[i];
    for (int i = 0; i < 5; i++) changeLogicDaysArr[i + 7] = licenseTimeArr[i];
    for (int i = 0; i < 2; i++) changeLogicDaysArr[i + 12] = maxActionQtyArr[i];

    char[] changeLogicDaysArrEnc = LFSRencArr(changeLogicDaysArr);
    send_package(remoteCMD, changeLogicDaysArrEnc, "Установить логику дни");

    for (int i = 0; i < changeLogicDaysArr.length; i++) print((int)changeLogicDaysArr[i] + " "); 
    println();
    for (int i = 0; i < changeLogicDaysArrEnc.length; i++) print((int)changeLogicDaysArrEnc[i] + " "); 
    println();
  }

  if (logic == g_actionsLogic)
  {
    // Команда изменения логики
    // changeLogic_cmd

    // логика
    // logic

    // Количество анализов
    int actionQty = Integer.parseInt(tbx_max_Action_Qty.getText());
    char[] actionQtyArr = SplitArray(actionQty, 2);

    char[] changeLogicActionsArr = new char[4];
    changeLogicActionsArr[0] = (char)changeLogic_cmd;
    changeLogicActionsArr[1] = (char)logic;

    for (int i = 0; i < 2; i++) changeLogicActionsArr[i + 2] = actionQtyArr[i];

    char changeLogicActionsArrEnc[] = LFSRencArr(changeLogicActionsArr);
    send_package(remoteCMD, changeLogicActionsArrEnc, "Изменить логику на обследования");

    for (int i = 0; i < changeLogicActionsArr.length; i++) print((int)changeLogicActionsArr[i] + " "); 
    println();
    for (int i = 0; i < changeLogicActionsArrEnc.length; i++) print((int)changeLogicActionsArrEnc[i] + " "); 
    println();
  }
}

void btn_get_logic()
{
  send_package(getLogic_cmd, null, "Получить логику");
}

//////////////////////////////////////////
void btn_Set_Logic_days()
{
  SetLogic(g_daysLogic);
}

void btn_Set_Logic_actions()
{
  SetLogic(g_actionsLogic);
}

void SetLogic(char logic)
{
  send_package(setLogic_cmd, new char[]{logic}, "Установка логики");
}

/***********************************************************************************************/
/********************** Запрос количества действий/анализов ************************************/
/***********************************************************************************************/
/***********************************************************************************************/
void btn_get_Actions_Or_Days_cmd()
{
  char[] getActionsOrDays_cmdArr = {(char)getActionsOrDays_cmd};
  char[] getActionsOrDays_cmdArrEnc = LFSRencArr(getActionsOrDays_cmdArr);
  char[] qtyArrEnc = send_package(remoteCMD, getActionsOrDays_cmdArrEnc, "Запрос количества действий/анализов").Data;
  char[] qtyArr = LFSRdecryptArr(qtyArrEnc);  
  int qty = JoinArray(qtyArr, (byte)2);
  println(qty);
}

/***********************************************************************************************/
/*************************** Блокировка/Разблокировка/Причина **********************************/
/***********************************************************************************************/
/***********************************************************************************************/
void btn_Unlock()
{
  char[] unlockArr = {(char)unlock_cmd};
  char[] unlockArrEnc = LFSRencArr(unlockArr);
  send_package(remoteCMD, unlockArrEnc, "Разблокировать ключ");
}

void btn_get_block_reason()
{  
  char[] blockReasonCmd = {(char)getBlockReason_cmd};
  char[] blockReasonCmdEnc = LFSRencArr(blockReasonCmd);
  packageMC blockReasonArrEnc = send_package(remoteCMD, blockReasonCmdEnc, "Получить причину блокировки");
  if (blockReasonArrEnc.Data.length != 0)
  {
    char[] qtyArr = LFSRdecryptArr(blockReasonArrEnc.Data);
    println((int)qtyArr[0]);
  }
}

void btn_send_bad_remote_cmd()
{
  char[] badCmd = {(char)150};
  char[] badCmdEnc = LFSRencArr(badCmd);
  send_package(remoteCMD, badCmdEnc, "Тест блокировки при ситуации когда не удалось расшифровать LFSR команду.");
}

void btn_block()
{
  send_package(block_cmd, null, "Заблокировать ключ для отладки");
}
/***********************************************************************************************/
/******************* Продление лицензии на количество дней/обследований ************************/
/***********************************************************************************************/
/***********************************************************************************************/
void btn_prolongation()
{
  int prolongationValue = 10;
  char[] prolongationValueArr = SplitArray(prolongationValue, 2);
  char[] prolongationArr = {(char)prolongation_cmd, 
    prolongationValueArr[0], 
    prolongationValueArr[1]};
  char[] prolongationArrEnc = LFSRencArr(prolongationArr);
  send_package(remoteCMD, prolongationArrEnc, "Продлить лицензию на количество дней/обследований");
}

/***********************************************************************************************/
/******************* Выполнение удаленной команды **********************************************/
/***********************************************************************************************/
/***********************************************************************************************/
void btn_execute_remote_cmd()
{  
  char[] sNumArr = send_package(getSerNum, null, "Получить серийный номер").Data;
  String serialNum = "";
  if (sNumArr != null && sNumArr.length > 1)
    serialNum = String.valueOf(sNumArr);

  String testRemoteCMD = tbx_remote_cmd.getText();
  String[] commands = testRemoteCMD.split(";");
  String response = "";
  for (int i = 0; i <commands.length; i++ )
  {
    String[] cmdParts = commands[i].split("#");
    String tag = cmdParts[0];
    String cmdStr = cmdParts[1];

    char[] remoteCmdArr = cmdStr.toCharArray();
    char[] remoteCmdByteArr = new char[remoteCmdArr.length];

    // toByteArr
    for (int j = 0; j < remoteCmdArr.length; j++) 
      remoteCmdByteArr[j] = (char)(Integer.parseInt(String.valueOf(remoteCmdArr[j])));

    packageMC result = send_package(remoteCMD, remoteCmdByteArr, "Выполнение удаленной команды");


    response += tag + "#" + serialNum + "#" + Byte.toString((byte)result.Response);

    if (result.Data != null && result.Data.length > 1)
    {
      response += "#";  
      for (int j = 0; j < result.Data.length; j++)
        response += Byte.toString((byte)result.Data[j]);
    }

    response += ";";
  }
  
  response = trimStringByString(response, ";");
  //tbx_remote_response.setText(response);
  println(response);
}

public static String trimStringByString(String text, String trimBy) 
{
  int beginIndex = 0;
  int endIndex = text.length();

  while (text.substring(beginIndex, endIndex).startsWith(trimBy)) 
  {
    beginIndex += trimBy.length();
  } 

  while (text.substring(beginIndex, endIndex).endsWith(trimBy)) 
  {
    endIndex -= trimBy.length();
  }

  return text.substring(beginIndex, endIndex);
}