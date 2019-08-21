#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"
#include "serial_communication.h"
#include "adsb.h"
#include "listFunctions.h"
#include "getTime.h"
#include <unistd.h>			//Retirar depois. Apenas para delay de teste!!!!!!!!!
#include <signal.h>
#include <netinet/in.h>
#include <pthread.h>
#include "network.h"



int getICAO(char *ICAO, char* msgi){
	if(strlen(ICAO) == 0){															//Se a string que guarda icao estiver vazia, a completamos. Devemos continuar a decodificação diretamente.
		icaoADSB(msgi, ICAO);
		return 0;
	}

	char icao[7];
	icaoADSB(msgi, icao);

	if(strcmp(ICAO, icao) == 0){													//Se a string não estiver vazia, mas o icao da nova msg for igual ao dela, fazemos nada.
		return -1;																	//Devemos continuar a decodificação diretamente.
	}

	return 1;																		//1 indica que teremos que criar um novo nó na nossa fila de structs. Criaremos um novo nó para a fila.
																					//Então continuamos a decodificação.
}

#define BUFFERSIZE	2048

adsbMsg *messages = NULL;

typedef struct buffer{
	adsbMsg completeMsg[BUFFERSIZE];
	int pos;	
}Buffer;

Buffer fullMsgBuf;

 void hello(){
 	printf("Ctrl+C\n");
 	removeMsg(messages);
 	exit(1);
 }

void main(){	
	toSend packet;
	signal(2, hello);														//A main será, na verdade, o código que controla todas as funções que, em python, está no arquivo __init__.py do diretorio receptor.
	char BUFF[29];								// 8D00C30A7000415D22A169020481 ID = 8D4840D6202CC371C32CE0576098
	int fd = 0, flagi = 0, oeordem = 0, alt = 0, i=0, oeflag = 0, rateV = 0, serialPort = 0;
	char icao[7], callsign[9], tag[3];
	float lat = 0.0, lon = 0.0, vel_h = 0.0, heading = 0.0;

	pthread_t thread;
	adsbMsg* no = NULL;	


	int du = pthread_create(&thread, NULL, dataUploader, NULL);					//Cria uma thread responsável apenas por mandar um Hello do coletor para o servidor, a cada 1 min.
    if (du){
	 	printf("ERROR; return code from pthread_create() is %d\n", du);
 		exit(-1);
 	}	

 	//serialPort = serialSTART();

	FILE *p = fopen("/home/marianna/Documentos/03-Bolsa_PIBITI/01-Coletor_versoes_PIBITI/07-Coletor_ADSB/src/teste01_Todas.txt", "r");

    if(p == NULL){
         printf("File don't found\n");
     }

	while(fscanf(p," %s", BUFF) != EOF){
		BUFF[strlen(BUFF)] = '\0';

		// if (serialREADING(&serialPort, BUFF) == 10 ){			//Se a mensagem for um caractere branco, que nesse caso é o NL, não executaremos o restante das instruções.
		// 	continue;
		// }
		messages = decodeMessage(BUFF, messages, &no);

		if(isComplete(no) == NULL){
			printf("Message doesn't complete\n");
		}else{
			int pm = pthread_create(&thread, NULL, postMessage, (void *)no);				//Se a mensagem já contiver os dados suficientes, ela será enviada.
		    if (pm){
	 			printf("ERROR; return code from pthread_create() is %d\n", pm);
 				exit(-1);
 			}	
		}

		no = NULL;
		memset(BUFF, 0x0, 29);
		sleep(1);
	}	

}

	

