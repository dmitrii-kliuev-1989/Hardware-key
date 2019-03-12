rem #detect signature
avrdude.exe -c stk500v1 -p m32u4 -P COM3 -n -b 19200
rem #burn bootloader
avrdude.exe -c stk500v1 -p m32u4 -P COM3 -b 19200 -e -U flash:w:"!Project\bootloader\bootloader\BootLoader.hex":a
rem #burn main program
avrdude.exe -c stk500v1 -p m32u4 -P COM3 -b 19200 -D -U flash:w:"!Project\App\caterina\Caterina.hex":a -U lfuse:w:0xFF:m -U hfuse:w:0xD8:m -U efuse:w:0xC8:m -U lock:w:0x3C:m

pause