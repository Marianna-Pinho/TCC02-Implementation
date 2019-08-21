#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#define SERIALPORT "/dev/ttyACM0"
#define BAUDRATE	B115200


void formatMESSAGE(char *msg);
void getFRAME(char *msg);
int openSPORT(void);
void configSPORT(int fd);
int readSPORT(int fd, char *BUFF);
int disconnectedSPORT(int fd, int status);
int serialSTART();
char serialREADING(int* fd, char *BUFF);

#endif // SERIALCOMMUNICATION_H