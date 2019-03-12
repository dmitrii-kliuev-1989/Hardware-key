//______________________________________________________________________________________________________
//__________________________________  Функции  _________________________________________________________
//______________________________________________________________________________________________________

char[] SplitArray(int timeStamp, int size)
{
  int divisor = 1;
  int subtrahend;
  int tmpTime;
  int time = timeStamp;
  char mass[] = new char[size];

  for (int i = 1; i < size; i++)
    divisor *= 100;

  /*println();
   println("divisor " + divisor);
   println("size " + size);*/

  for (int i = 0; i < size; i++)
  {
    if (i != 0)
    {
      subtrahend = mass[i - 1] * divisor;
      time -= subtrahend;
      divisor /= 100;
    }

    tmpTime = time / divisor;
    mass[i] = (char)tmpTime;
  }

  return mass;
}

char[] SplitArray_new(int timeStamp)
{
  int divisor = 1;
  int multiplier = 1;
  int prevTime = 0;
  int item;
  int time = timeStamp;

  divisor = 100;
  multiplier = 100;

  int size = 0;
  int tmpTime = timeStamp;
  while (tmpTime != 0)
  {
    tmpTime /= divisor;
    size++;
  }
  
  char[] mass = new char[size];

  for (int i = size - 1; i >= 0; i--)
  {
    if (i == size - 1)
      prevTime = timeStamp;
    else
      prevTime = prevTime / divisor;


    time = time / divisor;
    item = prevTime - time * multiplier;
    mass[i] = (char)item;
  }

  return mass;
}

int JoinArray(char[] mass, byte size)
{
  if (mass != null && mass.length != 0 )
  {
    int joinTS = 0;
    int multiplier = 1;
    int i;   

    for (int x = 1; x < size; x++)
      multiplier *= 100;

    for (i = 0; i < size; i++)
    {
      if (i != 0) multiplier /= 100;
      joinTS += mass[i] * multiplier;
    }

    return joinTS;
  }
  return 0;
}

long JoinArrayLong(char[] mass, byte size)
{
  if (mass != null && mass.length != 0 )
  {
    long joinTS = 0;
    long multiplier = 1;
    int i;   

    for (int x = 1; x < size; x++)
      multiplier *= 100;

    for (i = 0; i < size; i++)
    {
      if (i != 0) multiplier /= 100;
      joinTS += mass[i] * multiplier;
    }

    return joinTS;
  }
  return 0;
}

public void printCharArrayINT(char[] array)
{
  if (array != null)
  {
    for (int i = 0; i < array.length; i++)
    {
      d_print((int)array[i] + " ");
    }
    d_println("");
  }
  else
    d_println("array is null.");  
}

public void printByteArray(byte[] array)
{
  if (array != null)
  {
    for (int i = 0; i < array.length; i++)
    {
      d_print((int)array[i] + " ");
    }
    d_println("");
  }
  else
    d_println("array is null.");  
}

public void printCharArrayNoDebug(char[] array)
{
  if (array != null)
  {
    for (int i = 0; i < array.length; i++)
    {
      print((int)array[i] + " ");
    }
    println("");
  }
}

public void printHexArray(byte[] array)
{
  if (array != null)
  {
    for (int i = 0; i < array.length; i++)
    {
      d_print(hex(array[i]) + " ");
    }
  }
}

public void printCharArrayReverse(byte[] array)
{
  if (array != null)
  {
    for (int i = array.length - 1; i >= 0; i--)
    {
      d_print(array[i] + " ");
    }
  }
}

void printCharArrayInBit(char[] arr)
{
  for(int i = 0; i < arr.length; i++)
  {
    d_print(String.format("%8s", Integer.toBinaryString(arr[i])).replace(' ', '0') + " ");
  }
}

public int PC_UTS()
{
  return (int)(System.currentTimeMillis()/1000);
}

public int MC_UTS()
{
  char[] syncTimeArr = send_package(get_time, null, "Получить текущее время МК").Data;
  int mc_time = JoinArray(syncTimeArr, (byte)syncTimeArr.length); 
  return mc_time;
}

// Поздороваться с МК. Вернуть true если МК поздоровался в ответ
public boolean SendHandshake()
{
  // отправить handshake
  myPort.write(handshake); 

  if (!PortWait(10)) // ждем ответа.
    return false;  // если не дождались
  byte incomingHandShake = (byte)myPort.read();
    
  if (incomingHandShake == handshake)
    return true;
  else
    return false;
}

void d_print(String str)
{
  if (DEBUG)
    print(str);
}

void d_println(String str)
{
  if (DEBUG)
    println(str);
}


byte[] split_int(long num)
{
  long n = num;
  int numQty = 0;

  // посчитать кол-во цифр в числе
  while (n != 0) { 
    n /= 10; 
    numQty++;
  }

  byte[] c = new byte[numQty];

  for (int i = 0; i < numQty; i++)
  {
    c[i] = (byte)(num % 10);
    num = num / 10;
  }

  byte t;
  //инвертируем массив символов
  for (int i = 0; i < numQty / 2; i++)
  {
    t = c[i];
    c[i] = c[numQty - 1 - i];
    c[numQty - 1 - i] = t;
  }

  return c;
}