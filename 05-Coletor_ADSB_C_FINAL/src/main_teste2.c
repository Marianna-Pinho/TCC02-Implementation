// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <signal.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include "adsb_auxiliars.h"
// #include "adsb_lists.h"
// #include "adsb_time.h"
// #include "adsb_decoding.h"
// #include "adsb_serial.h"
// #include "adsb_db.h"
// #include "adsb_createLog.h"
// #include "adsb_network.h"

// adsbMsg *messagesList = NULL;

// volatile int timer_flag = 0;

// static void timerHandler(int sig, siginfo_t *si, void *uc){
//     timer_flag = 1;
// }

// void suddenStop(){

//  	printf("\nCtrl+C...\nClosing Collector...\n");

//  	LIST_removeAll(&messagesList);
 
//     printf("Collector closed!\n");
//     LOG_add("suddenStop", "Collector was closed");

//  	exit(1);
// }

// int main(){
//     int serialPort = 0, syndrome = 0;
//     char buffer[29]; buffer[0] = '\0';
//     adsbMsg *node = NULL;
//     adsbMsg *nodePost = NULL;

//     signal(2, suddenStop);

//     FILE *p = fopen("src/teste01_Todas.txt", "r");

//     if(p == NULL){
//         printf("File don't found\n");
        
//     }

//     //Starting Timer
//     TIMER_setSignalHandler(timerHandler, SIG);
//     timer_t timerid = TIMER_create(CLOCKID, SIG);
//     TIMER_setTimeout(TIMEOUT, timerid);

//     //Initializing semaphore
//     SEM_init();

//     pthread_t thread;
    
//     //This thread sends a hello to the server
//     int sendHello = pthread_create(&thread, NULL, NET_dataUpload, NULL);					//Cria uma thread responsável apenas por mandar um Hello do coletor para o servidor, a cada 1 min.
//     if (sendHello){
// 	 	printf("ERROR; return code from pthread_create() is %d\n", sendHello);
//  		exit(-1);
//  	}

//     while(fscanf(p," %s", buffer) != EOF){   //Polling method
//         buffer[strlen(buffer)] = '\0'; 

//         //If CRC returns 1, the message is correct. Otherwise, we don't do anything with the message.
//         if(CRC_tryMsg(buffer, &syndrome)){

//             messagesList = decodeMessage(buffer, messagesList, &node);

//             if(isNodeComplete(node) != NULL){
//                 if(DB_saveData(node) != 0){
//                     //()printf("The aircraft information couldn't be saved!\n");
//                 }else{
//                     //()printf("Aircraft %s information saved succesfully!\n", node->ICAO);

//                     if(DB_readData(&nodePost) != 0){
//                         printf("We coudn't read the aircraft information\n");
//                     }else{

//                         NET_addBuffer((void *)nodePost);

//                         clearMinimalInfo(node);
//                     }
                    
//                 }
//             }else{
//                 //()printf("Information is not complete!\n");
//             }
//         }
        
//         node = NULL;
//         nodePost = NULL;
// 		memset(buffer, 0x0, 29);

//         // //It cleans the old nodes in the messages list
//         if(timer_flag){
//             messagesList = LIST_delOldNodes(messagesList);
//             timer_flag = 0;
//         }

//         usleep(1000000); //0.001s
//     }
//     return 0;
// }