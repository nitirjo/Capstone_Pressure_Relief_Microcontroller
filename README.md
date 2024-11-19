If you want to load this code onto the ESP32, you'll need to install the files needed for ESP32 board on Arduino IDE.
You can follow the guide in this link to install it: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/

You'll also want the libraries for the LSM6DSO32, which you can find instructions for installing here: https://learn.adafruit.com/lsm6dsox-and-ism330dhc-6-dof-imu/arduino

The tests folders contain some sketchbooks you can upload to test the BLE functionality of the device.
The basic test just constantly updates the values of the characteristics
The onReadWrite test does the following
  angleCharacteristic is constantly updated
  timeCharacteristic is only updated on read and prints to serial if it is read from
  otherCharacteristic is constantly updated and echoes whatever is written to it to serial
