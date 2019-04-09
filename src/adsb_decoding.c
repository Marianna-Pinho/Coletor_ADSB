#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "adsb_decoding.h"
#include "adsb_auxiliars.h"
#include "adsb_lists.h"
#include "adsb_time.h"
#include "adsb_createLog.h"

/*===========================
Functions used in identication
decoding
=============================*/

/*==============================================
FUNCTION: getCallsign
INPUT: two char vectors
OUTPUT: a char vector, passed by reference
DESCRIPTION: this function receives the 28 bytes
(or 112 bits) of ADS-B data and returns the callsign
of the aircraft that sent the data.
================================================*/
int getCallsign(char *msgi, char *msgf){

	if((getTypecode(msgi)<1)||(getTypecode(msgi)>4)){ //Identification messages are between 1 and 4
		//printf("It's not an Identification Message\n");
		return DECODING_ERROR;
	}

	char cs_table[] = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ#####_###############0123456789######";
	char msgbin[113], char_aux[7];
	int i = 0, pos = 0, j = 0;

	hex2bin(msgi, msgbin);

	strncpy(msgbin,&msgbin[40],48); //here we extract the information bits of interest (56 bits of data - 5 bits of tc - 3 bits of ec)
	msgbin[48]='\0';

	for(j = 0; j * 6 < 48; j++){        //We want to group the bits in sets of six bits
		
        strncpy(char_aux,&msgbin[j*6], 6);
		char_aux[6]='\0';
		
        pos = bin2int(char_aux);    //Each set of six bits is used to index a position in the table

		if(cs_table[pos]=='#'){	    //We must ignore the # character
			continue;
		}

		msgf[i] = cs_table[bin2int(char_aux)]; //We save the extracted character in the final vector 
		i++;
	}
		
    if(msgf[i-1] == '_'){   //We remove the _ character from the final vector
        msgf[i-1] = '\0';
    }else{
        msgf[i] = '\0';
    }

	return DECODING_OK;				
}

/*===========================
Functions used in velocity
decoding
=============================*/

/*==============================================
FUNCTION: getVelocities
INPUT: a char vector, two float variables,
an integer and a char
OUTPUT: the float, integer and char variables,
passed by reference
DESCRIPTION: this function receives the 28 bytes
(or 112 bits) of ADS-B data and calculates the 
horizontal velocity (stored into the "speed" variable),
the vertical velocity (stored into "rateCD"), the
aircraft heading (stored into "head") and the kind
of velocities calculated, GS or AS, stored into "tag".
================================================*/
int getVelocities(char *msgi, float *speed, float *head, int *rateCD, char *tag){

	if(getTypecode(msgi)!= 19){ //Velocity messages have typecode equal to 19
		//printf("It's not a Airbone Velocity Message\n");
		return DECODING_ERROR;
	}

	char msgbin[113], msgAUX[113];
	int subtype = 0, Vew_sign = 0, 
    Vns_sign = 0, Vel_ew = 0, 
	Vel_ns = 0, Vr_sign = 0, Vr = 0;

	hex2bin(msgi, msgbin);

	strncpy(msgAUX,&msgbin[37],3);
	msgAUX[3] = '\0';

	subtype = bin2int(msgAUX);			//It reads the velocity subtype

	if((subtype == 1)||(subtype == 2)){

		Vew_sign = msgbin[45] - 48;			//It reads the East-West sign

		strncpy(msgAUX,&msgbin[46],10);		//It gets the velocity in East-West direction
		msgAUX[10] = '\0';

		Vel_ew = bin2int(msgAUX) - 1;
	
		Vns_sign = msgbin[56] - 48;			//It reads the North-South sign

		strncpy(msgAUX,&msgbin[57],10);		//It gets the velocity in North-South direction
		msgAUX[10] = '\0';

		Vel_ns = bin2int(msgAUX) - 1;

		if(Vew_sign){ //Vew_sign can be 1 or 0
			Vel_ew = -1*Vel_ew;
		}
		if(Vns_sign){ //Vns_sign can be 1 or 0
			Vel_ns = -1*Vel_ns;				
		}

		*speed = sqrt(Vel_ns*Vel_ns + Vel_ew*Vel_ew); //It gets the final horizontal velocity

		*head = atan2(Vel_ew, Vel_ns);
		*head = (*head)*180/PI_MATH;			//It gets the final heading, in degrees

		if(*head < 0){
			*head = *head + 360;
		}

		strcpy(tag,"GS");				//GroundSpeed.
		tag[2] = '\0';

	}else{
		strncpy(msgAUX, &msgbin[46],10);
		msgAUX[10]='\0';

		*head = (bin2int(msgAUX)/1024.0)*360;

		strncpy(msgAUX, &msgbin[57],10);
		msgAUX[10]='\0';		

		*speed = bin2int(msgAUX);

		strcpy(tag,"AS");			//AirSpeed.
		tag[2] = '\0';
	}

	Vr_sign = msgbin[68] - 48;			//It indicates the direction of the vertical velocity (UP/DOWN).

	strncpy(msgAUX,&msgbin[69],9);		//It gets the vertical velocity (rate)
	msgAUX[9] = '\0';

	Vr = bin2int(msgAUX);

	*rateCD = (Vr - 1)*64;				//It gets the final vertical velocity
	
	if(Vr_sign){
		*rateCD = -1*(*rateCD);//-1*Vr;				//DOWN
	}else{
		*rateCD = *rateCD;//Vr;					//UP
	}

	return DECODING_OK;
}

