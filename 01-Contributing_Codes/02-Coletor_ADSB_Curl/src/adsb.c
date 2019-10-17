#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"
#include "adsb.h"
#include "listFunctions.h"




//Teoricamente, está funcionando. O erro estava em (360/(double)ni), que não tinha o double.

int df_ADSB(char *msg_hex){					//Essa função recebe os 28 bytes de mensagem ADSB e retorna um inteiro que informa seu downlinkFormat.
	return downlinkFormat(msg_hex);
}

void icaoADSB(char *msgi, char *msgf){		//Essa função recebe a mensagem ADSB de 28 bytes e retorna uma string que contém os 6 bytes do ICAO da aeronave.
	strncpy(msgf,&msgi[2],6);				//O ICAO começa no byte 2, quando a contagem começa em 0.
	msgf[6]='\0';
}

void dataADSB(char *msgi, char *msgf){		//Essa função recebe a mensagem ADSB de 28 bytes e retorna os 14 bytes(ou 56 bits) de informação, que pode ser posição, velocidade, etc.
	strncpy(msgf,&msgi[8],14);				//Os dados começam no byte 8, quando a contagem começa em 0.
	msgf[14]='\0';
}

int typecodeADSB(char *msgi){				//Essa função recebe uma string com os 28 bytes da mensagem ADSB e retorna um inteiro contendo seu Type Code
	char msgbin[113];

	hex2bin(msgi, msgbin);					//Primeiro, convertemos a mensagem de hexadecimal para binário.
	
	strncpy(msgbin,&msgbin[32],5);			//Depois, pegamos os campos de interesse. O Type code está nos bits [32:36], quando a contagem começa de 0, da mensagem ADSB.
	msgbin[5]='\0';

	return bin2int(msgbin);					//Convertemos a string obtida para um inteiro e o retornamos.
}

////////////////////////////////////AIRCRAFT IDENTIFICATION////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int capabilityADSB(char *msgi){				//Essa função retorna a informação de Capability da aeronave, que está localizada nos 3 próximos bits depois do DownlinkFormat.
	char msgbin[113];
	hex2bin(msgi, msgbin);

	strncpy(msgbin,&msgbin[5],3);
	msgbin[3] = '\0';

	return bin2int(msgbin);
}

int emitterCategoryADSB(char *msgi){		//Essa função retorna a Categoria da aeronave, que tem haver com tamanho e peso. Ela está localizada nos bits [37:39] da mensagem ADSB.

	if((typecodeADSB(msgi)<1)||(typecodeADSB(msgi)>4)){			//Verificamos se a mensagem é do tipo Identificação. Se ela não for desse tipo, não podemos interpretar seus bits dessa forma.
		printf("It's not a Identification Message\n");			//Se a mensagem não for do tipo identificação, avisamos isso e retornamos o inteiro -1, para que possa ser usado como um sinal de controle pelo função chamadora.
		return -1;
	}

	char msgbin[112];											//Se a mensagem for de identificação, a convertemos para binário para podermos capturar os bits de interesse.
	hex2bin(msgi,msgbin);

	strncpy(msgbin,&msgbin[37],3);								//Aqui pegamos os bits de interesse.
	msgbin[3]='\0';

	return bin2int(msgbin);										//E retornamos seu valor, em inteiro, para a função chamadora.
}

