# MDISP++ demonstration
MDISP ++ is a graphics library that supports multiple monochrome graphics displays. It is written in C++ and is a development of the MDISP graphics library. 

This demo project uses an STM32F103C8T as a controller to which four 128x64 SSD1306 based screens are connected.
 
This project created in EmBitz IDE.

## Click below to watch video on youtube:
[![Watch the video](./screenshot1.jpg)](https://www.youtube.com/watch?v=l5EkvUo-8eM)

## Screens connections to the MC GPIO pins:

Screen #  | Interface | I²C Address | MC Pins
----------|-----------|-------------|--------
 All @ SPI|           |             | RST: A12 
 1        |   SPI2    |             | SCK: B13 <br>MOSI: B15 <br>MISO: B14 <br>CS: A10 <br>DC: A11 
 2        |   I²C2    |   0x3c      | SDA: B11<br>SCL: B10
 3        |   I²C1    |   0x3c      | SDA: B7<br>SCL: B6
 4        |   I²C1    |   0x3d      | SDA: B7<br>SCL: B6
 
 
 

