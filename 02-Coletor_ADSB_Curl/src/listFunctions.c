#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "listFunctions.h"
#include "getTime.h"
#include "adsb.h"
#include "userConfigs.h"

adsbMsg *criar(char *ICAO){
	adsbMsg *msg = (adsbMsg*)malloc(sizeof(adsbMsg));
	
	strcpy(msg->ICAO, ICAO); //Copiando ICAO para a estrutura
	msg->ICAO[6] = '\0';
	strcpy(msg->COLLECTOR_ID, collectorId); //Copiando ID do coletor para a estrutura
	msg->COLLECTOR_ID[strlen(collectorId)] = '\0';
	
	msg->callsign[0] = '\0';
	msg->oeMSG[0][0] = '\0';
	msg->oeMSG[1][0] = '\0';
	msg->oeTimestamp[0] = 0;
	msg->oeTimestamp[1] = 0;
	msg->Latitude = 0;
	msg->Longitude = 0;
	msg->Altitude = 0;
	msg->horizontalVelocidade = 0;
	msg->verticalVelocidade = 0;
	msg->groundTrackHeading = 0;
	msg->messageID[0] = '\0';
	msg->mensagemVEL[0] = '\0';

	msg->next = NULL;
	return msg;
}

adsbMsg *inserir(char *ICAO, adsbMsg* list){
	adsbMsg* aux1, *aux2;

	for(aux1 = list; aux1 != NULL; aux1 = aux1->next){	
		if(strcmp(aux1->ICAO, ICAO) == 0){
			perror("ICAO já existe!");
			return NULL; 						//ICAO já existe;
		}
		aux2 = aux1;
	}
	aux2->next = (adsbMsg*)malloc(sizeof(adsbMsg));
	strcpy(aux2->next->ICAO, ICAO);
	aux2->next->ICAO[6] = '\0';
	aux2->next->next = NULL;
	strcpy(aux2->next->COLLECTOR_ID, collectorId); //Copiando ID do coletor para a estrutura
	aux2->next->COLLECTOR_ID[strlen(collectorId)] = '\0';

	aux2->next->callsign[0] = '\0';
	aux2->next->oeMSG[0][0] = '\0';
	aux2->next->oeMSG[1][0] = '\0';
	aux2->next->oeTimestamp[0] = 0;
	aux2->next->oeTimestamp[1] = 0;
	aux2->next->Latitude = 0;
	aux2->next->Longitude = 0;
	aux2->next->Altitude = 0;
	aux2->next->horizontalVelocidade = 0;
	aux2->next->verticalVelocidade = 0;
	aux2->next->groundTrackHeading = 0;
	aux2->next->messageID[0] = '\0';
	aux2->next->mensagemVEL[0] = '\0';

	return aux2->next;					//SUCCESS
}

adsbMsg* removeMsg(adsbMsg* list){
	adsbMsg* aux1 = list, *aux2 = NULL;

	while(aux1 != NULL){
		aux2 = aux1;
		aux1 = aux1->next;
		free(aux2);
	}
	return NULL;					//ICAO não encontrado
}

adsbMsg* removeOneList(char* ICAO, adsbMsg* list){	//It needs be verified
	adsbMsg* aux1 = NULL, *aux2 = NULL;

	for(aux1 = list; aux1 != NULL; aux1 = aux1->next){
		if(strcmp(aux1->ICAO, ICAO) == 0){
			if(aux2 == NULL){		//The ICAO belongs to the first node
				aux2 = aux1;
				aux1 = aux1->next;
				free(aux2);
				return aux1;

			}
			aux2->next = aux1->next;	//The ICAO belongs to some intermadiate node
			free(aux1);
			return aux2;			
		}

		aux2 = aux1;
	}

	return NULL;		//The node was not found
}

adsbMsg* encontrar(char* ICAO, adsbMsg* list){ 			//Essa função permite que capturemos um nó adicionar informações aos seus campos.
	adsbMsg* aux;
	for(aux = list; aux != NULL; aux = aux->next){
		if(strcmp(aux->ICAO, ICAO) == 0){
			return aux;						//nó encontrado
		}
	}
	return NULL;			//nó não encontrado.
}
void imprimir(adsbMsg* list){
	adsbMsg* aux;
	for(aux = list; aux != NULL; aux=aux->next){
		printf("ICAO: %s\n", aux->ICAO);
	}
}

adsbMsg* addPosition(char *msg, adsbMsg *no){
	double ctime = getCurrentTime(); //pega o timestamp
	int msgType = oeflagADSB(msg);	//pega o tipo da mensagem (se é Par ou Ímpar)
	printf("FLAG: %d\n", msgType);
	int tamMsg[2];
	tamMsg[0] = strlen(no->oeMSG[0]), tamMsg[1] = strlen(no->oeMSG[1]);  //Verifica se já existe alguma mensagem salva.


	if(tamMsg[!msgType] != 0){		//Se já houver uma mensagem do outro tipo
		printf("DIF: %lf\n", ctime - no->oeTimestamp[!msgType]);
		if((ctime - no->oeTimestamp[!msgType]) > 10){	//Verificamos se a diferença de tempo entre elas é menor ou igual aos 10 segundos permitidos
			no->oeMSG[!msgType][0] = '\0'; 			//Se não for, a mensagem já salva não encontrará mais ninguém com uma diferença de tempo menor, então a apagamos e salvamos a nova.
		}
	}
	printf("OTHER: %s\n", no->oeMSG[!msgType]);
	strcpy((no->oeMSG[msgType]), msg);			//Independentemente do tipo de msg já salvo, salvamos a nova mensagem, pois é a mais recente.
	no->oeMSG[msgType][28] = '\0';
	no->oeTimestamp[msgType] = ctime;

	return no;

}

