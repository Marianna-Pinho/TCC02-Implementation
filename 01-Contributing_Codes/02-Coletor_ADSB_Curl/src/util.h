#ifndef UTIL_H
#define UTIL_H
//Funções usadas como utilitárias na decodificação de mensagens ADSB.

void crc(char *msg_hex, int encode, char *reminder);
void frameADSB(char *msgi, char *msgf);
void int2bin(int num, char *msgbin);
void hex2bin(char *msgi, char *msgbin);
int hex2int(char caractere);
int bin2int(char *msgbin);
int downlinkFormat(char *msgi);
//int floor(float num);

#endif // UTIL_H