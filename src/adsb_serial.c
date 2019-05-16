#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>  //file control
#include <termios.h>    //terminal(command line) communication control
#include <sys/ioctl.h>  // input/output generic operations
#include "adsb_auxiliars.h"
#include "adsb_serial.h"
#include "adsb_createLog.h"

const char *SERIALPORTS[13] = {"/dev/ttyACM0", "/dev/ttyACM1", "/dev/ttyACM2"};

/*==============================================
FUNCTION: SERIAL_open
INPUT: void
OUTPUT: it returns an integer
DESCRIPTION: this functions opens a serial
communication port, which is represented by a file
descriptor. While the port can't be opened, the
function stays in a loop trying to open it. The
function returns the file descriptor, represented
by a integer number.
================================================*/
int SERIAL_open(void){

	int fd = 0;
	int pos = 0;

	do{
		fd = open(SERIALPORTS[pos], O_RDWR | O_NOCTTY);

		if(fd < 0){
			perror(SERIALPORTS[pos]);
			pos++;
			pos = pos % 3;
			usleep(500);
		}

	}while(fd < 0);

	printf("MicroADSB_Colector: opened!\n");
	LOG_add("SERIAL_open", "MicroADSB_Colector was opened succesfully");
	printf("MicroADSB_Colector: configuring...\n");
	
    return fd;
}

/*==============================================
FUNCTION: SERIAL_configure
INPUT: an integer
OUTPUT: void
DESCRIPTION: this function receives a integer that
represents a file descriptor and configures the
communication with the device represented by the
file descriptor received.
================================================*/
int SERIAL_configure(int fd){

	int status = 0, st_read = 0;
	struct termios newsport, oldsport;
	
	status = tcgetattr(fd, &oldsport); //It gets the currently configuration of the serial port and saves it into oldsport
	
    if(status < 0){   //It wasn't possible get the currently configuration, probably because the port wasn't correctly opened
		perror("Old configuration couldn't be got");
		LOG_add("SERIAL_configure", "MicroADSB_Colector old serial port configuration couldn't be got");
		
		if(close(fd) < 0){ //It closes the opened serial port
           perror("MicroADSB_Colector: serial port couldn't be closed!");	
		   LOG_add("SERIAL_configure", "MicroADSB_Colector serial port couldn't be closed");
		}

		return SERIAL_ERROR;
	}

	bzero(&newsport, sizeof(newsport));
	
    newsport.c_iflag = IXOFF | IXON | IGNBRK | IGNCR;     //(ICRNL this flag can be inserted) //https://www.gnu.org/software/libc/manual/html_node/Input-Modes.html
	newsport.c_oflag = 0;							//No output control configurations are changed
	newsport.c_cflag = CS8 | CREAD | CLOCAL; 		//It enables receiving messages
	newsport.c_lflag = ICANON;						//It enables the canonical mode, which reads each data line available

	cfsetispeed(&newsport, BAUDRATE); 				//It configures the input baudrate
	cfsetospeed(&newsport, BAUDRATE);				//It configures the output baudrate

	tcflush(fd, TCIFLUSH);							//Here we clean all the received, but not read, messages until this moment. It's like clean the system buffer.

	status = tcsetattr(fd, TCSANOW, &newsport);		//It saves the new configuration of the serial port, stored in newsport.
	
    if(status < 0){
		perror("It couldn't set new serial port configuration");
		LOG_add("SERIAL_configure", "MicroADSB_Colector new serial port configuration couldn't be set");

		if(close(fd) < 0){ //It closes the opened serial port
           perror("MicroADSB_Colector: serial port couldn't be closed!");
		   LOG_add("SERIAL_configure", "MicroADSB_Colector serial port couldn't be closed");	
		}

		return SERIAL_ERROR;
	}

	if(write(fd,"#43-02\n",7) != 7){				//#43-02 is a string of initialization that says to the device that it can start to send messages
		perror("It couldn't initialize serial port\n");
		LOG_add("SERIAL_configure", "MicroADSB_Colector serial port couldn't be initialized");

		if(close(fd) < 0){ //It closes the opened serial port
           perror("MicroADSB_Colector: serial port couldn't be closed!");
		   LOG_add("SERIAL_configure", "MicroADSB_Colector serial port couldn't be closed");	
		}

		return SERIAL_ERROR;
	}

	printf("MicroADSB_Colector: configured!\n");
	LOG_add("SERIAL_configure", "MicroADSB_Colector configured");

	return SERIAL_OK;
}

/*==============================================
FUNCTION: SERIAL_start
INPUT: void
OUTPUT: an integer
DESCRIPTION: this function starts the serial
communication, opening the port and configuring it.
================================================*/
int SERIAL_start(void){

	int fd = 0;

	do{
		fd = SERIAL_open();
		sleep(6);
	}while(SERIAL_configure(fd) < 0);
	
	return fd;
}

/*==============================================
FUNCTION: SERIAL_removeFL
INPUT: a string passed by reference
OUTPUT: void
DESCRIPTION: this function receives the message
sent throught the serial port and removes the
@ and ; characters, which are the first and last
visible characters of the message.
================================================*/
void SERIAL_removeFL(char *msg){
	unsigned int i = 0;
	
    while(msg[i] != '\0'){
		i++;
	}
	
    if(i > 1){
		msg[i-2] = '\0';				//It takes out ;	

		for(i = 0; msg[i] != '\0'; i++){ //It takes out @
			msg[i] = msg[i+1];
		}	
	}
}

