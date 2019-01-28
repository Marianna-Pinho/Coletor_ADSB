#include <stdio.h>
#include <string.h>
#include <math.h>
#include "adsb_auxiliars.h"

/*==============================================
FUNCTION: bin2int
INPUT: a char vector
OUTPUT: it returns an integer
DESCRIPTION: this function receives a binary number
and transforms it in its integer representation.
The range of values is limited to integer size,
being between -2^32 to 2^32. 
================================================*/
int bin2int(char *msgbin){
	int tam = strlen(msgbin);
	int soma = 0, mult = 1, pos = 0;

	for(pos=tam-1;pos>=0;pos--){
		soma+=(msgbin[pos] - 48)*mult;
		mult*=2;
	}
	return soma;
}

/*==============================================
FUNCTION: hex2int
INPUT: a char
OUTPUT: it returns an integer
DESCRIPTION: this function receives a hexadecimal
digit, been it uppercase or lowercase, and returns
its integer value.
================================================*/
int hex2int(char caractere){
	if((48<=caractere)&&(caractere<=57)){   //interval {0,9}
		return(int)caractere - 48;
	}else if((65<=caractere)&&(caractere<=70)){ //interval {A,F}
		return (int)caractere - 55;
	}else if((97<=caractere)&&(caractere<=102)){ //interval {a,f}
        return (int)caractere - 87;
    }
}

/*==============================================
FUNCTION: int2bin
INPUT: an integer and a char pointer
OUTPUT: a char vector, passed by reference
DESCRIPTION: this function receives a integer
number (num), that represents a hexadecimal digit,
and transforms it in its binary representation,
saving the result in a char vector 
(msgbin).

e.g: transform 15 in a binary number
15 / 2
1   7 / 2
    1   3 / 2
        1   1 / 2
            1   0

Result: 01111  

obs: A hexadecimal number can be represented by a 
four digit binary number. Therefore, we can write
Result: 1111
================================================*/
void int2bin(int num, char *msgbin){			
	int aux = num, pos = 3;
	while(pos>=0){
		msgbin[pos] = (aux%2)+48;
		aux = aux/2;
		pos--;
	}
}

/*==============================================
FUNCTION: hex2bin
INPUT: two char vectors
OUTPUT: a char vector, passed by reference
DESCRIPTION: this function receives a hexadecimal
number, transforms each character of this number
in an integer and, after, converts each integer
in a binary number. In the end, the hexadecimal
number will have been converted in a binary number.

obs: a hexadecimal number can be converted in a 
binary number transforming each one of its digits
in a four digit binary number.

e.g: transform A5 in a binary number
A -> 1010
5 -> 0101

Result: 10100101
================================================*/
void hex2bin(char *msgi, char *msgbin){			
	int i = 0;

	for(i=0;msgi[i]!='\0';i++){
		int2bin(hex2int(msgi[i]), &msgbin[i*4]);
	}
		msgbin[i*4] = '\0';
}

/*==============================================
FUNCTION: getDownlinkFormat
INPUT: a char vector
OUTPUT: it returns an integer
DESCRIPTION: this function receives a vector of
chars containing hexadecimal digits and calculates
the downlinkFormat of the message represented by
the sequence.
================================================*/
int getDownlinkFormat(char *msgi){
	char msg_hex[3], msgbin[9], msgbin_aux[6];
	msg_hex[0] = msgi[0];
	msg_hex[1] = msgi[1];
	msg_hex[2] = '\0';

	hex2bin(msg_hex,msgbin);
	strncpy(msgbin_aux,msgbin,5);
	msgbin_aux[5] = '\0';

	return bin2int(msgbin_aux);
}

/*==============================================
FUNCTION: getFrame
INPUT: a char vectors
OUTPUT: a char vector, passed by reference
DESCRIPTION: this function receives all the hexadecimal
digits received from the receiver device and returns 
only the 112 bits (or 28 bytes) that represent
the ADS-B message. Or returns the 14 bytes that
represents other kind of message.
================================================*/
void getFrame(char *msg){

	if((strlen(msg) > 26) && (strlen(msg) < 41)){
		strncpy(msg, &msg[12], 28);
		msg[28] = '\0';

	}else{
		strncpy(msg, &msg[12], 14);
		msg[14] = '\0';

	}
}

/*==============================================
FUNCTION: getICAO
INPUT: two char vectors
OUTPUT: a char vector, passed by reference
DESCRIPTION: this function receives the 28 bytes
of ADS-B data and returns the ICAO of the aircraft
that sent that data.
================================================*/
void getICAO(char *msgi, char *msgf){
	strncpy(msgf,&msgi[2],6);
	msgf[6]='\0';
}

/*==============================================
FUNCTION: getData
INPUT: two char vectors
OUTPUT: a char vector, passed by reference
DESCRIPTION: this function receives the 28 bytes
of ADS-B data and returns the 56 bits (or 14 bytes)
of information, which can contain, for example, the
velocity, latitude, etc. of the aircraft that sent
the data.
================================================*/
void getData(char *msgi, char *msgf){
	strncpy(msgf,&msgi[8],14);
	msgf[14]='\0';
}

/*==============================================
FUNCTION: getTypecode
INPUT: a char vector
OUTPUT: a integer
DESCRIPTION: this function receives the 28 bytes
(or 112 bits) of ADS-B data and returns its type-
code, which indicates the kind of information
the data contains. 
================================================*/
int getTypecode(char *msgi){
	char msgbin[113];

	hex2bin(msgi, msgbin);
	strncpy(msgbin,&msgbin[32],5);
	msgbin[5]='\0';

	return bin2int(msgbin);
}

/*==============================================
FUNCTION: getMOD
INPUT: two float arguments
OUTPUT: a float
DESCRIPTION: this function receives two float
values and returns the rest of the division
between them (x MOD y). The rest is calculated
based on the Euclidean division, where the rest
is a non-negative value.
================================================*/
float getMOD(float x, float y){
	return x - y*(floor(x/y));
}

/*==============================================
FUNCTION: getLarger
INPUT: two integer arguments
OUTPUT: an integer
DESCRIPTION: this function receives two integer
values and returns the larger between them.
================================================*/
int getLarger(int a, int b){
	if(a > b){
		return a;
	}else{
		return b;
	}
}