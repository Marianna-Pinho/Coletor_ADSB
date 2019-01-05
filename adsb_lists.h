#ifndef ADSB_LISTS_H
#define ADSB_LISTS_H

/*===============================
These functions are responsible
for deal with the lists created
in the system.
=================================*/

/*==================================
STRUCT: adsbMsg
DESCRIPTION:
	char COLLECTOR_ID[40]: receives the collector id.
	char ICAO[7]: receives the unique address of the aircraft (ICAO).
	char callsign[9]: receives the flight id (callsign).
	
	double oeTimestamp[2]: stores the arrive timestamp of the odd and even messages.					
	int lastTime: indicates the last message received (even or odd).
	
	float Latitude: receives the aircraft latitude.
	float Longitude: receives the aircraft longitude.
	int Altitude: receives the aircraft altitude.
	
	float horizontalVelocity: receives the aircraft velocity in a horizontal direction.
	int verticalVelocity: receives the aircraft up or down rate of movement.				

	float groundTrackHeading: receives the angle for which the aircraft nose is pointing. 
	
	char oeMSG[2][29]: stores the even and odd messages.
	char messageID[29]: stores the identification message.
	char mensagemVEL[29]: stores the velocity message.	
===================================*/

typedef struct msg{
	char COLLECTOR_ID[40];
	char ICAO[7];
	char callsign[9];
	
	double oeTimestamp[2];
    int lastTime;
    
    //ADSB position
	float Latitude;
	float Longitude;
	int Altitude;
	
	//Airplane velocity
	float horizontalVelocity;
	int verticalVelocity;

	//Airplane angle
	float groundTrackHeading;
	
	//Original ADSB messages
	char oeMSG[2][29];
	char messageID[29];
	char mensagemVEL[29];

	struct msg *next;
}adsbMsg;

adsbMsg* LIST_create(char *ICAO);
adsbMsg* LIST_insert(char *ICAO, adsbMsg* list);
adsbMsg* LIST_find(char* ICAO, adsbMsg* list);
adsbMsg* LIST_removeOne(char* ICAO, adsbMsg** list);
void	 LIST_removeAll(adsbMsg** list);

#endif