/*===========================
Functions used in position
decoding
=============================*/

/*==============================================
FUNCTION: getPositionMessage
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives the 28 bytes
(or 112 bits) of ADS-B data and returns 0 if the
data isn't about position or returns 1, if the
data is about position.
================================================*/
int isPositionMessage(char *msgi){
	if((getTypecode(msgi)<5)||(getTypecode(msgi)>18)){
		//printf("It's not a Position Message\n");
		return 0;
	}else{
		return 1;
	}
}

/*==============================================
FUNCTION: getPositiontype
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives the 28 bytes
(or 112 bits) of ADS-B data and returns the value
that indicates if the position message is of the
even or odd type.
================================================*/
int getPositionType(char *msgi){
	if(!isPositionMessage(msgi)){ //It verifies if the data is about position
		//printf("It's not a Position Message\n");
		return DECODING_ERROR;
	}

	char msgbin[113];							
	hex2bin(msgi, msgbin);

	return msgbin[53]-48; //It gets the value of the "CPR odd/even frame" flag
}

/*==============================================
FUNCTION: getCPRLatitude
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives the 28 bytes
(or 112 bits) of ADS-B data and converts the value
of the latitude, still in the CPR format, from 
binary to integer.
================================================*/
int getCPRLatitude(char *msgi){
	if(!isPositionMessage(msgi)){
		//printf("It's not a Position Message\n");
		return DECODING_ERROR;
	}
	char msgbin[113];
	hex2bin(msgi,msgbin);

	strncpy(msgbin,&msgbin[54],17); //It gets the latitude information from the data
	msgbin[17] = '\0';

	return bin2int(msgbin);	//It converts the latitude value to an integer
}

/*==============================================
FUNCTION: getCPRLongitude
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives the 28 bytes
(or 112 bits) of ADS-B data and converts the value
of the longitude, still in the CPR format, from 
binary to integer.
================================================*/
int getCPRLongitude(char *msgi){
	if(!isPositionMessage(msgi)){
		//printf("It's not a Position Message\n");
		return DECODING_ERROR;
	}

	char msgbin[113];
	hex2bin(msgi,msgbin);

	strncpy(msgbin,&msgbin[71],17); //It gets the longitude information from the data
	msgbin[17] = '\0';

	return bin2int(msgbin); //It converts the longitude value to an integer
}

/*==============================================
FUNCTION: getCprNL
INPUT: a float
OUTPUT: an integer
DESCRIPTION: this function receives a latitude value
and calculates its latitude zone, based on a lookup
table proposed on the paper "A Formal Analysis of 
the Compact Position Reporting Algorithm". To build
the lookup table, the original formula was applied 
for each one of the 58 possible values.
================================================*/
int getCprNL(float lat){
	float LAT = abs(lat);
	int i = 0;

	for(i = 2; i <= 59;i++){
		if(LAT > transitionLatTable[i-2]){		//transitionLatTable is the lookup table defined in adsb_decoding.h
			return i-1;
		}
	}
		return 59;
}

