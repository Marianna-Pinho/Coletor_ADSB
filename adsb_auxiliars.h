#ifndef ADSB_AUXILIARS_H
#define ADSB_AUXILIARS_H

/*===============================
These functions are auxiliars of
other functions in the system.
=================================*/

int bin2int(char *msgbin);
int hex2int(char caractere);
void int2bin(int num, char *msgbin);
void hex2bin(char *msgi, char *msgbin);
int downlinkFormat(char *msgi);

#endif
