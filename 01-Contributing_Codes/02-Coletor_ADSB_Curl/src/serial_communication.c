
#include <stdio.h>
#include <fcntl.h>   //file control
#include <errno.h>
#include <termios.h> //terminal communication control
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  //read function
#include <sys/ioctl.h>
#include "serial_communication.h"

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
void formatMESSAGE(char *msg){				/*/Essa função retira o caractere inicial e final, que indicam início e fim de mensagem.*/
	unsigned int i = 0;
	while(msg[i]!='\0'){
		i++;
	}
	if(i>1){
		msg[i-2] = '\0';				//Retira o ;	

		for(i=0;msg[i]!='\0';i++){
			msg[i] = msg[i+1];
		}								//Retira o @
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void getFRAME(char *msg){
	//printf("TAM:%ld \n", strlen(msg));
	if((strlen(msg) > 26) && (strlen(msg) < 41)){
		strncpy(msg, &msg[12], 28);
		msg[28] = '\0';
	}else{
		strncpy(msg, &msg[12], 14);
		msg[14] = '\0';
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int openSPORT(void){					/*/Essa função estabelece a conexão com a porta serial, criando um descritor de arquivos para ela.*/
	int fd = 0;
	do{ 								//Ficaremos nesse loop até conseguirmos abrir o descritor de arquivo.
		fd = open(SERIALPORT, O_RDWR | O_NOCTTY); 		//Aqui dizemos que podemos escrever para ou ler do receptor microADSB, e que a comunicação aberta não controla o terminal.
		if(fd < 0){
			perror(SERIALPORT);			
		}
	}while(fd < 0);
	printf("MicroADSB_Colector: opened!\n");
	return fd;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void configSPORT(int fd){ 						//Aqui configuramos como será realizada a comunicação serial.
	int status = 0, st_read = 0;
	struct termios newsport, oldsport;
	
	status = tcgetattr(fd, &oldsport); 				//Pegamos as atuais configurações da comunicação.
	if(status < 0){
		perror("SERIAL DEVICE"); 					//Se não foi possível pegá-las, geramos um erro. Provavelmente, a comunicação não foi corretamente aberta.
		exit(0);
	}

	bzero(&newsport, sizeof(newsport)); 			//Estamos zerando todos os campos dessa estrutura, para evitar lixos.
	newsport.c_iflag = ICRNL;						//Aqui dizemos que o CR será transformado em NL na mensagem enviada.
	newsport.c_oflag = 0;							//Não alteramos nenhuma configuração de controle de saída.
	newsport.c_cflag = CS8 | CREAD | CLOCAL; 		//Aqui habilitamos a recepção de mensagens.
	newsport.c_lflag = ICANON;						//Aqui dizemos que estamos usando o modo não canônico, que lê toda a linha de dados disponível.

	cfsetispeed(&newsport, BAUDRATE); 				//Aqui configuramos a taxa de dados de entrada, que é de 112500 mbps.
	cfsetospeed(&newsport, BAUDRATE);				//Aqui configuramos a taxa de dados de saída, que é de 112500 mbps.

	tcflush(fd, TCIFLUSH);							//Aqui apagamos todas as mensagens recebidas, mas não lidas, até o momento. É como limpar o buffer do sistema.

	status = tcsetattr(fd, TCSANOW, &newsport);		//Aqui salvamos as configurações realizadas na estrutura que representa a porta serial.
	if(status < 0){
		perror("SET ATTRIBUTE");
		exit(0);
	}
	//0100 0000
	if(write(fd,"#43-02\n",7) != 7){				//Aqui dizemos para o receptor que ele pode começar a mandar as mensagens. #43-02 é uma string de inicialização.
			printf("INITIALIZATION ERROR\n");
			return;
	}
	printf("MicroADSB_Colector: configured!\n");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int readSPORT(int fd, char *BUFF){					//Essa é a função que recebe a mensagem enviada pelo microADSB.
	int st_read = 0, serial = 0;

	st_read = read(fd, BUFF, 43);					//Aqui passamos para o buffer(BUFF) uma mensagem de até 43*4 bits, que é a mensagem enviada pelo microADSB.
	if(st_read < 0){								//Se não tivermos conseguido ler, geramos um erro.
		perror("Can't read serial port");
		
	}	
	BUFF[st_read] = '\0';							//Completamos a string, com o '\0'.
	
	//printf("B: %s\n", BUFF);	//DEBUG
	formatMESSAGE(BUFF);
	//printf("B: %ld\n", strlen(BUFF));	//DEBUG
								//Aqui retiramos os caracteres de início e fim de mensagem.
	if(BUFF[0] == '\0'){
		return -1;									//Se o buffer só tiver '\0', significa que o receptor está desconectado, ou seja, ele não está recebendo nada.
	}
		//getFRAME(BUFF);
		return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int disconnectedSPORT(int fd, int status){			//Aqui tratamos o caso em que o microADSB é desconectado com o programa em execução.
	while(status == -1){
		if(close(fd) < 0){							//Liberamos o descritor de arquivo que havia sido aberto.
		   perror("Serial port can't be closed");		
		}else{
		   			
		   sleep(1);								//Esperamos um segundo até tentarmos nos conectar novamente.
	       fd = openSPORT();
	       sleep(6);								//Uma vez estabelecida a conexão, esperamos um segundo para iniciar as configurações.Esses tempos são tentativas de intervalos.
		   configSPORT(fd);
		   printf("Serial port closed\n");
		   return fd;								//Retornamos o novo descritor de arquivos, correspondente à nova conexão.
		}
	}
		printf("Passou direto\n");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int serialSTART(){									//Essa é a função de inicialização, que deve ser chamada antes de qualquer leitura.
	int fd = 0;

	fd = openSPORT();
	configSPORT(fd);

	return fd;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char serialREADING(int* fd, char *BUFF){				//Essa é a função de leitura das mensagens.
	int pstatus = 1;

	while(pstatus!=0){
		pstatus = readSPORT(*fd, BUFF);				//Se conseguimos ler, mas a mensagem é um caractere branco, tentamos ler outra vez. Ela retorna 0 caso não seja um caractere nulo.
		if(strlen(BUFF)==1){						//Então, temos que fazer pstatus receber 1, para que não imprimamos o caractere \n.
			pstatus = 1;
			continue; //Isso, provavelmente, pode ser tirado se colocarmos para ler 44 bytes ao invés de 43
		}
		if(pstatus == -1){								//Se a pstatus for == -1, quer dizer que o receptor está desconectado. Logo, tentaremos reestabelecer a conexão.
			*fd = disconnectedSPORT(*fd, pstatus);
			printf("MicroADSB_Colector: reading!\n");
		}	
	}
	getFRAME(BUFF);
	return BUFF[0];
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

