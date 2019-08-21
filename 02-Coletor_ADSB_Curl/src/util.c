#include <stdio.h>
#include <string.h>
#include "util.h"


//Essa função deve receber toda a mensagem serial formatada e retornar apenas os 112 bits de interesse.
void frameADSB(char *msgi, char *msgf){				//msgi: mensagem inicial/ msgf: mensagem final.
	int i = 0, j=0;
	for(i=12;msgi[i]!='\0';i++){
		msgf[j] = msgi[i];
		j++;
	}
	msgf[j]='\0';
}


int hex2int(char caractere){ 			//Essa função converte caracteres em hexadecimal para sua representação na base decimal, retornando um inteiro.
	if((48<=caractere)&&(caractere<=57)){
		return(int)caractere - 48;
	}else if((65<=caractere)&&(caractere<=70)){
		return (int)caractere - 55;
	}
}

void int2bin(int num, char *msgbin){			//Essa função converte um número inteiro em uma string com sua representação em binários.
	int resto = 0, aux = num, div = 0, i = 3;
	while(i>=0){
		msgbin[i] = (aux%2)+48;
		aux = aux/2;
		i--;
	}
}

//Essa função recebe uma string com um valor hexadecimal e preenche uma string como aquele valor em binário.
void hex2bin(char *msgi, char *msgbin){			//msgi: mensagem em hexadecimal/ msgbin: receberá a mensagem em binário.
	int i = 0;

	for(i=0;msgi[i]!='\0';i++){					//Cada caractere será convertido em um inteiro e esse inteiro será convertido em um binário(string).
		int2bin(hex2int(msgi[i]), &msgbin[i*4]);

	}
		msgbin[i*4] = '\0';
}

int bin2int(char *msgbin){					//Essa função converte um valor em binário em um inteiro. Entretanto, ela possui a limitação dos valores máximos que um inteiro pode assumir.
	int tam = strlen(msgbin);				//Se o inteiro for muito grande, ele será abreviado. Os valores são -2^(32) <= x <= 2^(31).
	int soma = 0, mult = 1;
	int i = 0;

	for(i=tam-1;i>=0;i--){
		soma+=(msgbin[i] - 48)*mult;
		mult*=2;
		//printf("SOMA:%d\n", soma);
	}
	return soma;
}

int downlinkFormat(char *msgi){ 			//Função que retorna o valor de DownlinkFormat da mensagem ADSB.
	char msg_hex[3], msgbin[9], msgbin_aux[6];
	msg_hex[0] = msgi[0];
	msg_hex[1] = msgi[1];
	msg_hex[2] = '\0';

	hex2bin(msg_hex,msgbin);
	strncpy(msgbin_aux,msgbin,5);
	msgbin_aux[5] = '\0';

	return bin2int(msgbin_aux);
}

/*int floor(float num){		//Função que retorna o piso de um número quebrado.
	if(num >= 0){
		return (int)num;		//Se o número for positivo, retornamos o número após um cast para inteiro.	
	}
	
	return (num - (1-((int)num-num)));	 //Se o número for negativo, completamos ele com o decimal negativo que falta para ele ir para o inteiro mais próximo, menor do que ele.
}
*/
void crc(char *msg_hex, int encode, char *reminder){

	int i=0,j=0, tam = 0;
	char GENERATOR[] = "1111111111111010000001001";			//GENERATOR é o polinômio padrão para a geração de checksum e para a verificação da validade das mensagens recebidas.
	
	tam = strlen(msg_hex)*4+1;								//tam é o tamanho do vetor que guarda a mensagem em binário. Cada caractere hexadecimal possui 4 bits, então tam será o tamanho da mensagem hexadecimal vezes 4 + 1 bit para o caractere nulo '\0'.
	char msgbin[tam];										//msgbin é um vetor auxiliar que receberá a versão binária da mensagem de entrada.
	
	hex2bin(msg_hex,msgbin);								//Transforma a mensagem hexadecimal em binária.
	
	if(encode){												//Se enconde == 1, zeraremos todos os últimos 24 bits, que são usados para a paridade.
		for(i=tam-1;i>=(tam-24);i--){
			msgbin[i] = '0';
		}
	}

	for(i=0; i<tam-24; i++){								//Temos 24 bits de paridade. Entretanto, o gerador tem 25 bits, pois sua última atuação tem que englobar o último bit de mensagem antes dos de paridade. Isso soma 25 bits.
		if(msgbin[i] == '1'){								//A condição msgbin[i] = 1 trata mensagens do tipo 00110, que, na verdade, são 110.  
			for(j=0;j<strlen(GENERATOR);j++){
				if((msgbin[i+j]-48)^(GENERATOR[j]-48)){		//Aqui é realizado o xor entre um bit da mensagem e um bit correspondente no polinômio gerador.	
					msgbin[i+j]='1';						//Temos que salvar o resultado dessa operação de volta na mesma posição do bit da mensagem que foi utilizado.
				}else{
					msgbin[i+j]='0';
				}
			}
		}
	}	
	
	strncpy(reminder,&msgbin[tam-25],24);					//Aqui passamos os 24 bits de paridade, depois de toda a mensagem ter passado pela divisão, para um reminder da função que chamou a função crc.			
	reminder[24] = '\0';									//Estamos subtraindo 25 e não 24 do tamanho da mensagem total, pq esta tem um bit a mais reservado para o caractere nulo '\0' e ele é considerado no cálculo de tam.
}