int callsignADSB(char *msgi, char *msgf){

	if((typecodeADSB(msgi)<1)||(typecodeADSB(msgi)>4)){			//Verificamos se a mensagem é do tipo Identificação. Se ela não for desse tipo, não podemos interpretar seus bits dessa forma.
		printf("It's not a Identification Message\n");			//Se a mensagem não for do tipo identificação, avisamos isso e retornamos o inteiro -1, para que possa ser usado como um sinal de controle pelo função chamadora.
		return -1;
	}

	char cs_TABLE[] = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ#####_###############0123456789######";	//Essa string é usada como uma tabela. Agrupamos os caracteres da mensagem binária de seis em seis, e transformamos esses seis caracteres em um inteiro que indexará uma posição dessa tabela.
	char msgbin[113], char_aux[7];
	int i = 0, table = 0, j = 0;

	hex2bin(msgi, msgbin);									//Aqui transformamos a mensagem de hexadecimal para binário.

	strncpy(msgbin,&msgbin[40],48);							//Aqui pegamos os bits de dados que nos interessam, que são os 56 bits, menos os 8 bits reservados para TC e EC.
	msgbin[48]='\0';

	for(j=0;j*6<48;j++){
		strncpy(char_aux,&msgbin[j*6], 6);					//Aqui copiamos os grupos de seis caracteres para um char auxiliar.
		char_aux[6]='\0';
		table = bin2int(char_aux);							//Aqui transformamos o valor representado por esses caracteres (binários) no inteiro que será usado como índice.

		if(cs_TABLE[table]=='#'){							//O caractere # deve ser ignorado, pois ele não aparece no Callsign. Dessa forma, semre
			continue;
		}
		msgf[i] = cs_TABLE[bin2int(char_aux)]; 
		i++;
	}
		if(msgf[i-1] == '_'){
			msgf[i-1] = '\0';
		}else{
			msgf[i] = '\0';
		}
		
				
}

//////////////////////////AIRCRAFT POSITION///////////////////////////////////////////////////////////////////////////////////
int isPositionMessage(char *msgi){
	if((typecodeADSB(msgi)<5)||(typecodeADSB(msgi)>18)){			//Verificamos se a mensagem é do tipo Posição. Se ela não for desse tipo, não podemos interpretar seus bits dessa forma.
		printf("It's not a Position Message\n");			//Se a mensagem não for do tipo Posição, avisamos isso e retornamos o inteiro -1, para que possa ser usado como um sinal de controle pelo função chamadora.
		return 0;
	}else{
		return 1;
	}
}

int oeflagADSB(char *msgi){			//Essa função retorna o bit que indica se a mensagem está no formato ímpar ou par.
	if((typecodeADSB(msgi)<5)||(typecodeADSB(msgi)>18)){			//Verificamos se a mensagem é do tipo Posição. Se ela não for desse tipo, não podemos interpretar seus bits dessa forma.
		printf("It's not a Position Message\n");			//Se a mensagem não for do tipo Posição, avisamos isso e retornamos o inteiro -1, para que possa ser usado como um sinal de controle pelo função chamadora.
		return -1;
	}

	char msgbin[113];							
	hex2bin(msgi, msgbin);						//Aqui passamos a mensagem de hexadecimal para binário.

	return msgbin[53]-48;						//Aqui retornamos o valor, inteiro, do bit na posição 54 da mensagem, que é o responsável por indicar o formato da mensagem.
}

int cprlatitudeADSB(char *msgi){						//Essa função recebe a mensagem em hexadecimal e retorna um inteiro contendo o valor da latitude codificada em cpr.
	if((typecodeADSB(msgi)<5)||(typecodeADSB(msgi)>18)){			//Verificamos se a mensagem é do tipo Posição. Se ela não for desse tipo, não podemos interpretar seus bits dessa forma.
		printf("It's not a Position Message\n");			//Se a mensagem não for do tipo Posição, avisamos isso e retornamos o inteiro -1, para que possa ser usado como um sinal de controle pelo função chamadora.
		return -1;
	}

	char msgbin[113];
	hex2bin(msgi,msgbin);									//Aqui transformamos a mensagem de hexadecimal para binário.

	strncpy(msgbin,&msgbin[54],17);							//Aqui pegamos os 17 bits da mensagem que representam a latitude em CPR.
	msgbin[17] = '\0';

	return bin2int(msgbin);									//Aqui retornamos o valor inteiro da latitude obtida.
}

