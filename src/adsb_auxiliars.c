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
FUNCTION: int2hex
INPUT: an integer
OUTPUT: a char
DESCRIPTION: this function receives an integer number
between 0 and 15 and transforms it in its hexadecimal
digit representation. 
================================================*/
char int2hex(int num){
	if((0 <= num) && (num <= 9)){
		return num + 48;	
	}
	else if((10 <= num) && (num <= 15)){
		return num + 55;
	}
	else{
		return -1;
	}
}


/*==============================================
FUNCTION: bin2hex
INPUT: two char vectors
OUTPUT: void
DESCRIPTION: this function receives a binary number
and transforms it in its hexadecimal representation. 
================================================*/
void bin2hex(char *msgbin, char *msghex){
	int tambin = strlen(msgbin);
	int tamhex = ((tambin % 4 == 0) ? tambin/4 : tambin/4 + 1);
	int cont = 4, soma = 0, mult = 1, i = 0, j = 0;

	j = tamhex - 1;
	for(i = tambin-1; i >= 0; i--){
		soma += (msgbin[i]-48) * mult;
		mult *= 2;
		cont--;

		if((cont == 0) || (i == 0)){
			msghex[j] = int2hex(soma);
			cont = 4;
			soma = 0;
			mult = 1;
			j--;
		}
	}

	msghex[tamhex] = '\0';
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

	}else if(strlen(msg) == 26){
		strncpy(msg, &msg[12], 14);
		msg[14] = '\0';
	}else{
		msg[0] = '\0';
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

/*==============================================
FUNCTION: CRC_verifyMsg
INPUT: a char vector and a pointer to an integer
OUTPUT: an integer
DESCRIPTION: this function receives an adsb message
and returns if the received message is correct or
not, applying CRC (Cyclic Redundancy Check). If this
functions returns 0, the message has parity erros.
Otherwise, the function returns 1.
================================================*/
int CRC_verifyMsg(char *msg, int *syndrome){

	char GENERATOR[] = "1111111111111010000001001";

	char msgbin[strlen(msg)*4+1];
	char *remainder = NULL;
	int pos_msg=0, pos_gen=0, len_msg = 0;
	
	hex2bin(msg,msgbin);
	len_msg = strlen(msgbin);

	//We are only dealing with messages of length 112.
	//So, messages shorter than this length are not verified.
	if(len_msg < LEN_ES_MSG){
		return 0;
	}

	//It performs the arithmetic division in module 2 (Check)
	unsigned int len_gen = strlen(GENERATOR);
	for(pos_msg = 0; pos_msg < len_msg-24; pos_msg++){	
		if(msgbin[pos_msg] == '1'){
			for(pos_gen = 0; pos_gen < len_gen; pos_gen++){
				if((msgbin[pos_msg+pos_gen]-48) ^ (GENERATOR[pos_gen]-48)){
					msgbin[pos_msg+pos_gen]='1';
				}else{
					msgbin[pos_msg+pos_gen]='0';
				}
			}
		}
	}	

	printf("%s\n",msgbin);
	*syndrome = bin2int(msgbin);

	//If the remainder of the division is zero, the message is correct and we return 1.
	//Otherwise, we return 0.
	for(pos_msg = 0; pos_msg < len_msg; pos_msg++){
		if(msgbin[pos_msg] == '1'){
			return 0;
		}
	}

	return 1;
}

/*==============================================
FUNCTION: CRC_correctMsg
INPUT: a char vector and a pointer to an integer
OUTPUT: an integer
DESCRIPTION: this function receives an adsb message
and a syndrome and makes a one single bit CRC correction
in the bit that generates that syndrome. If this
functions returns 1, the message was succesfully
corrected. Otherwise, the function returns 0.
================================================*/
int CRC_correctMsg(char *msghex, int *syndrome){
	char msgbin[4*strlen(msghex) + 1];
	int i = 0;

	hex2bin(msghex, msgbin);
	for(i = 0; i < crcTableSize; i++){
		if(crcSyndromeTable[i] == *syndrome){
			msgbin[crcTableSize - i - 1] ^= 1;
			bin2hex(msgbin, msghex);

			return CRC_verifyMsg(msghex, syndrome);
		}
	}

	return 0;
}

/*==============================================
FUNCTION: CRC_tryMsg
INPUT: a char vector and a pointer to an integer
OUTPUT: an integer
DESCRIPTION: this function receives an adsb message
and a syndrome, verifies if the message contains some
error and, if the message has an error, it tries to
correct. If this functions returns 1, the message was
succesfully corrected. Otherwise, the function returns 0.
It is important remember that this algorithm just correct
one single bit error.
================================================*/
int CRC_tryMsg(char *msg, int *syndrome){
	if(!CRC_verifyMsg(msg, syndrome)){
		printf("CRC detected an error!\n");

		if(CRC_correctMsg(msg, syndrome)){
			printf("Message successfully corrected!\n");
			return 1;
		}

		return 0;
	}

	return 1;
}