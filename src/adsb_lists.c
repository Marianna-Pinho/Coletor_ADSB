#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "adsb_lists.h"
#include "adsb_userInfo.h"
#include "adsb_time.h"

/*==============================================
FUNCTION: LIST_create
INPUT: a char vector
OUTPUT: it returns a struct of type adsbMsg
DESCRIPTION: this function creates the first node
of a dynamic list that stores the aircraft information 
received through adsb messages.
================================================*/

adsbMsg* LIST_create(char *ICAO, adsbMsg**LastNode){
	adsbMsg *msg = (adsbMsg*)malloc(sizeof(adsbMsg));

	strcpy(msg->ICAO, ICAO);
	msg->ICAO[6] = '\0';

	strcpy(msg->COLLECTOR_ID, collectorId);
	msg->COLLECTOR_ID[strlen(collectorId)] = '\0';
	
	msg->callsign[0] = '\0';
	msg->oeMSG[0][0] = '\0';
	msg->oeMSG[1][0] = '\0';
	msg->oeTimestamp[0] = 0;
	msg->oeTimestamp[1] = 0;
	msg->lastTime = 0;
	msg->uptadeTime = getCurrentTime();
	msg->Latitude = 0;	//Change 0 for -1. Verifies if nothing depends on it.
	msg->Longitude = 0;
	msg->Altitude = 0;
	msg->horizontalVelocity = 0;
	msg->verticalVelocity = 0;
	msg->groundTrackHeading = 0;
	msg->messageID[0] = '\0';
	msg->mensagemVEL[0] = '\0';

	msg->next = NULL;
	*LastNode = msg;

	return msg;
}

/*==============================================
FUNCTION: LIST_insert
INPUT: a char vector and a list of type adsbMsg
OUTPUT: it updates the list passed by reference
DESCRIPTION: this function adds a new node to the
list that stores aircrafts information. Each node
is identified by the ICAO of the aircraft. This 
means that there is only one node for each aircraft
sharing information and all the new messages received
is used to update the information stored in a node.
================================================*/
adsbMsg *LIST_insert(char *ICAO, adsbMsg* list, adsbMsg**LastNode){
	adsbMsg* aux1, *aux2;

	for(aux1 = list; aux1 != NULL; aux1 = aux1->next){	
		if(strcmp(aux1->ICAO, ICAO) == 0){
			perror("ICAO already exists!");
			return NULL; 						//ICAO already exists;
		}
		aux2 = aux1;
	}
	aux2->next = (adsbMsg*)malloc(sizeof(adsbMsg));
	aux2->next->next = NULL;            //It adds a new node in the end of the list and makes the node to point to NULL;

    strcpy(aux2->next->ICAO, ICAO);
	aux2->next->ICAO[strlen(ICAO)] = '\0';
	
	strcpy(aux2->next->COLLECTOR_ID, collectorId); 
	aux2->next->COLLECTOR_ID[strlen(collectorId)] = '\0';

	aux2->next->callsign[0] = '\0';
	aux2->next->oeMSG[0][0] = '\0';
	aux2->next->oeMSG[1][0] = '\0';
	aux2->next->oeTimestamp[0] = 0;
	aux2->next->oeTimestamp[1] = 0;
	aux2->next->lastTime = 0;
	aux2->next->uptadeTime = getCurrentTime();
	aux2->next->Latitude = 0;
	aux2->next->Longitude = 0;
	aux2->next->Altitude = 0;
	aux2->next->horizontalVelocity = 0;
	aux2->next->verticalVelocity = 0;
	aux2->next->groundTrackHeading = 0;
	aux2->next->messageID[0] = '\0';
	aux2->next->mensagemVEL[0] = '\0';

	//return list;
	*LastNode = aux2->next;
	return aux2->next;					//SUCCESS
}

