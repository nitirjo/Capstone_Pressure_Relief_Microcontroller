/*
  Basic control and commands for the PCA9548A/TCA9548A I2C multiplexer
  Edited from code provided by SparkFun Electronics
*/

#define MUX_ADDR 0x70 //7-bit unshifted default I2C Address

//Enables a specific port number
//Written by SparkFun
void enableMuxPort(byte portNumber) {
  if (portNumber > 7) portNumber = 7;

  //Read the current mux settings
  Wire.requestFrom(MUX_ADDR, 1);
  if (!Wire.available()) return; //Error
  byte settings = Wire.read();

  //Set the wanted bit to enable the port
  settings |= (1 << portNumber);

  //Write the updated settings
  Wire.beginTransmission(MUX_ADDR);
  Wire.write(settings);
  Wire.endTransmission();
}

//Disables a specific port number
//Written by SparkFun
void disableMuxPort(byte portNumber) {
  if (portNumber > 7) portNumber = 7;

  //Read the current mux settings
  Wire.requestFrom(MUX_ADDR, 1);
  if (!Wire.available()) return; //Error
  byte settings = Wire.read();

  //Clear the wanted bit to disable the port
  settings &= ~(1 << portNumber);

  //Write the updated settings
  Wire.beginTransmission(MUX_ADDR);
  Wire.write(settings);
  Wire.endTransmission();
}

// Initialize MUX. Disabling all ports
void initMUX() {
  Wire.begin();
  for (byte i = 0; i <= 7; i++) {
    disableMuxPort(i);
  }
}

// Disables all other ports. Enables the input port
void selectMuxPort(byte portNumber) {
  for (byte i = 0; i <= 7; i++) {
    disableMuxPort(i);
  }
  enableMuxPort(portNumber);
}
