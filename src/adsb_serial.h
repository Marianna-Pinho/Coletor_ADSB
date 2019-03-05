#ifndef ADSB_SERIAL_H
#define ADSB_SERIAL_H

/*===============================
These functions are responsible
for the serial communication
operations.
=================================*/

//#define SERIALPORT "/dev/ttyACM0"
#define BAUDRATE	B115200

int SERIAL_open(void);
int SERIAL_configure(int fd);
int SERIAL_start(void);
void SERIAL_removeFL(char *msg);
int SERIAL_read(int fd, char *sBuffer);
int SERIAL_reconnect(int fd);
char SERIAL_communicate(int* fd, char *sBuffer);

#endif