int cprlongitudeADSB(char *msgi){						//Essa função recebe a mensagem em hexadecimal e retorna um inteiro contendo o valor da longitude codificada em cpr.
	if((typecodeADSB(msgi)<5)||(typecodeADSB(msgi)>18)){			//Verificamos se a mensagem é do tipo Posição. Se ela não for desse tipo, não podemos interpretar seus bits dessa forma.
		printf("It's not a Position Message\n");			//Se a mensagem não for do tipo Posição, avisamos isso e retornamos o inteiro -1, para que possa ser usado como um sinal de controle pelo função chamadora.
		return -1;
	}

	char msgbin[113];
	hex2bin(msgi,msgbin);									//Aqui transformamos a mensagem de hexadecimal para binário.

	strncpy(msgbin,&msgbin[71],17);							//Aqui pegamos os 17 bits da mensagem que representam a longitude em CPR.
	msgbin[17] = '\0';

	return bin2int(msgbin);									//Aqui retornamos o valor inteiro da longitude obtida.
}

float MOD(float x, float y){			//Função que retorna o resto de x/y, ou xMODy.
	return x - y*(floor(x/y));
}

/*int _cprNL(float lat){	//Função que denota o número de zonas de longitute para um determinado valor de latitude.
	if(lat == 0){
		return 59;
	}else if((lat == 87)||(lat == -87)){
		return 2;
	}else if((lat > 87)||(lat < -87)){
		return 1;
	}

	int NZ = 15;
	float a = 1 - cos(M_PI/(2*NZ));
	float b = cos((M_PI/180)*abs(lat))*cos((M_PI/180)*abs(lat));

	float nl = 2*M_PI/(acos(1 - a/b));
	return floor(nl);

}*/

int _cprNL(float lat){	//Lookup table based on article "A Formal Analysis of the Compact Position Reporting Algorithm"
	float LAT = abs(lat);
	int i = 0;

	for(i = 2; i <= 59;i++){
		if(LAT > transition[i-2]){		//transition é uma lookup table declarada em adsb.h
			return i-1;
		}
	}
		return 59;
}
int max(int A, int B){		//Retorna o maior entre dois números
	if(A > B){
		return A;
	}else{
		return B;
	}
}

