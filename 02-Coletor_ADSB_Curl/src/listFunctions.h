#ifndef LISTFUNCTIONS_H
#define LISTFUNCTIONS_H

typedef struct msg{
	char COLLECTOR_ID[40];			//Recebe a autenticação do coletor, obtida a partir da plataforma Django.
	char ICAO[7];					//Recebe o ICAO, que é a identificação única da aeronave.
	char callsign[9];				//Recebe o callsign, que é a identificação do voo.
	
	double oeTimestamp[2];					//flag para a ordem em que os tipos de mensagem chegam, usada como substituta do timestamp, por enquanto.
	int lastTime;							//Indica qual a mensagem mais recente, guiando a escolha do tempo de chegada a ser usado.
	
	//ADSB position
	float Latitude;					//Recebe a Latitude da aeronave.
	float Longitude;				//Recebe a longitude da aeronave.
	int Altitude;					//Recebe a altitude da aeronave.
	
	//Airplane velocity
	float horizontalVelocidade;				//Recebe a velocidade horizontal da aeronave.
	int verticalVelocidade;				//Recebe a velocidade vertical da aeronave, que é dada pela taxa de subida ou descida.
	
	//Airplane angle
	float groundTrackHeading;			//Recebe o ângulo para o qual o nariz do avião aponta.
	
	//Originals ADSB messages
	char oeMSG[2][29];				//Recebe as mensagens Even[0] e Odd[1].
	char messageID[29];					//Recebe a mensagem de identificação.
	char mensagemVEL[29];				//Recebe a mensagem de velocidade.	

	struct msg *next;
}adsbMsg;

enum FLAGS{ERRO = -1, SUCCESS};

adsbMsg* criar(char *ICAO);
adsbMsg* inserir(char *ICAO, adsbMsg* list);
adsbMsg* removeMsg(adsbMsg* list);
adsbMsg* encontrar(char* ICAO, adsbMsg* list);
void imprimir(adsbMsg* list);
adsbMsg* addPosition(char *msg, adsbMsg *no);
adsbMsg* removeOneList(char* ICAO, adsbMsg* list);

#endif // LISTFUNCTIONS_H