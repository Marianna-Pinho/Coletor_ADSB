#include <stdio.h>
#include <string.h>

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
FUNCTION: hex2int
INPUT: a char
OUTPUT: it returns an integer
DESCRIPTION: this function receives a hexadecimal
digit, been it uppercase or lowercase, and returns
its integer value.
================================================*/
int downlinkFormat(char *msgi){ 			//Função que retorna o valor de DownlinkFormat da mensagem ADSB.
	char msg_hex[3], msgbin[9], msgbin_aux[6];
	msg_hex[0] = msgi[0];
	msg_hex[1] = msgi[1];
	msg_hex[2] = '\0';

	hex2bin(msg_hex,msgbin);
	strncpy(msgbin_aux,msgbin,5);
	msgbin_aux[5] = '\0';

	return bin2int(msgbin_aux);
}