int airbone_positionADSB(char *msgEVEN, char *msgODD, double timeE, double timeO, float *lat, float *lon){
	char msgbinE[113], msgbinO[113]; msgbinE[0] = '\0'; msgbinO[0] = '\0';
	char msgAUX[18]; msgAUX[0] = '\0';

	float LATcpr_even = 0.0, LATcpr_odd = 0.0, LONcpr_even = 0.0, LONcpr_odd = 0.0;
	float LAT_sizeZoneE = 0.0, LAT_sizeZoneO = 0.0;
	float LATeven = 0.0, LATodd = 0.0;
	float j = 0.0;


	hex2bin(msgEVEN,msgbinE);	//transforma a mensagem em binário
	hex2bin(msgODD, msgbinO);

	strncpy(msgAUX, &msgbinE[54],17);			//Transforma as informações de sua representação binário para um valor inteiro
	msgAUX[17]='\0';
	LATcpr_even = ((float)bin2int(msgAUX))/(131072.0);   //Então observa quanto esse valor inteiro representa do valor total que ele pode assumir, que é 131072 (2^17).
	bzero(&msgAUX,18);
	
	strncpy(msgAUX, &msgbinO[54],17);
	msgAUX[17]='\0';
	LATcpr_odd = ((float)bin2int(msgAUX))/(131072.0);
	bzero(&msgAUX,18);
	
	strncpy(msgAUX, &msgbinE[71],17);
	msgAUX[17]='\0';
	LONcpr_even = ((float)bin2int(msgAUX))/(131072.0);
	bzero(&msgAUX,18);

	strncpy(msgAUX, &msgbinO[71],17);
	msgAUX[17]='\0';
	LONcpr_odd = ((float)bin2int(msgAUX))/(131072.0);
	bzero(&msgAUX,18);

	LAT_sizeZoneE = 360.0/60.0;	//calcula o tamanho da zona de latitude, na direção Norte-Sul.
	LAT_sizeZoneO = 360.0/59.0;
	LAT_sizeZoneO = 6.101694915254237288;

	j = floor(59.0*LATcpr_even - 60.0*LATcpr_odd + 0.5);		//Calcula o índice de latitude j.
	printf("J: %f\n", j);

//*****************Formulas antigas*******************************//
	//LATeven = (LAT_sizeZoneE*MOD(j,60))+LATcpr_even;		//Calcula a Latitude par
	//LATodd = (LAT_sizeZoneO*MOD(j,59))+LATcpr_odd;			//Calcula a Latitude ímpar
//***********************Formulas novas **************************//
	LATeven = LAT_sizeZoneE*(MOD(j,60)+LATcpr_even);		//Calcula a Latitude par
	 LATodd = LAT_sizeZoneO*(MOD(j,59)+LATcpr_odd);


	if(LATeven >= 270){					//No hemisfério sul, os valores de latitude podem ficar no intervalo [270,360]
		LATeven = LATeven - 360;		//Isso garante que eles fiquem no intervalo [-90,90]
	}
	if(LATodd >= 270){
		LATodd = LATodd - 360;
	}

	if(_cprNL(LATeven)!=_cprNL(LATodd)){		//Verifica se as mensagens correspondem à mesma zona de latitude.
		return -1;								//Se forem diferentes, não podemos calcular a longitude. Se forem iguais, devemos calcular a longitude.
	}

	int ni = 0, m = 0;
	
	m = floor(LONcpr_even*(_cprNL(LATeven)-1) - LONcpr_odd*_cprNL(LATodd) + 0.5);

	if(timeE > timeO){							//Quando a mensagem par chegar primeiro, calcularemos sua longitude e latitude
		ni = max(1,_cprNL(LATeven));
		//m = floor(LONcpr_even*(_cprNL(LATeven)-1) - LONcpr_odd*_cprNL(LATeven) + 0.5);
		*lon = (360/(double)ni) * (MOD(m,ni) + LONcpr_even);
		*lat = LATeven;
	}else if(timeO > timeE){											//Quando a mensagem ímpar chegar primeiro, calcularemos sua longitude e latitude
		ni = max(1,_cprNL(LATodd) - 1);
		//m = floor(LONcpr_even*(_cprNL(LATodd)-1) - LONcpr_odd*_cprNL(LATodd) + 0.5);
		*lon = (360.0/(double)ni) * (MOD(m,ni) + LONcpr_odd);
		*lat = LATodd;
	}

	if(*lon > 180){								//Se a longitude for maior do que 180, usaremos valores negativos.
		*lon = *lon - 360;
	}

}
int positionADSB(char *msgEVEN, char *msgODD, double timeE, double timeO, float *lat, float *lon){
	if(((5 <= typecodeADSB(msgEVEN)) && (typecodeADSB(msgEVEN) <= 8)) && ((5 <= typecodeADSB(msgODD)) && (typecodeADSB(msgODD) <= 8))){
		//surface_positionADSB(msgEVEN, msgODD, timeE, timeO, lat, lon);
	}else if(((9 <= typecodeADSB(msgEVEN))&&(typecodeADSB(msgEVEN) <= 18)) && ((9 <= typecodeADSB(msgODD)) && (typecodeADSB(msgODD) <= 18))){
		airbone_positionADSB(msgEVEN, msgODD, timeE, timeO, lat, lon);
	}else{
		printf("Incorrect or Inconsistent message types...\n");
		return -1;
	}
}
int altitudeADSB(char *msgi){		//Função que calcula a altitude em que a aeronave se encontra. Ela recebe uma string, que é a mensagem em hexadecimal, e retorna um inteiro, que é a altitude em feets. 
	if((typecodeADSB(msgi)<9)||(typecodeADSB(msgi)>18)){			//Verificamos se a mensagem é do tipo Posição. Se ela não for desse tipo, não podemos interpretar seus bits dessa forma.
		printf("It's not a Airbone Position Message\n");			//Se a mensagem não for do tipo Posição, avisamos isso e retornamos o inteiro -1, para que possa ser usado como um sinal de controle pelo função chamadora.
		return -1;
	}

	char msgbin[113], msgAUX[113];

	hex2bin(msgi,msgbin);
	char Qbit = msgbin[47];		//Qbit diz se a altitude está em múltiplos de 25(1) ou 100(0).

	if(Qbit == '1'){
		int N = 0;
	
		strncpy(msgAUX,&msgbin[40],7);	//Já que o Qbit deve ser tirado do calculo da altitude codificada, juntamos a parte anterior e posterior a esse bit em uma única string.
		msgAUX[7]='\0';
		strncat(msgAUX,&msgbin[48],4);
		msgAUX[11]='\0';
		N = bin2int(msgAUX);	//Calcula a altitude codificada.

//		printf("N:%d\n",N );
		return N*25 - 1000; 	//Atitude final

	}else{
		printf("Múltiplo de 100ft\n");
		return -1;
	}
}