/*==============================================
FUNCTION: getAirbonePosition
INPUT: two char vectors, two double variables
and two float variables
OUTPUT: an integer and two float variables, passed
by reference
DESCRIPTION: this function receives an Even and 
an Odd 28 bytes ADS-B messages, and their respec-
tives timestamps, and returns the latitude and 
longitude of the aircraft that sent the data,
based on those messages.
================================================*/
int getAirbornePosition(char *msgEVEN, char *msgODD, double timeE, double timeO, float *lat, float *lon){
	float latCPR_even = 0.0, latCPR_odd = 0.0,
	lonCPR_even = 0.0, lonCPR_odd = 0.0;

	float latSizeZone_even = 0.0, latSizeZone_odd = 0.0;
	float latEven = 0.0, latOdd = 0.0;
	float latIndex = 0.0;

	latCPR_even = ((float)getCPRLatitude(msgEVEN))/(131072.0);   //It converts the integer value in a fraction of the total value it can assumes, which is 131072 (2^17).	
	latCPR_odd = ((float)getCPRLatitude(msgODD))/(131072.0);

	lonCPR_even = ((float)getCPRLongitude(msgEVEN))/(131072.0);
	lonCPR_odd = ((float)getCPRLongitude(msgODD))/(131072.0);

	latSizeZone_even = 6.0; 	//It calculates the size of the latitude zone in the North-South direction, which is given by 360.0/60.0
	latSizeZone_odd = 6.101694915254237288; 	//It's the result of 360.0/59.0

	latIndex = floor(59.0*latCPR_even - 60.0*latCPR_odd + 0.5);		//It calculates the latitude index

	latEven = latSizeZone_even*(getMOD(latIndex,60)+latCPR_even);		//It calculates the final even latitude
	latOdd = latSizeZone_odd*(getMOD(latIndex,59)+latCPR_odd);			//It calculates the final odd latitude

	if(latEven >= 270){					//In the South Hemisphere, latitude values can be in the interval [270,360]
		latEven = latEven - 360;		//It makes the values stay in the interval [-90,90]
	}
	if(latOdd >= 270){
		latOdd = latOdd - 360;
	}

	if(getCprNL(latEven) != getCprNL(latOdd)){		//It checks if the odd and even latitudes are in the same latitude zone
		return DECODING_ERROR;								//If they aren't, the longitude can't be calculated and the decoding stops.
	}

	/* To calculate the longitude */

	int ni = 0, m = 0;
	
	if(timeE > timeO){	//It gets the latitude and longitude from the more recent message
		ni = getLarger(1,getCprNL(latEven));
		m = floor(lonCPR_even*(getCprNL(latEven)-1) - lonCPR_odd*getCprNL(latEven) + 0.5);
	
		*lon = (360.0/(double)ni) * (getMOD(m,ni) + lonCPR_even);	//It gets the final longitude
		*lat = latEven;		//It gets the final latitude
	
	}else if(timeO > timeE){
		ni = getLarger(1,getCprNL(latOdd) - 1);
		m = floor(lonCPR_even*(getCprNL(latOdd)-1) - lonCPR_odd*getCprNL(latOdd) + 0.5);

		*lon = (360.0/(double)ni) * (getMOD(m,ni) + lonCPR_odd);	//It gets the final longitude
		*lat = latOdd;	//It gets the final latitude
	}

	if(*lon > 180){	 //If the longitude is larger than 180, we use negative values
		*lon = *lon - 360;
	}

	return DECODING_OK;
}

