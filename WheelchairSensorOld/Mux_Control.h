//Enables a specific port number
void enableMuxPort(byte portNumber);

//Disables a specific port number
void disableMuxPort(byte portNumber);

// Initialize MUX. Disabling all ports
void initMUX();

// Disables all other ports. Enables the input port
void selectMuxPort(byte portNumber);