int nicADSB(char *msgi){	//Essa função retorna o NIC da aeronave. Ela recebe uma string com a mensagem hexadecimal e retorna um inteiro, que representa o NIC.
	if((typecodeADSB(msgi)<9)||(typecodeADSB(msgi)>18)){			//Verificamos se a mensagem é do tipo Posição. Se ela não for desse tipo, não podemos interpretar seus bits dessa forma.
		printf("It's not a Airbone Position Message\n");			//Se a mensagem não for do tipo Posição, avisamos isso e retornamos o inteiro -1, para que possa ser usado como um sinal de controle pelo função chamadora.
		return -1;
	}

	char msgbin[113];
	hex2bin(msgi,msgbin);
	int tc = typecodeADSB(msgi);
	int NICsupB = msgbin[39] - 48;
	int NIC = 0;

	if((tc == 0) || (tc == 18) || (tc == 22)){
		NIC = 0;
	}else if(tc == 17){
		NIC = 1;
	}else if(tc == 16){
		if(NICsupB){
			NIC = 3;
		}else{
			NIC = 2;
		}
	}else if(tc == 15){
		NIC = 4;
	}else if(tc == 14){
		NIC = 5;
	}else if(tc == 13){
		NIC = 6;
	}else if(tc == 12){
		NIC = 7;
	}else if(tc == 11){
		if(NICsupB){
			NIC = 9;
		}else{
			NIC = 8;
		}
	}else if((tc == 10) || (tc == 21)){
		NIC = 10;
	}else if((tc == 9) || (tc == 20)){
		NIC = 11;
	}else{
		NIC = -1;
	}

	return NIC;
}

