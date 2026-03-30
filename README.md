# astraUIapplication
This project is an application of astraUI.astraUI is an UI framework which is highly compatible and portable for OLED screens,and was written by the Bilibili uploader "无理造物".

I’ve spent nearly half a year polishing this little weather clock project. Although the Wi-Fi part occasionally parses data incorrectly, it still feels like a really neat and well‑finished project to me. Unfortunately, I never managed to light up that RGB LED in the end—probably because it was my first time designing a PCB, and it turned out pretty messy, haha.
To build this project, you only need:
an STM32F103RCT6
a 7‑pin SPI OLED
a UART serial port for the ESP8266
an IIC interface for the MPU6050
two pushbuttons
That’s all you need to complete it.

It also includes another small project for a smart door lock based on the STM32F103ZET6.It supports:
Manual password entry
Bluetooth password unlocking
NFC unlocking
Fingerprint unlocking
Fingerprint disabling
NFC function control
Fingerprint enrollment and deletion
Password modification with power-off persistence
This is also a really fun and interesting project.All the required driver modules are located in the hardware directory.Get hands-on and explore — I’m sure you can do it too!
Oh right! I almost forgot — both projects are developed with CLion + STM32CubeMX.
If you want to open this project and compile it successfully, please make sure to configure the CMake toolchain properly, clear the CMake cache, and then restart.