/*==============================================
FUNCTION: SERIAL_isDisconnected
INPUT: an integer
OUTPUT: an integer
DESCRIPTION: this function receives a file descriptor
and reads a message from the serial port. If it reads
zero bytes 3 times, we will consider that the serial
port is disconnected. The number 3 is arbitrary.
================================================*/
int SERIAL_isDisconnected(int fd, char *buffer){
	int tries = 3, count = 3, status = 0;
	
	while(tries > 0){
		if(((status = read(fd, &buffer, 43)) == 0)){
			count--;
			memset(buffer, 0x0, 29);
			usleep(500);
		}
		tries--;
	}
	
	//It verifies if a timer interrupt occured
	if((status == -1) && (errno == EINTR)){
		memset(buffer, 0x0, 29);

		return SERIAL_INTERRUPTED;
	}

	buffer[status] = '\0';
	
	//It verifies if the serial is disconnected even after the three tries
	if((!count) || ((status == 0) && strlen(buffer) == 0)){
		memset(buffer,0x0, 29);

		return SERIAL_DISCONNECTED;
	}
	
	
	return SERIAL_OK;
}

/*==============================================
FUNCTION: SERIAL_read
INPUT: an integer and a char pointer
OUTPUT: an integer and a string passed by reference
DESCRIPTION: this function receives a file descriptor
and a char pointer, reads a message from the serial port,
saves it into the string referenced by the pointer
and returns the status of the operation. If the
function returns 0, it means that data was read
from the serial port. Otherwise, the function
returns -1.
================================================*/
int SERIAL_read(int fd, char *sBuffer){

	int status = 0, serial = 0;

	status = read(fd, sBuffer, 43);	 //It reads a message from the serial port. Once the message expected has the format @(48 bits + 112 bits);<CR><NL>, we expect 44 bytes of data. However <CR> is ignored.

	//It verifies if a timer interrupt occurred
    if((status < 0) && (errno == EINTR)){
		perror("It couldn't read serial port due to timer interrupt");
		LOG_add("SERIAL_read", "It couldn't read serial port due to timer interrupt");
		memset(sBuffer, 0x0, 29);

		return SERIAL_INTERRUPTED;
	}

	sBuffer[status] = '\0';

	if((status == 0) && (sBuffer[0] == '\0')){ //If there is no messages, probably the device is not connected.
		
		if((status = SERIAL_isDisconnected(fd, sBuffer)) == SERIAL_DISCONNECTED){
			
			return SERIAL_DISCONNECTED;
		}	
	}
	
	SERIAL_removeFL(sBuffer); //It takes out the characters @ and ;

	return status;
}

/*==============================================
FUNCTION: SERIAL_reconnect
INPUT: an integer
OUTPUT: an integer
DESCRIPTION: this function receives a file descriptor,
tries to close the connection with it, open a new file
descriptor, configure it and returns it. This function
is used to try recover the communication with the device,
once disconnected while the system is running.
================================================*/
int SERIAL_reconnect(int fd){
	
    while(1){
		
        if(close(fd) < 0){ //It closes the opened serial port
           perror("MicroADSB_Colector: serial port couldn't be closed!");
		   LOG_add("SERIAL_reconnect", "MicroADSB_Colector serial port couldn't be closed");	
		}
        else{
			printf("MicroADSB_Colector: serial port closed!\n");
			LOG_add("SERIAL_reconnect", "MicroADSB_Colector serial port was closed");	
		    
			/*sleep(1); // Wait 1 second
	        fd = SERIAL_open(); //It tries to open a new serial port for the same device

	        sleep(6); //Wait 6 seconds
		    SERIAL_configure(fd); //It configure the new serial port opened*/
			sleep(1);
			fd = SERIAL_start();

			LOG_add("SERIAL_reconnect", "MicroADSB_Colector serial port was reconnected");	
		    return fd;
		}
	}
}

/*==============================================
FUNCTION: SERIAL_communicate
INPUT: an integer and a char pointer
OUTPUT: string passed by reference
DESCRIPTION: this function receives a file descriptor
and a char pointer, reads data from the serial port
and saves it into the string referenced by the pointer.
While the device is connected, the function keep
reading. Once the device is disconnected, the function
tries to reconnect.
================================================*/
char SERIAL_communicate(int* fd, char *sBuffer){
	char buffer_aux[44]; buffer_aux[0] = '\0';

    int status = SERIAL_DISCONNECTED;

	//Using this while, some timer interrupts will be ignored. This happens because, if
	//the serial was disconnected, when it to arrive from the reconnect function, the status
	//will remain equal to SERIAL_DISCONNECTED, leading to a re-reading of the serial.
	//The question is: do we let this while in that way?
	while(status == SERIAL_DISCONNECTED){
		
		status = SERIAL_read(*fd, buffer_aux);
		
        // if(strlen(buffer_aux) == 1){ //If a special character was read, we do nothing.
		// 	status = SERIAL_DISCONNECTED;
		// 	memset(buffer_aux, 0x0, 29);
		// 	continue;
		// }

		if(status == SERIAL_DISCONNECTED){ //If status == 0, probably the device is not connected. So, we try reconnect it.	
           	LOG_add("SERIAL_communicate", "MicroADSB_Colector serial port was disconnected");
		    *fd = SERIAL_reconnect(*fd);
			printf("MicroADSB_Colector: reading!\n");
		}	

	}
	
	getFrame(buffer_aux);
	
	strcpy(sBuffer, buffer_aux);
	sBuffer[strlen(sBuffer)] = '\0';
	
	return sBuffer[0];
}