//////////////////////////AIRCRAFT VELOCITY/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int velocityADSB(char *msgi, float *speed, float *head, int *rateCD, char *tag){ //Essa função calcula a velocidade da aeronave. Ela recebe a mensagem em hexadecimal e retorna a velocidade(kt), o heading(graus), taxa de subida e descida(ft/min) e o tipo de velocidade(GS ou AS).
	if(typecodeADSB(msgi)!=19){			//Verificamos se a mensagem é do tipo velocidade. Se ela não for desse tipo, não podemos interpretar seus bits dessa forma.
		printf("It's not a Airbone Velocity Message\n");	//pesquisar assert()		//Se a mensagem não for do tipo velocidade, avisamos isso e retornamos o inteiro -1, para que possa ser usado como um sinal de controle pelo função chamadora.
		return -1;
	}

	char msgbin[113], msgAUX[113];
	int subtype = 0, VewSIGN = 0, VnsSIGN = 0, Vew = 0, Vns = 0, VrSIGN = 0, Vr = 0;

	hex2bin(msgi, msgbin);
	strncpy(msgAUX,&msgbin[37],3);
	msgAUX[3] = '\0';
	subtype = bin2int(msgAUX);			//Lê o subtipo de velocidade

	if((subtype == 1)||(subtype == 2)){
		VewSIGN = msgbin[45] - 48;			//Lê o sinal da velocidade East-West.
		strncpy(msgAUX,&msgbin[46],10);
		msgAUX[10] = '\0';
		Vew = bin2int(msgAUX) - 1;			//Lê a velociade East-West e subtrai 1.
	
		VnsSIGN = msgbin[56] - 48;			//Lê o sinal da velocidade North-South.
		strncpy(msgAUX,&msgbin[57],10);
		msgAUX[10] = '\0';
		Vns = bin2int(msgAUX) - 1;			//Lê a velociade North-South e subtrai 1.

		if(VewSIGN){					//Se a aeronave estiver no sentido contrário (East-West), invertemos o sinal.
			Vew = -1*Vew;
		}
		if(VnsSIGN){					//Se a aeronave estive no sentido contrário (North-South), invertemos o sinal.
			Vns = -1*Vns;				
		}

		*speed = sqrt(Vns*Vns + Vew*Vew);	//Calculamos a velocidade final.

		*head = atan2(Vew, Vns);			//Calculamos o heading em radianos
		*head = (*head)*180/M_PI;			//Passamos o heading para graus

		if(*head < 0){
			*head = *head + 360;
		}

		strcpy(tag,"GS");				//Informamos que a velocidade é GroundSpeed.
		tag[2] = '\0';

	}else{
		strncpy(msgAUX, &msgbin[46],10);
		msgAUX[10]='\0';

		*head = (bin2int(msgAUX)/1024)*360;

		strncpy(msgAUX, &msgbin[57],10);
		msgAUX[10]='\0';		

		*speed = bin2int(msgAUX);

		strcpy(tag,"AS");			//Informamos que a velocidade é AirSpeed.
		tag[2] = '\0';
	}

	VrSIGN = msgbin[68] - 48;			//Calcula a direção da velocidade vertical (UP/DOWN).
	strncpy(msgAUX,&msgbin[69],9);
	msgAUX[9] = '\0';
	Vr = bin2int(msgAUX);				//Calcula a taxa da velocidade vertical.
	*rateCD = (Vr - 1)*64;				//calcula a velociade em ft/min. Talvez isso deva ser comentado
	
	if(VrSIGN){
		*rateCD = -1*(*rateCD);//-1*Vr;				//DOWN
	}else{
		*rateCD = *rateCD;//Vr;					//UP
	}
}

void speed_headingADSB(char *msgi, float *speed, float *head){ 		//Recebe a mensagem em hexadecimal e retorna a velocidade escalar e o heading.
	char tag[3];
	int rateCD = 0;

	velocityADSB(msgi, speed,head,&rateCD,tag);  //Recebe a velocidade escalar, o heading, a taxa da velocidade vertical e a tag indicando o tipo de mensagem.
}

