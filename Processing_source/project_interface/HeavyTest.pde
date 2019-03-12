public void btn_heavy_test()
{
  h_test();
}

public void h_test()
{
  long counter = 0;
  while (true)
  {
    char fCount = 3;
    char[][] outArr = new char[fCount][3];  // 3 группы по 3 числа. Где каждое число будет занимать

    outArr[0][0] = (char)(10 + Math.random() * 500);  //w = 0.2148 
    outArr[0][1] = (char)(10 + Math.random() * 50);     //z
    outArr[0][2] = (char)(10 + Math.random() * 50);     //h

    outArr[1][0] = (char)(10 + Math.random() * 500);  //w = 0.2571 
    outArr[1][1] = (char)(10 + Math.random() * 50);     //z
    outArr[1][2] = (char)(10 + Math.random() * 50);     //h

    outArr[2][0] = (char)(10 + Math.random() * 5000);  //w = 0.4781 
    outArr[2][1] = (char)(10 + Math.random() * 50);   //z
    outArr[2][2] = (char)(10 + Math.random() * 50);    //h

    char[] wzhArr = prepareArray(outArr, fCount);
    send_package(compute_f_wzh, wzhArr, "Посчитать wzh");
    
    counter++;

    if (counter % 100 == 0)
    {
      println(counter);
    }
    
    //delay(1000);
  }
}