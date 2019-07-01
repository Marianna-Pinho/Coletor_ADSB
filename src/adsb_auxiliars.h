#ifndef ADSB_AUXILIARS_H
#define ADSB_AUXILIARS_H

/*===============================
These functions are auxiliars of
other functions in the system.
=================================*/

#define LEN_ES_MSG  112
#define crcTableSize    112

int bin2int(char *msgbin);
int hex2int(char caractere);
void int2bin(int num, char *msgbin);
void hex2bin(char *msgi, char *msgbin);
int getDownlinkFormat(char *msgi);
void getFrame(char *msg);
void getICAO(char *msgi, char *msgf);
void getData(char *msgi, char *msgf);
int getTypecode(char *msgi);
float getMOD(float x, float y);
int getLarger(int a, int b);
int CRC_verifyMsg(char *msg, int *syndrome);
int CRC_correctMsg(char *msghex, int *syndrome);
int CRC_tryMsg(char *msg, int *syndrome);

static const  int crcSyndromeTable[crcTableSize] = 
{
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024,2048, 4096, 8192,
    16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152,
    4194304, 8388608, 16774153, 7195, 14390, 28780, 57560, 115120,
    230240, 460480, 920960, 1841920, 3683840, 7367680, 14735360, 4080649,
    8161298, 16322596, 906305,1812610, 3625220, 7250440, 14500880, 4553769,
    9107538, 15336621, 2882899, 5765798, 11531596, 10493585, 12568875, 8414815,
    16726199, 99687, 199374, 398748, 797496, 1594992, 3189984, 6379968, 12759936,
    8033033, 16066066, 1423405, 2846810, 5693620, 11387240, 10778329, 11999675,
    9553791, 14449399, 4653543, 9307086, 14939029, 3674915, 7349830, 14699660,
    4156689, 8313378, 16626756, 302209, 604418, 1208836, 2417672, 4835344, 9670688,
    14210121, 5135515, 10271030, 13010533, 7534787, 15069574, 3413765, 6827530,
    13655060, 6245409, 12490818, 8569997, 16416019, 724527, 1449054, 2898108,
    5796216, 11592432, 10367465, 12818395, 7920575, 15841150, 1874677, 3749354
};

#endif
