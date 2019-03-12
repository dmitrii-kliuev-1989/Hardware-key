import controlP5.*; //<>//
import processing.serial.*;
import g4p_controls.*;
import java.util.Arrays;
PrintWriter output;

ControlP5 cp5;
Serial myPort;

boolean DEBUG = true;

int ProgramCounter = 0;

void draw()
{
  background(0);
}

void setup()
{
  // автоматический выбор порта с ключом
  int portNum = 0;
  boolean result = false;
  printArray(Serial.list());
  for (int i = 0; i < Serial.list().length; i++)
  {
    myPort = new Serial(this, Serial.list()[i], 9600);
    result = SendHandshake();
    if (result)
    {
      if (!PortWait(10)) return;
      byte incomingByte = (byte)myPort.read();
      println("TimeOutError  = " + hex(incomingByte));
      portNum = i;
      break;
    }
  }

  if (result)
    println("Key COM port = ", Serial.list()[portNum]);
  else
  {
    println("Порт с ключом не найден!!!");
    exit();
  }
  
  //boolean runTest = true;
  boolean runTest = false;

  size(1000, 900);


  if (runTest)
  {
    //DEBUG = false;
    keyArr_test = GenerateDefaultKey("QWEDFLKNLDDFLKGN4K3J4G93JG0J34JNG3NRSGDF");

    int pc_utc = PC_UTS();
    int mc_ust = MC_UTS();
    println("PC time: " + pc_utc + "\nMC time: " + mc_ust);

    h_test();
  } else
  {
    // Интерфейс
    Controls();
  }
}

GTextField tbx_cmdLine;


GTextField tbx_password;
GTextField tbx_serial_num;
GTextField tbx_end_license_time;
GTextField tbx_max_Action_Qty;
GTextField tbx_GMT_Time_Corrector;
GTextField tbx_LFSR_init_arr;
GTextField tbx_remote_cmd;
GTextField tbx_remote_response;

GLabel lbl_password;
GLabel lbl_serial_num;
GLabel lbl_end_license_time;
GLabel lbl_max_Action_Qty;
GLabel lbl_GMT_Time_Corrector;
GLabel lbl_LFSR_init_arr;
GLabel lbl_remote_cmd;
GLabel lbl_remote_response;

