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

	do{
		fd = open(SERIALPORT, O_RDWR | O_NOCTTY);

		if(fd < 0){
			perror(SERIALPORT);
			usleep(500);
		}

	}while(fd < 0);

	printf("MicroADSB_Colector: opened!\n");
	
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
		perror("Old configuration can't be got");
		
		if(close(fd) < 0){ //It closes the opened serial port
           perror("MicroADSB_Colector: serial port can't be closed!");	
		}

		return -1;
	}

	bzero(&newsport, sizeof(newsport));
	
    newsport.c_iflag = IXOFF | IXON | IGNBRK | IGNCR | ICRNL;     // https://www.gnu.org/software/libc/manual/html_node/Input-Modes.html
	newsport.c_oflag = 0;							//No output control configurations are changed
	newsport.c_cflag = CS8 | CREAD | CLOCAL; 		//It enables receiving messages
	newsport.c_lflag = ICANON;						//It enables the canonical mode, which reads each data line available

	cfsetispeed(&newsport, BAUDRATE); 				//It configures the input baudrate
	cfsetospeed(&newsport, BAUDRATE);				//It configures the output baudrate

	tcflush(fd, TCIFLUSH);							//Here we clean all the received, but not read, messages until this moment. It's like clean the system buffer.

	status = tcsetattr(fd, TCSANOW, &newsport);		//It saves the new configuration of the serial port, stored in newsport.
	
    if(status < 0){
		perror("Can't set new serial port configuration");

		if(close(fd) < 0){ //It closes the opened serial port
           perror("MicroADSB_Colector: serial port can't be closed!");	
		}

		return -1;
	}

	if(write(fd,"#43-02\n",7) != 7){				//#43-02 is a string of initialization that says to the device that it can start to send messages
		perror("Can't initialize serial port\n");

		if(close(fd) < 0){ //It closes the opened serial port
           perror("MicroADSB_Colector: serial port can't be closed!");	
		}

		return -1;
	}

	printf("MicroADSB_Colector: configured!\n");
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

	status = read(fd, sBuffer, 43);	 //It reads a message from the serial port. Once the message expected has the format @(48 bits + 112 bits);<CR><NL>, we expect 43 bytes of data.
	
    if(status < 0){
		perror("Can't read serial port");
	}

	sBuffer[status] = '\0';
	printf("M: %s\n", sBuffer);
	
	SERIAL_removeFL(sBuffer); //It takes out the characters @ and ;

	if(sBuffer[0] == '\0'){ //If there is no messages, probably the device is not connected.
		return -1;
	}

	return 0;
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
           perror("MicroADSB_Colector: serial port can't be closed!");	
		}
        else{
			printf("MicroADSB_Colector: serial port closed!\n");	
		    
			/*sleep(1); // Wait 1 second
	        fd = SERIAL_open(); //It tries to open a new serial port for the same device

	        sleep(6); //Wait 6 seconds
		    SERIAL_configure(fd); //It configure the new serial port opened*/
			sleep(1);
			fd = SERIAL_start();

		    return fd;
		}
	}
}

/*==============================================
FUNCTION: SERIAL_communicate
INPUT: an integer and a char pointer
OUTPUT: an integer and a string passed by reference
DESCRIPTION: this function receives a file descriptor
and a char pointer, reads data from the serial port
and saves it into the string referenced by the pointer.
While the device is connected, the function keep
reading. Once the device is disconnected, the function
tries to reconnect.
================================================*/
char SERIAL_communicate(int* fd, char *sBuffer){
	
    int status = 1;

	while(status!=0){
		status = SERIAL_read(*fd, sBuffer);
		
        if(strlen(sBuffer) == 1){ //If no message has been read, we do nothing.
			status = 1;
			continue;
		}

		if(status == -1){ //If status == -1, probably the device is not connected. So, we try reconnect it.	
            *fd = SERIAL_reconnect(*fd);
			printf("MicroADSB_Colector: reading!\n");
		}	

	}

	getFrame(sBuffer);
	return sBuffer[0];
}