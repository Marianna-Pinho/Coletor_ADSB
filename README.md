# RadarLivre C Collector

This algorithm is an ADS-B based message decoder written in C language, which communicates with a [micro ADS-B receptor](http://www.microadsb.com/), decodes the incoming messages and saves them in a local database. This project makes part of the RadarLivre project, developed by students and professors of Federal University of Ceara, campus Quixada.

## Files
This project contains the following files:
- **adsb_auxiliars(.c .h)**: this file has the auxiliary functions that are used for conversion, formatting, calculation and CRC operations.
- **adsb_decoding(.c .h)**: this file has the functions responsible for decode the incoming ADS-B messagens, getting the *ICAO address*, *callsign*, *latitude*, *longitude*, *altitude*, *horizontal velocity*, *vertical velocity* and *heading*.
- **adsb_lists(.c .h)**: this file has the functions responsible for list operations. The list is used to temporarily store the decoded ADS-B information.
- **adsb_serial(.c .h)**: the functions of this file are responsible for configuring and performing the serial communication operations, which are used to communicate with the micro ADS-B receptor.
- **adsb_time(.c .h)**: this file has the functions responsible for time reading and formatting, and for interrupt and timer configuration.
- **adsb_createLog(.c .h)**: this file has the functions responsible for create logs about the system.
- **adsb_db(.c .h)**: this file has the functions responsible for database operations. More specific, for initializing and saving operations.
- **adsb_userInfo.h**: this file has the user information that will be used to communicate with a remote server.
- **adsb_collector.c**: this file has the main function.

### Database
The database used in this version is the [SQLite](https://www.sqlite.org/index.html) 3.28.0. We use two main tables: **radarlivre_api_adsbinfo** and **radarlivre_api_airline**. Their schematic can be saw below:

```sh
CREATE TABLE "radarlivre_api_adsbinfo" 
(
	"id" integer NOT NULL PRIMARY KEY AUTOINCREMENT, 
	"collectorKey" varchar(64) NULL, 
	"modeSCode" varchar(16) NULL, 
	"callsign" varchar(16) NULL, 
	"latitude" decimal NOT NULL, 
	"longitude" decimal NOT NULL, 
	"altitude" decimal NOT NULL, 
	"verticalVelocity" decimal NOT NULL, 
	"horizontalVelocity" decimal NOT NULL, 
	"groundTrackHeading" decimal NOT NULL, 
	"timestamp" bigint NOT NULL, 
	"timestampSent" bigint NOT NULL, 
	"messageDataId" varchar(100) NOT NULL, 
	"messageDataPositionEven" varchar(100) NOT NULL, 
	"messageDataPositionOdd" varchar(100) NOT NULL, 
	"messageDataVelocity" varchar(100) NOT NULL
);
```

```sh
CREATE TABLE "radarlivre_api_airline"
(
	"id" integer NOT NULL PRIMARY KEY AUTOINCREMENT, 
	"name" varchar(255) NULL,
	"alias" varchar(255) NULL,
	"iata" varchar(4) NULL,
	"icao" varchar(8) NULL,
	"callsign" varchar(255) NULL,
	"country" varchar(255) NULL,
	"active" bool NULL	
);
```

## Compiling and Running

To compile the system, we use the Makefile. As compiler, we are using the **gcc** and as a cross-compiler we are using the **arm-linux-gnueabihf-gcc**. To compile, it is just necessary to execute the make command, as below:
```sh
make
```
And to clean the project, we execute:
```sh
make clean
```
After the compiling, a file called **run_collector** will be generated. To run the system, we run this file, as below:
```sh
sudo ./run_collector
```
When running the system, two files will be generated: **radarlivre_v4.db**, which is the database file, and **adsb_log.log**, which is the log file.