import processing.serial.*;
import controlP5.*;
ControlP5 cp5;
PrintWriter file;

void setup() 
{
  file = createWriter("out.txt");
  btn_test();
  exit();

  size(200, 150);
  cp5 = new ControlP5(this);
  PFont font = createFont("arial", 50);

  // Инициализировать
  cp5.addBang("btn_test")     .setPosition(10, 10).setSize(140, 20).getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER);

  textFont(font);
}

void draw()
{
  background(0);
}

byte g_LFSRkey = 125;
byte g_LFSRtakt = 114;

public void btn_test()
{
  /*
    2503935002
   2503935002
   */
  println("GO");
  long test = LFSR(g_LFSRkey, g_LFSRtakt);
  println(test);
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

char[] split_int(long n)
{
  char[] c = new char[10];
  int v = 0; //количество цифр в числе n
  //разбиваем на отдельные символы число n
  while (n > 9)
  {
    c[v++] = (char) ((n % 10) + '0');
    n = n / 10;
  }
  c[v++] = (char) (n + '0');
  c[v] = '\0';
  char t;
  //инвертируем массив символов
  for (int i = 0; i < v / 2; i++)
  {
    t = c[i];
    c[i] = c[v - 1 - i];
    c[v - 1 - i] = t;
  }
  v = 0;

  return c;
}

/*long LFSR(long key, long takt) //функция возвращает значение LFSR при данном ключе и на заданном такте
 {
 if (key == 0) return -1;  // ошибка
 long ShiftRegister = key; // Все, кроме 0.
 for (int i = 0; i < takt; i++)
 {
 ShiftRegister = ((
 ((ShiftRegister >> 2)     // 32
 ^ (ShiftRegister >> 6)      // 7
 ^ (ShiftRegister >> 4)      // 5
 ^ (ShiftRegister >> 255)      //
 ^ (ShiftRegister >> 2)      // 3
 ^ (ShiftRegister >> 1)      // 2
 ^ (ShiftRegister >> 4)      // 
 ^ (ShiftRegister >> 0)      // 
 ^ (ShiftRegister))         // 1
 & 0x00000001)
 << 31) | (ShiftRegister >> 1);
 }
 return ShiftRegister;
 }*/

int takt[] = new int[16]; //запоминать предыдущее состояние всех 16 LFSR - на сколько оборотом провращался регистр.
int key[] = { 94, 12395, 942, 65736, 275, 1431, 65337, 27724, 24000, 38294, 929483, 54305, 20900, 2, 420, 50284 }; //начальные позиции каскада голлмана
String result2 = "";
String t;
long shiftreg;
int MAXLENGTH = 5554;  //Длина тестовой последовательности. 

String GOLLMANN(int deep/*глубина рекурсии*/)
{
  long ShiftRegister = key[deep]; /* Все, кроме 0. */
  for (int i = 0;; i++)
  {
    ShiftRegister = LFSR(key[deep], takt[deep]);
    ShiftRegister = ((
      ((ShiftRegister >> 31)     // 32
      ^ (ShiftRegister >> 6)      // 7
      ^ (ShiftRegister >> 4)      // 5
      ^ (ShiftRegister >> 2)      // 3
      ^ (ShiftRegister >> 1)      // 2
      ^ (ShiftRegister))         // 1
      & 0x00000001)
      << 31) | (ShiftRegister >> 1);
    takt[deep]++;
    t = DecToBin(ShiftRegister);

    if (t.charAt(t.length() - 1) == '1' && deep < 15)
    {
      GOLLMANN(deep + 1);   //рекурсия
    } else
    {
      if (deep == 15)
      {
        result2 += t.charAt(t.length() - 1);
      } else
      {
        shiftreg = LFSR(key[15], takt[15]);
        t = DecToBin(shiftreg);
        result2 += t.charAt(t.length() - 1);
      }
    }
    if (result2.length() >= MAXLENGTH) 
      break;
  }

  return t;
}

String DecToBin(long number)
{
  String result = "";

  do
  {
    if ((number & 1) == 0)
      result += "0";
    else
      result += "1";

    number >>= 1;
  } 
  while (number > 0);

  result = reverseIt(result);
  //reverse(result, result);
  return result;
}

public static String reverseIt(String source) 
{
  int i, len = source.length();
  StringBuilder dest = new StringBuilder(len);

  for (i = (len - 1); i >= 0; i--) {
    dest.append(source.charAt(i));
  }

  return dest.toString();
}