/*==============================================
FUNCTION: getAltitude
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives the 28 bytes
(or 112 bits) of ADS-B data and returns the altitude
of the aircraft that sent the data, in the unit of
measure "feet".
================================================*/
int getAltitude(char *msgi){ 
	if(!isPositionMessage(msgi)){
		//printf("It's not a Airbone Position Message\n");
		return DECODING_ERROR;
	}

	char msgbin[113], msgAUX[113];

	hex2bin(msgi,msgbin);
	char Qbit = msgbin[47];		//Qbit says whether the altitude is a multiple of 25(1) or 100(0)

	if(Qbit == '1'){
		int N = 0;
	
		strncpy(msgAUX,&msgbin[40],7);	//It gets the rest of the bits, taking off the Qbit
		msgAUX[7]='\0';
		strncat(msgAUX,&msgbin[48],4);
		msgAUX[11]='\0';

		N = bin2int(msgAUX);	//It calculates the coded altitude.

		return N*25 - 1000; 	//It gets the final altitude

	}else{
		//The code for multiples of 100 is not implemented yet
		return DECODING_ERROR;
	}
}

/*==============================================
FUNCTION: setPosition
INPUT: a char vector and an adsbMsg variable passed
by reference
OUTPUT: an adsbMsg pointer
DESCRIPTION: this function receives the 28 bytes
(or 112 bits) of ADS-B position data, verifies the
conditions to save the message and returns an adsbMsg
variable filled with the data received. If already exists
a message,of the same type, saved, it will be replaced.
If the message is of the oposite type, the time interval
between them must be less or equal to 10 seconds, on the
other hand, the message will be removed. In all the cases,
the new message (more recent) will be saved. 
================================================*/
adsbMsg* setPosition(char *msg, adsbMsg *node){
	double ctime = getCurrentTime();	//It gets the time of arrive of the new message
	int typeMsg = getPositionType(msg);
	int sizeMsg[2];

	sizeMsg[0] = strlen(node->oeMSG[0]);
	sizeMsg[1] = strlen(node->oeMSG[1]);

	if(sizeMsg[!typeMsg] != 0){		//It verifies if there is a message of the other type saved
		if((ctime - node->oeTimestamp[!typeMsg]) > 10){	//It verifies if the time interval between the already saved and the new message isn't bigger than 10 seconds
			node->oeMSG[!typeMsg][0] = '\0';
		}
	}

	strcpy((node->oeMSG[typeMsg]), msg);	//It saves the new message
	node->oeMSG[typeMsg][28] = '\0';
	node->oeTimestamp[typeMsg] = ctime;	//It saves the time of arrive of the new message
	node->lastTime = typeMsg;

	return node;
}

