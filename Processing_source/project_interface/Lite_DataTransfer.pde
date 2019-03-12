void liteDT(byte cmd)
{
    myPort.write(cmd);
    if (!PortWait(1000)) return;
    
    while(myPort.available() != 0)
    {
      int inByte = myPort.read();
      print((char)inByte + " ");
    }
}