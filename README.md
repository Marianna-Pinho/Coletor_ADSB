# Coletor_ADSB v 1.0
This repository has the C code to perform the decoding of ADS-B messages.

#Serial Communication

## The microADSB receptor
Our system (Collector) uses an auxiliary [microADSB receptor](http://www.microadsb.com/), responsible for receiving the eletromagnetic waves from aircraft communication and for converting it into 
ASCII characters. These characters are sent to our system through a serial port.

Searching for some place that could helps us to understand the messages format sent by the
microADSB receptor, we found the [RxControl](http://rxcontrol.free.fr/PicADSB/index.html) site. In RxControl, was said that the receptor can send a message in the format **@ + 48 bitsTAG + 112 bitsFRAME + ; + \<CR\>\<LF\>**. As we know, 1 Byte is equal to 8 bits. So, we could want to translate (48 + 112 = 160) bits into (160/8 = 20) Bytes. However, instead of sends that bits to the serial port, the receptor first convertes those bits into hexadecimal characters and after translate them to ASCII characters. Only after those, the receptor sends the message.

Hexadecimal digits can be built with 4 bits. So, the receptor creates a message with (160/4 = 40) digits. Once ASCII characters have 8 bits, the receptor will send to out system:
**@ + 40 hexadecimal bits + ; + \<CR\>\<LF\>** = **8 + 40*8 + 8 + 8 + 8** = **1 Byte + 40 Bytes + 1 Byte + 1 Byte + 1 Byte** = **44 Bytes**.

## The Collector
Although the microADSB receptor is sending 44 bytes, the Collector was configured to ignore the <CR> character. So, the system is reading just 43 bytes.