/*==============================================
FUNCTION: LIST_find
INPUT: a char vector and a list of type adsbMsg
OUTPUT: an adsbMsg element or NULL.
DESCRIPTION: this functions returns the node of
the list that has its ICAO equal to that passed
by the function. Or returns NULL, if no elements
has that ICAO.
================================================*/
adsbMsg* LIST_find(char* ICAO, adsbMsg* list){
	adsbMsg* aux;
	for(aux = list; aux != NULL; aux = aux->next){
		if(strcmp(aux->ICAO, ICAO) == 0){

			return aux;						//it found the node
		}
	}
	return NULL;			//the node wasn't found
}

/*==============================================
FUNCTION: LIST_removeOne
INPUT: a char vector and a list of type adsbMsg
OUTPUT: an adsbMsg list or NULL.
DESCRIPTION: this function searches an element in
a list, identified by the ICAO, and deletes it. If
the element is deleted, the resultant list is returned.
If the element is not found, the function returns NULL.
================================================*/
adsbMsg* LIST_removeOne(char* ICAO, adsbMsg** list){	//It needs be verified
	adsbMsg* aux1 = NULL, *aux2 = NULL;

	for(aux1 = *list; aux1 != NULL; aux1 = aux1->next){
		if(strcmp(aux1->ICAO, ICAO) == 0){
			if(aux2 == NULL){		//The ICAO belongs to the first node
				*list = aux1->next;
			}else{
				aux2->next = aux1->next;	//The ICAO belongs to some intermadiate node
			}

			free(aux1);
			return *list;
		}
		aux2 = aux1;
	}
	
	return NULL;		//The node was not found
}

/*==============================================
FUNCTION: LIST_removeAll
INPUT: a list of type adsbMsg
OUTPUT: void
DESCRIPTION: this function receives a list and 
free all its elements.
================================================*/
void LIST_removeAll(adsbMsg** list){
	adsbMsg* aux1 = *list;

	while(aux1 != NULL){
		*list = aux1->next;
		free(aux1);
		aux1 = *list;
	}
}

/*==============================================
FUNCTION: LIST_orderByUpdate
INPUT: a list of type adsbMsg and two adsbMsg
pointers.
OUTPUT: an adsbMsg pointer.
DESCRIPTION: this function receives a list of messages,
a node that was updated and the pointer to the last
element of the list, and reorder the list, putting
the node updated at its end. At the end, the function
returns a pointer to the last element of the list.
Or it returns NULL, if it wasn't possible to sort
the list.
================================================*/
adsbMsg* LIST_orderByUpdate(char *ICAO, adsbMsg *lastNode, adsbMsg **list){
	adsbMsg *aux1 = NULL, *aux2 = NULL;

	if(lastNode == NULL){
		return lastNode;
	}

	//The updated node is already the last node
	if(strcmp(ICAO, lastNode->ICAO) == 0){
		return lastNode;
	}

	for(aux1 = *list; aux1 != NULL; aux1 = aux1->next){
		if(strcmp(ICAO, aux1->ICAO) == 0){
			if(aux2 == NULL){		//The ICAO belongs to the first node
				*list = aux1->next;
			}else{
				aux2->next = aux1->next;	//The ICAO belongs to some intermadiate node
			}

			lastNode->next = aux1; //The previous lastNode now points to the updated node
			lastNode = aux1;		//The lastNode is now the updated node
			lastNode->next = NULL;

			return lastNode;
		}

		aux2 = aux1;
	}

	return NULL;
}

/*==============================================
FUNCTION: LIST_delOldNodes
INPUT: a list of type adsbMsg
OUTPUT: an adsbMsg pointer.
DESCRIPTION: this function receives a list of messages,
removes all the nodes from the list that were updated
there is more time than a limit time interval previously
determined, and returns the updated list of messages.
================================================*/
adsbMsg * LIST_delOldNodes(adsbMsg *messages){
	if(messages == NULL){
		return messages;
	}

	long int current_time = getCurrentTime();
	adsbMsg *aux = messages;

	while((aux != NULL) && (current_time - (aux->uptadeTime) > LIMIT_DIFF_TIME)){
		messages = messages->next;
		free(aux);
		aux = messages;
	}

	return messages;
}