import processing.serial.*;
import controlP5.*;
ControlP5 cp5;
PrintWriter file;
byte handshake =                      'Z';       // поздороваться

Serial myPort;  // The serial port

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
      if (!PortWait()) return;
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

  size(200, 150);
  cp5 = new ControlP5(this);
  PFont font = createFont("arial", 50);

  // Инициализировать
  cp5.addBang("charge")     .setPosition(10, 10).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
  cp5.addBang("discharge")  .setPosition(10, 40).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);
}

char MeasureVoltage_charge = 'c';
char MeasureVoltage_discharge = 'd';

void charge()
{
  file = createWriter("measureCharge.csv");
  println("GO");
  measure(MeasureVoltage_charge);
}

void discharge()
{
  file = createWriter("measureDisCharge.csv");
  println("GO");
  measure(MeasureVoltage_discharge);
}

void measure(char measureType)
{
  int  i = 0;
  while (true)
  {
    myPort.write(measureType);
    if (!PortWait()) return;
    
    String line = "";
    print(i + ":\t");
    while(myPort.available() != 0)
    {
      int inByte = myPort.read();
      print((int)inByte + " ");
      line += (int)inByte + ";";
    }
    
    file.println(line);
    file.flush();
    println();
    i++;
  }
}

void draw() 
{  
  background(0);
}

// Поздороваться с МК. Вернуть true если МК поздоровался в ответ
public boolean SendHandshake()
{
  // отправить handshake
  myPort.write(handshake); 

  if (!PortWait()) // ждем ответа.
    return false;  // если не дождались
  byte incomingHandShake = (byte)myPort.read();

  if (incomingHandShake == handshake)
    return true;
  else
    return false;
}

public boolean PortWait()
{
  // ждем ответа
  int waitCounter = 0;
  boolean wait = true;
  while (myPort.available() == 0 && wait)
  {
    delay(10);
    waitCounter++;
    if (waitCounter > 100) wait = false;
  }
  
  return wait;
}