public void Controls() 
{
  tbx_cmdLine = new GTextField(this, 10, 10, 180, 20);


  // Параметры, необходимые для использования рабочих функций МК
  tbx_password =                 new GTextField(this, 600, 90, 390, 20);
  tbx_serial_num =               new GTextField(this, 600, 115, 390, 20);
  tbx_end_license_time =         new GTextField(this, 600, 140, 390, 20);
  tbx_max_Action_Qty =           new GTextField(this, 600, 165, 390, 20);
  tbx_GMT_Time_Corrector =       new GTextField(this, 600, 190, 390, 20);
  tbx_LFSR_init_arr =            new GTextField(this, 600, 215, 390, 20);
  tbx_remote_cmd =               new GTextField(this, 600, 550, 390, 20);
  tbx_remote_response =          new GTextField(this, 600, 625, 390, 20);
  

  G4P.setGlobalColorScheme(8);
  lbl_password =                 new GLabel(this, 350, -75, 250, 350, "Пароль");
  lbl_serial_num =               new GLabel(this, 350, -50, 250, 350, "Серийный номер");
  lbl_end_license_time =         new GLabel(this, 350, -25, 250, 350, "Дата окончания лицензии");
  lbl_max_Action_Qty =           new GLabel(this, 350, 0, 250, 350, "Максимальное кол-во бесед в сутки");
  lbl_GMT_Time_Corrector =       new GLabel(this, 350, 25, 250, 350, "Корректор времени для GMT");
  lbl_LFSR_init_arr =            new GLabel(this, 350, 50, 250, 350, "Массив инициализации LFSR");  
  lbl_remote_cmd =               new GLabel(this, 350, 385, 250, 350, "Удаленная команда");
  lbl_remote_response =          new GLabel(this, 350, 460, 250, 350, "Удаленный ответ");
  

  lbl_password.setTextAlign(GAlign.RIGHT, null);
  lbl_serial_num.setTextAlign(GAlign.RIGHT, null);
  lbl_end_license_time.setTextAlign(GAlign.RIGHT, null);
  lbl_max_Action_Qty.setTextAlign(GAlign.RIGHT, null);
  lbl_GMT_Time_Corrector.setTextAlign(GAlign.RIGHT, null);
  lbl_LFSR_init_arr.setTextAlign(GAlign.RIGHT, null);
  lbl_remote_cmd.setTextAlign(GAlign.RIGHT, null);
  lbl_remote_response.setTextAlign(GAlign.RIGHT, null);

  tbx_password.setText("qwedflknlddflkgn4k3j4g93jg0j34jng3nrsgdf".toUpperCase());
  tbx_serial_num.setText("0705160102");
  tbx_end_license_time.setText(Integer.toString(PC_UTS() + 86400 * 31));  // кол-во дней 86400 * 31
  tbx_max_Action_Qty.setText("40");
  tbx_LFSR_init_arr.setText("11711,20,17,11,3,8,22,9,5");
  tbx_GMT_Time_Corrector.setText("10800");
  tbx_remote_cmd.setText("");


  cp5 = new ControlP5(this);
  PFont font = createFont("arial", 50);

  // Инициализировать
  cp5.addBang("btn_init")                         .setPosition(850, 240).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  
  cp5.addBang("btn_send_bad_remote_cmd")          .setPosition(200, 600).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  
  // Выполнение удаленной команды
  cp5.addBang("btn_execute_remote_cmd")           .setPosition(850, 575).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Поздороваться
  cp5.addBang("btn_handshake_test_z")             .setPosition(10, 50).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // тест
  cp5.addBang("btn_Send_Command_Line")            .setPosition(200, 10).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Работа с паролем
  cp5.addBang("btn_Send_true_Password_A")         .setPosition(10, 100).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_Send_bad_Password_A")          .setPosition(10, 125).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_set_password_P")               .setPosition(10, 150).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_use_default_key_Y")            .setPosition(10, 200).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER); 


  // Работа со временем
  cp5.addBang("btn_Timing_i")                     .setPosition(200, 100).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_time_test_t")                  .setPosition(200, 125).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_get_mc_time")                  .setPosition(200, 150).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_GetSynchroTimeFromEEPROM_i")   .setPosition(200, 175).setSize(170, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_GetPlusTimeMistake_F")         .setPosition(200, 225).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_SendMinusTimeMistake_m")       .setPosition(200, 275).setSize(170, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_SendMaxPositive_d")            .setPosition(200, 325).setSize(170, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("byte_SendMaxNegative_k")           .setPosition(200, 350).setSize(170, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Работа с флэш
  cp5.addBang("btn_FLASH_FORMAT_F")               .setPosition(400, 10).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_get_formatting_reason_r")      .setPosition(400, 35).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Серийный номер
  cp5.addBang("btn_set_serialNum_w")              .setPosition(600, 10).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_get_serialNum_s")              .setPosition(600, 35).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Время лицензии
  cp5.addBang("btn_set_endLicenseTime_T")         .setPosition(800, 10).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_get_endLicenseTime_e")         .setPosition(800, 35).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Action. Обследование/беседа с проверяемым
  cp5.addBang("btn_Action_u")                     .setPosition(10, 300).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_SetActionMaxQuontity_h")       .setPosition(10, 325).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_GETActionMaxQuontity_g")       .setPosition(10, 350).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_SetActionTimeCorrector_q")     .setPosition(10, 375).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_GetActionTimeCorrector_j")     .setPosition(10, 400).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_RemainCurrentQtyAction_r")     .setPosition(10, 425).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Корридоры напряжений
  cp5.addBang("btn_SetHallWay_h")                 .setPosition(10, 500).setSize(170, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_GetHallWay_g")                 .setPosition(10, 525).setSize(170, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_enable_Check_Voltage_Flag_v")  .setPosition(10, 550).setSize(170, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Шифрование
  cp5.addBang("btn_GenerateKey_4")                .setPosition(200, 425).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_encryption_test")              .setPosition(200, 450).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // База данных
  cp5.addBang("btn_DB_Read_All_5")                .setPosition(600, 250).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_DB_SetOneVal_6")               .setPosition(600, 275).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_DB_GetOneVal_7")               .setPosition(600, 300).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Посчитать по формуле
  cp5.addBang("btn_compute_f_wzh")                .setPosition(850, 300).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_heavy_test")                   .setPosition(850, 325).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Дистанционное продление лицензии
  cp5.addBang("btn_set_LFSR_init_Arr_b")          .setPosition(600, 375).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_Get_Prolongation_Key_PC")      .setPosition(600, 400).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);  
  cp5.addBang("btn_LFSR_encrypt")                 .setPosition(600, 425).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_LFSR_decrypt")                 .setPosition(600, 450).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  cp5.addBang("btn_test")                         .setPosition(200, 50).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  
  cp5.addBang("btn_Set_Ip_Address")               .setPosition(400, 425).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_Get_Ip_Address")               .setPosition(400, 450).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_Set_Port")                     .setPosition(400, 475).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_Get_Port")                     .setPosition(400, 500).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  // Логика. дни/обследования
  cp5.addBang("btn_Set_Logic_days")               .setPosition(400, 250).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_Set_Logic_actions")            .setPosition(400, 275).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_change_Logic_days")            .setPosition(400, 300).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_change_Logic_actions")         .setPosition(400, 325).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_get_logic")                    .setPosition(400, 350).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  
  
  cp5.addBang("btn_block")                        .setPosition(200, 500).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_get_block_reason")             .setPosition(200, 525).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_Unlock")                       .setPosition(200, 550).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
    
  cp5.addBang("btn_prolongation")                 .setPosition(200, 625).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  
  cp5.addBang("btn_get_Actions_Or_Days_cmd")      .setPosition(200, 675).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  
  cp5.addBang("btn_Generate_LFSR_code")           .setPosition(200, 800).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("btn_LFSR_Equally_TEST")            .setPosition(200, 825).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);


  textFont(font);
  text("word", 10, 50);
}

public void handleTextEvents(GEditableTextControl textControl, GEvent event) {
}