/////////////////////////////////////////////DECODIFICAÇÂO/////////////////////////////////////////////////////////////////////////
adsbMsg* decodeMessage(char* buffer, adsbMsg* messages, adsbMsg** nof){
	// struct sockaddr_in server;
	// int sockfd = 0;	
	adsbMsg* no = NULL;
	char icao[7]; icao[0] = '\0';								//icao é a identificação da aeronave. Ele consiste de 6 caracteres de 4 bits cada. Por isso, declaramos tamanho 7, por causa do caractere nulo.
	char tag[3];
	int rateV = 0, alt = 0;
	float lat = 0.0, lon = 0.0, heading = 0.0, vel_h = 0.0;

/////////////////////////VERIFICAÇÂO DO TIPO DE MENSAGEM///////////////////////////////////////////////////////////////////////////////////////////////////////////
	if((df_ADSB(buffer) == 17) && (strlen(buffer) == 28)){
		printf("\n\n***********ADSB MESSAGE*************\n");
		printf("MESSAGE:%s\n", buffer);

///////////////////////////REGISTRANDO NÓ PELO ICAO///////////////////////////////////////////////////////////////////////////////////////////////////////////
		icaoADSB(buffer, icao);
		if(messages == NULL){																//indica que é a primeira mensagem. Ou seja, devemos criar a lista.
			messages = criar(icao);
			no = messages;
		}else{
			if((no = inserir(icao, messages)) == NULL){							//Se ele recebeu NULL é porque esse icao já está registrado. 
				if((no = encontrar(icao, messages)) == NULL){
					perror("ICAO não encontrado");
					return messages;
				}									//Então ele vai pegar o nó registrado.					
			}
		}

		printf("\n-----------------------------------------------------------\n| ");
		printf("ICAO:%s\n", no->ICAO); 
		printf("-----------------------------------------------------------\n");

////////////////////////////CAPTURANDO CALLSIGN//////////////////////////////////////////////////////////////////////////////////////////////////////////
		if((1 <= typecodeADSB(buffer)) && (typecodeADSB(buffer) <= 4)){
			callsignADSB(buffer, no->callsign);
			
			printf("\n-------------------IDENTIFICAÇÂO----------------------------------------\n|");	
			printf(" CALLSIGN: %s\n", no->callsign );	printf("\t\n");
			printf("--------------------------------------------------------------------------\n");

		}

/////////////////////////////CAPTURANDO LAT LONG ALT/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if((9 <= typecodeADSB(buffer)) && (typecodeADSB(buffer) <= 18)){ 	 	// vamos ter que adicionar as msgs impares e pares nos nós para podermos usar as funções de velocidade
				no = addPosition(buffer, no);
			
			if((strlen(no->oeMSG[0]) != 0) && (strlen(no->oeMSG[1]) != 0)){											//Aqui verificamos se temos as duas mensagens (par e impar)
					airbone_positionADSB(no->oeMSG[0], no->oeMSG[1], no->oeTimestamp[0], no->oeTimestamp[1], &lat, &lon);		//Aqui pegamos a latitude e a longitude
					alt = altitudeADSB(buffer);																			//Aqui pegamos a altitude

					// no->Longitude = roundf(lon * 10000000000)/10000000000;
					// no->Latitude = roundf(lat * 10000000000)/10000000000;
					no->Longitude = lon;
					no->Latitude = lat;
					no->Altitude = alt;
					
					printf("\n-------------------POSICIONAMENTO--------------------------------------\n|");
					printf("LAT: %f\n|LON: %f\n|ALT: %d\n",no->Latitude,no->Longitude,no->Altitude); 
					printf("------------------------------------------------------------------------\n");

			}		
		}


////////////////////////////////CAPTURANDO VELOCIDADE//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
		else if(typecodeADSB(buffer) == 19){
			
			velocityADSB(buffer, &vel_h, &heading, &rateV, tag);

			no->horizontalVelocidade = vel_h;
			no->verticalVelocidade = rateV;
			no->groundTrackHeading = heading;
			strcpy(no->mensagemVEL, buffer);
			no->mensagemVEL[strlen(buffer)] = '\0';

			printf("-------------------------------------VELOCIDADE----------------------------------\n");
			printf("|VELH: %f\n|HEAD: %f\n|RATEV: %d\n|TAG: %s\n",no->horizontalVelocidade, no->groundTrackHeading, no->verticalVelocidade,tag); 
			printf("-------------------------------------------------------------------------------\n");

		}
	
	}else{
		printf("\n\n###############No ADS-B Message#####################\n");
		printf("|Message: %s\n", buffer );
	}
	memset(buffer, 0x0, 29);
	*nof = no;
	//printf("Teste NOF: P1:%s\nP2:%s\n", (*nof)->oeMSG[0],(*nof)->oeMSG[1]);
	
	return messages;
}

adsbMsg* isComplete(adsbMsg *no){
	if(no == NULL){
		//printf("NULL\n");
		return NULL;
	}
	if((strlen(no->oeMSG[0]) != 0) && (strlen(no->oeMSG[1]) != 0)){
		// no->oeMSG[0][0] = '\0';
		// no->oeMSG[1][0] = '\0';
		return no;
	}
}

adsbMsg * saveFullMessage(adsbMsg *list, adsbMsg *no){
	if(list == NULL){
		list = no;
		return list;
	}

	adsbMsg* aux = NULL;
	for(aux = list; aux->next != NULL; aux = aux->next);

	aux->next = no;
}