/*==============================================
FUNCTION: decodeMessage
INPUT: a char vector, an adsbMsg pointer and an 
adsbMgs pointer to pointer
OUTPUT: an adsbMsg pointer
DESCRIPTION: this function receives the 28 bytes
(or 112 bits) of ADS-B data, a list of information
about the aircraft, extracts the information from 
the data and save it into the list.
================================================*/
adsbMsg* decodeMessage(char* buffer, adsbMsg* messages, adsbMsg** nof){

	char icao[7], tag[3];
	icao[0] = '\0';

	int rateV = 0, alt = 0;
	float lat = 0.0, lon = 0.0,
	heading = 0.0, vel_h = 0.0;
	
	adsbMsg* no = NULL;

	if((getDownlinkFormat(buffer) == 17) && (strlen(buffer) == 28)){ //It verifies if the message received is of ADS-B type
		printf("\n\n***********ADSB MESSAGE*************\n");
		printf("MESSAGE:%s\n", buffer);

//Catching ICAO
		getICAO(buffer, icao);
		
		if(messages == NULL){
			messages = LIST_create(icao);	//If the list of messages is empty, we create the first node
			no = messages;

		}else{
			if((no = LIST_insert(icao, messages)) == NULL){ //It tries to insert a new node
				if((no = LIST_find(icao, messages)) == NULL){
					perror("ICAO not found");	
					return messages;
					
				}					
			}
		}

		printf("\n-----------------------------------------------------------\n| ");
		printf("ICAO:%s\n", no->ICAO); 
		printf("-----------------------------------------------------------\n");

//Catching the Callsign
		if((1 <= getTypecode(buffer)) && (getTypecode(buffer) <= 4)){
			if(getCallsign(buffer, no->callsign) < 0){
				printf("ADS-B Decoding Error: callsign couldn't be decoded!\n");
				LOG_add("decodeMessage", "callsign couldn't be decoded");
				
				return NULL;
			}
			strcpy(no->messageID, buffer);
			no->messageID[strlen(buffer)] = '\0';

			printf("\n-------------------IDENTIFICATION----------------------------------------\n|");	
			printf(" CALLSIGN: %s\n", no->callsign );	printf("\t\n");
			printf("--------------------------------------------------------------------------\n");

		}

//Catching Latitude, Longitude and Altitude
		else if(isPositionMessage(buffer)){

			no = setPosition(buffer, no);
			
			if((strlen(no->oeMSG[0]) != 0) && (strlen(no->oeMSG[1]) != 0)){ //It verifies if there is both messages (even and odd)
					if(getAirbornePosition(no->oeMSG[0], no->oeMSG[1], no->oeTimestamp[0], no->oeTimestamp[1], &lat, &lon) < 0){  //It gets the latitude and longitude
						printf("ADS-B Decoding Error: position couldn't be decoded!\n");
						LOG_add("decodeMessage", "position couldn't be decoded");

						return NULL;
					}

					alt = getAltitude(buffer); //It gets the altitude
					if(alt < 0){
						printf("ADS-B Decoding Error: altitude couldn't be decoded!\n");
						LOG_add("decodeMessage", "altitude couldn't be decoded");

						return NULL;
					}

					no->Longitude = lon;
					no->Latitude = lat;
					no->Altitude = alt;
					
					printf("\n-------------------POSICIONAMENTO--------------------------------------\n|");
					printf("LAT: %f\n|LON: %f\n|ALT: %d\n",no->Latitude,no->Longitude,no->Altitude); 
					printf("------------------------------------------------------------------------\n");

			}		
		}
//Catching the velocity
		else if(getTypecode(buffer) == 19){
		
			if(getVelocities(buffer, &vel_h, &heading, &rateV, tag) < 0){
				printf("ADS-B Decoding Error: velocities couldn't be decoded!\n");
				LOG_add("decodeMessage", "velocities couldn't be decoded");

				return NULL;
			}

			no->horizontalVelocity = vel_h;
			no->verticalVelocity = rateV;
			no->groundTrackHeading = heading;

			strcpy(no->mensagemVEL, buffer);
			no->mensagemVEL[strlen(buffer)] = '\0';

			printf("-------------------------------------VELOCIDADE----------------------------------\n");
			printf("|VELH: %f\n|HEAD: %f\n|RATEV: %d\n|TAG: %s\n",no->horizontalVelocity, no->groundTrackHeading, no->verticalVelocity,tag); 
			printf("-------------------------------------------------------------------------------\n");

		}
	
	}else{
		printf("\n\n###############No ADS-B Message#####################\n");
		printf("|Message: %s\n", buffer);
	}

	memset(buffer, 0x0, 29);
	
	*nof = no;
	return messages;
}

/*==============================================
FUNCTION: isNodeComplete
INPUT: an adsbMsg pointer
OUTPUT: an adsbMsg pointer
DESCRIPTION: this function receives an adsbMsg node
and verifies if it has the minimum information expected
to be saved in the database.
================================================*/
adsbMsg* isNodeComplete(adsbMsg *node){

	if(node == NULL){
		return NULL;
	}

	if((strlen(node->oeMSG[0]) != 0) && (strlen(node->oeMSG[1]) != 0)){ //It verifies if there are the two position messages
		if((node->ICAO[0] != 0) && (node->Altitude != 0)){	// It verifies if there is an ICAO and an Altitude
			
			//node->oeMSG[!(node->lastTime)][0] = '\0'; //It cleans up the oldest message			
			//node->Altitude = 0;
			//node->Latitude = 0;
			//node->Longitude = 0;

			return node;
		}
	}

	return NULL;
}

/*==============================================
FUNCTION: clearMinimalInfo
INPUT: an adsbMsg pointer
OUTPUT: an integer
DESCRIPTION: this function receives an adsbMsg node
and clears the minimal information fields to prevent
the algorithm of entering in isNodeComplete using
old information.
================================================*/
void clearMinimalInfo(adsbMsg *node){
	if(node == NULL){
		return;
	}

	node->oeMSG[!(node->lastTime)][0] = '\0'; //It cleans up the oldest message			
	node->Altitude = 0;
	node->Latitude = 0;
	node->Longitude = 0;
}