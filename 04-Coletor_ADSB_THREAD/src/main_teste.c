// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include "adsb_lists.h"
// #include "adsb_decoding.h"
// #include "adsb_auxiliars.h"
// #include "adsb_time.h"
// #include "adsb_serial.h"
// #include "adsb_db.h"
// #include "adsb_createLog.h"
// #include "adsb_network.h"

// volatile int timer_flag = 0;
// static void handler(int sig, siginfo_t *si, void *uc)
// {
//     //flag = 1;
// }

// static void timerHandler(int sig, siginfo_t *si, void *uc){
//     timer_flag = 1;
// }

// void main(){
// /* ======= LISTS =============
//     char *s[7] = {"ABCDEF\0", "ABCDFF\0", "ABCDGF\0", "ABCDHF\0", "ABCDIF\0", "ABCDJF\0"};
//     adsbMsg * t, *aux2, *last;

//     t = LIST_create(s[0]);
//     t->uptadeTime = 1;

//      printf("Oi\n");

//      int i = 0;
//     for(i = 1; i < 6; i++){
//         LIST_insert(s[i], t);
//         aux2 = LIST_find(s[i], t);
//         aux2->uptadeTime = i+1;
//     }
    
//     adsbMsg *aux = t;
//     while(aux != NULL ){
//         printf("%s\n%lf\n", aux->ICAO, aux->uptadeTime);
//         last = aux;
//         aux = aux->next;
//     }

//     // adsbMsg *f = NULL;
//     // f = LIST_find("ABCDHF", t);
    
//     // if(f == NULL){
//     //     printf("Not found\n");
//     // }else{
//     //     printf("%s\n", f->ICAO);
//     // }
//     // printf("REMOVE\n");
//     // LIST_removeOne(s[1], &t);

//     // aux = t;
//     // while(aux != NULL ){
//     //     printf("%s\n", aux->ICAO);
//     //     aux = aux->next;
//     // }

//     // printf("REMOVE2\n");

//     // LIST_removeAll(&t);
//     // aux = t;
//     // while(aux != NULL ){
//     //     printf("%s\n", aux->ICAO);
//     //     aux = aux->next;
//     // }

//     printf("\nORDENAR\n");
//     adsbMsg* teste =  LIST_create(s[4]);
//     if(!LIST_orderByUpdate(teste->ICAO, last, &t)){
//         printf("Não ordenado\n");
//         return;
//     }

//     aux = t;
//     while(aux != NULL ){
//         printf("%s\n%lf\n", aux->ICAO, aux->uptadeTime);
//         last = aux;
//         aux = aux->next;
//     }
// ============================== */
// /* ======= Velocity and Position =======================
// float speed = 0.0, head = 0.0;
// int rate = 0;

// char *msgi = "8D485020994409940838175B284F";
// char tag[3];

// //getVelocities(msgi, &speed, &head, &rate, tag);
// //printf("Speed: %f,\nHead:%f,\nRate: %d,\nTag: %s\n", speed, head, rate, tag);

// float lat = 0.0, lon = 0.0;
// int alt = 0;

// char *msgE = "8D40621D58C382D690C8AC2863A7"; //"8DE48BB2586FE3EA4732DDA1DD67"
// char *msgO = "8D40621D58C386435CC412692AD6"; //"8DE48BB2586FD77B6B6A5E606AE2"

// getAirbornePosition(msgE, msgO,2.0,1.0,&lat,&lon);
// alt = getAltitude(msgE);

// printf("Lat:%f\nLon:%f\nAlt: %d\n", lat, lon, alt);}
// //======================================================*/

// /* ============ Decoding =============================*/
//     /****** Testing Timer ***** */
//     // printf("Set handler\n");
//     // TIMER_setSignalHandler(handler, SIG);
//     // printf("Create Timer\n");
//     // timer_t timerid = TIMER_create(CLOCKID, SIG);
//     // printf("Set Timeout\n");
//     // TIMER_setTimeout(TIMEOUT, timerid);

//     // char icaos[30][9];
//     // int pos = 0, syndrome = 0;
//     // char buffer[29];
//     // buffer[0] = '\0';

//     // adsbMsg *no = NULL;
//     // adsbMsg *messages = NULL;

//     // FILE *p = fopen("/home/marianna/Documentos/03-Bolsa_PIBITI/01-Coletor_versoes_PIBITI/07-Coletor_ADSB/src/teste01_Todas.txt", "r");

//     // if(p == NULL){
//     //     printf("File don't found\n");
//     // }

//     // while(fscanf(p," %s", buffer) != EOF){
//     //     buffer[strlen(buffer)] = '\0'; 

//     //     //Testes: salva todas as mensagens recebidos.
//     //     FILE *totalMsg = fopen("totalMsg.txt", "a");
//     //     fprintf(totalMsg,"%s,%s", buffer, getFormatedTime());
//     //     fclose(totalMsg);  

//     //     if(CRC_tryMsg(buffer, &syndrome)){

//     //         //Teste: salva todas as mensagens que não tiveram erros ou que seus erros foram corrigidos. (noerroMsg + correctCRC)
//     //         FILE *crcM = fopen("crcM.txt", "a");
//     //         fprintf(crcM,"%s,%s", buffer, getFormatedTime());
//     //         fclose(crcM);

//     //         messages = decodeMessage(buffer, messages, &no);
        
//     //         if(isNodeComplete(no) != NULL){
//     //             // printf("collectorKey: %s\nmodeSCode:%s\ncallsign:%s\nlatitude:%f\nlongitude:%f\n"\
//     //             // "altitude:%d\nverticalVelocity:%d\nhorizontalVelocity:%f\ngroundTrackHeading:%f\n"\
//     //             // "timestamp:%lf\ntimestampSent:%lf\nmessageDataId:%s\nmessageDataPositionEven:%s\n"\
//     //             // "messageDataPositionOdd:%s\nmessageDataVelocity:%s\n", no->COLLECTOR_ID, no->ICAO,
//     //             // no->callsign, no->Latitude, no->Longitude, no->Altitude, no->verticalVelocity,
//     //             // no->horizontalVelocity, no->groundTrackHeading, no->oeTimestamp[0], no->oeTimestamp[1],
//     //             // no->messageID, no->oeMSG[0], no->oeMSG[1], no->mensagemVEL);

//     //             if(DB_saveData(no) != 0){
//     //                 printf("The aircraft information couldn't be saved!\n");
//     //             }else{
//     //                 printf("Aircraft %s information saved succesfully!\n", no->ICAO);
//     //                 clearMinimalInfo(no);
//     //             }
//     //         }else{
//     //             printf("Information is not complete!\n");
//     //         }
//     //     }
        
//     //     no = NULL;
// 	// 	memset(buffer, 0x0, 29);

//     //     if(flag){
//     //         messages = LIST_delOldNodes(messages);
//     //         flag = 0;
//     //     }


//     //     sleep(1);

//     // }

//     // adsbMsg *aux = messages;
//     // while(aux != NULL){
//     //     printf("\nICAO: %s\n", aux->ICAO);
//     //     printf("CallSign: %s\n", aux->callsign);
//     //     printf("Latitude: %lf\n", aux->Latitude);
//     //     printf("Longitude: %lf\n", aux->Longitude);
//     //     printf("Altitude: %d\n", aux->Altitude);
//     //     printf("HV: %lf\n", aux->horizontalVelocity);
//     //     printf("VV: %d\n", aux->verticalVelocity);
//     //     printf("Update: %lf\n\n", aux->uptadeTime);

//     //     aux = aux->next;
//     //     sleep(1);
//     // }

//     /*================== CRC Testing ================= */
    
//     // char *msg = "8DE48A6F587B64D5496D980301F5";
//     // int syndrome;
//     // printf("Valid? %d\n", CRC_verifyMsg(msg, &syndrome));

//    /* ============= Testing LIST_orderByUpdate ===================

//    char *ICAOS[] = {"305678", "824A88", "8982DA", "146819", "E48413", "E48501", "E491A1", "93D870", "E48A6F", "E49246"};
//    messages = LIST_create(ICAOS[0]);
//    LIST_insert(ICAOS[1], messages);
//    LIST_insert(ICAOS[2], messages);
//    LIST_insert(ICAOS[3], messages);
//    LIST_insert(ICAOS[4], messages);
//    LIST_insert(ICAOS[5], messages);
//    LIST_insert(ICAOS[6], messages);
//    LIST_insert(ICAOS[7], messages);
//    LIST_insert(ICAOS[8], messages);
//    LIST_insert(ICAOS[9], messages);

//    adsbMsg *last = LIST_orderByUpdate(ICAOS[9], LASTNODE, &messages);
   
//     adsbMsg *aux = messages;
//     while(aux != NULL){
//         printf("\nICAO: %s\n", aux->ICAO);
//         printf("CallSign: %s\n", aux->callsign);
//         printf("Latitude: %lf\n", aux->Latitude);
//         printf("Longitude: %lf\n", aux->Longitude);
//         printf("Altitude: %d\n", aux->Altitude);
//         printf("HV: %lf\n", aux->horizontalVelocity);
//         printf("VV: %d\n", aux->verticalVelocity);
//         printf("Update: %lf\n\n", aux->uptadeTime);

//         aux = aux->next;
//         sleep(1);
//     }

//     printf("LAST: %s\n", last->ICAO); */

//  /*======= Testando Função de Remoção de Nós Antigos ==========
//    adsbMsg * no_aux = NULL;
//    char *ICAOS[] = {"305678", "824A88", "8982DA", "146819", "E48413", "E48501", "E491A1", "93D870", "E48A6F", "E49246"};
//    messages = LIST_create(ICAOS[0]);
//    messages->uptadeTime = getCurrentTime();
//    no_aux = LIST_insert(ICAOS[1], messages);
//    no_aux->uptadeTime = getCurrentTime();
//    no_aux = LIST_insert(ICAOS[2], messages);
//    no_aux->uptadeTime = getCurrentTime();
//    no_aux = LIST_insert(ICAOS[3], messages);
//    no_aux->uptadeTime = getCurrentTime();
//    no_aux = LIST_insert(ICAOS[4], messages);
//    no_aux->uptadeTime = getCurrentTime();
//    no_aux = LIST_insert(ICAOS[5], messages);
//    no_aux->uptadeTime = getCurrentTime();
//    no_aux = LIST_insert(ICAOS[6], messages);
//    no_aux->uptadeTime = getCurrentTime();
//    no_aux = LIST_insert(ICAOS[7], messages);
//    no_aux->uptadeTime = getCurrentTime();
//    sleep(11);
//    no_aux = LIST_insert(ICAOS[8], messages);
//    no_aux->uptadeTime = getCurrentTime();
//    no_aux =  LIST_insert(ICAOS[9], messages);
//    no_aux->uptadeTime = getCurrentTime();
//    messages = LIST_delOldNodes(messages);

   

//    adsbMsg *aux = messages;
//     while(aux != NULL){
//         printf("\nICAO: %s\n", aux->ICAO);
//         printf("CallSign: %s\n", aux->callsign);
//         printf("Latitude: %lf\n", aux->Latitude);
//         printf("Longitude: %lf\n", aux->Longitude);
//         printf("Altitude: %d\n", aux->Altitude);
//         printf("HV: %lf\n", aux->horizontalVelocity);
//         printf("VV: %d\n", aux->verticalVelocity);
//         printf("Update: %lf\n\n", aux->uptadeTime);

//         aux = aux->next;
//         sleep(1);
//     }*/


// /*========================================================= */
// /* =============== Serial communication ====================
// char sbuffer[29];
// int serialPort = 0;

// serialPort = SERIAL_start();

//     while(1){
//         if (SERIAL_communicate(&serialPort, sbuffer) == 10 ){			//Se a mensagem for um caractere branco, que nesse caso é o NL, não executaremos o restante das instruções.
//                 continue;
//         }
//         printf("Buffer: %s\n", sbuffer);
//     }

//     char msg[43] = "@8B305678CA11029;";
//     SERIAL_removeFL(msg);
//     printf("M:%s\n", msg);
// ================================================================ */
// /* =========== Database ========================================*/
//     // sqlite3 *dbh = NULL;
//     // char *db_name = "radarlivre_v4.db";

//     // // dbh = DB_open(db_name);
//     // // if(dbh == NULL){
//     // //     printf("Database opening failed!");
//     // // }
//     // adsbMsg* aux = NULL;
//     // adsbMsg *teste = LIST_create("ABCDEF",&aux);
//     // strcpy(teste->COLLECTOR_ID,"123");
//     // strcpy(teste->callsign, "AAA");
//     // teste->Latitude = 1;
//     // teste->Longitude = 2;
//     // teste->Altitude = 3;
//     // teste->verticalVelocity = 4;
//     // teste->horizontalVelocity = 5;
//     // teste->groundTrackHeading = 6;
//     // teste->oeTimestamp[teste->lastTime] = 7;
//     // strcpy(teste->messageID, "QWERT");
//     // strcpy(teste->oeMSG[0], "aaaaaa");
//     // strcpy(teste->oeMSG[1], "bbbbbb");
//     // strcpy(teste->messageVEL, "cccccccc");
    
//     // DB_saveData(teste);

// /*------------------------------------------------------------*/
// // adsbMsg *node = NULL;
// // printf("P1:%p\n", node);
// // DB_readData(&node);
// // printf("P4:%p\n", node);


// // printf("===== MAIN ======\n");
// //    printf("COLID:%s\n", node->COLLECTOR_ID);
// //    printf("ICAO:%s\n", node->ICAO);
// //    printf("Callsign:%s\n", node->callsign);
// //    printf("Lat:%f\n", node->Latitude);
// //    printf("Long:%f\n", node->Longitude);
// //    printf("Alt:%d\n", node->Altitude);
// //    printf("VV:%d\n", node->verticalVelocity);
// //    printf("HV:%f\n", node->horizontalVelocity);
// //    printf("GT:%f\n", node->groundTrackHeading);
// //    printf("T0:%f\n", node->oeTimestamp[0]);
// //    printf("T1:%f\n", node->oeTimestamp[1]);
// //    printf("msgId:%s\n", node->messageID);
// //    printf("msgEven:%s\n", node->oeMSG[0]);
// //    printf("msgOdd:%s\n", node->oeMSG[1]);
// //    printf("msgVel:%s\n", node->messageVEL);


// // printf("\n\nP5:%p\n", node);
// // DB_readData(&node);
// // printf("P6:%p\n", node);

// // printf("===== MAIN2 ======\n");
// //    printf("COLID:%s\n", node->COLLECTOR_ID);
// //    printf("ICAO:%s\n", node->ICAO);
// //    printf("Callsign:%s\n", node->callsign);
// //    printf("Lat:%f\n", node->Latitude);
// //    printf("Long:%f\n", node->Longitude);
// //    printf("Alt:%d\n", node->Altitude);
// //    printf("VV:%d\n", node->verticalVelocity);
// //    printf("HV:%f\n", node->horizontalVelocity);
// //    printf("GT:%f\n", node->groundTrackHeading);
// //    printf("T0:%f\n", node->oeTimestamp[0]);
// //    printf("T1:%f\n", node->oeTimestamp[1]);
// //    printf("msgId:%s\n", node->messageID);
// //    printf("msgEven:%s\n", node->oeMSG[0]);
// //    printf("msgOdd:%s\n", node->oeMSG[1]);
// //    printf("msgVel:%s\n", node->messageVEL);

// //========================================== 
// /* ================= Data Upload ================= */
//     // adsbMsg *messagesList = NULL;

//     // int serialPort = 0, syndrome = 0;
//     // char buffer[29]; buffer[0] = '\0';
//     // adsbMsg *node = NULL;
//     // pthread_t thread;

//     // //Starting Timer
//     // TIMER_setSignalHandler(timerHandler, SIG);
//     // timer_t timerid = TIMER_create(CLOCKID, SIG);
//     // TIMER_setTimeout(TIMEOUT, timerid);

//     // FILE *p = fopen("src/teste01_Todas.txt", "r");

//     // if(p == NULL){
//     //     printf("File don't found\n");
        
//     // }

//     // int du = pthread_create(&thread, NULL, NET_dataUpload, NULL);					//Cria uma thread responsável apenas por mandar um Hello do coletor para o servidor, a cada 1 min.
//     // if (du){
// 	//  	printf("ERROR; return code from pthread_create() is %d\n", du);
//  	// 	exit(-1);
//  	// }

//     // while(fscanf(p," %s", buffer) != EOF){   //Polling method
//     //     buffer[strlen(buffer)] = '\0'; 


//     //     //If CRC returns 1, the message is correct. Otherwise, we don't do anything with the message.
//     //     if(CRC_tryMsg(buffer, &syndrome)){

//     //         messagesList = decodeMessage(buffer, messagesList, &node);

//     //         if(isNodeComplete(node) != NULL){
//     //             if(DB_saveData(node) != 0){
//     //                 //()printf("The aircraft information couldn't be saved!\n");
//     //             }else{
//     //                 //()printf("Aircraft %s information saved succesfully!\n", node->ICAO);
//     //                 int pm = pthread_create(&thread, NULL, NET_postMsg, (void *)node);				//Se a mensagem já contiver os dados suficientes, ela será enviada.
//     //                 if (pm){
//     //                     printf("ERROR; return code from pthread_create() is %d\n", pm);
//     //                     exit(-1);
//     //                 }
                    

//     //             }
//     //         }else{
//     //             //()printf("Information is not complete!\n");
//     //         }
//     //     }
        
//     //     node = NULL;
// 	// 	memset(buffer, 0x0, 29);

//     //     // //It cleans the old nodes in the messages list
//     //     if(timer_flag){
//     //         messagesList = LIST_delOldNodes(messagesList);
//     //         timer_flag = 0;
//     //     }

// 	// 	sleep(1);
//     // }
   


//     // int du = pthread_create(&thread, NULL, NET_dataUpload, NULL);					//Cria uma thread responsável apenas por mandar um Hello do coletor para o servidor, a cada 1 min.
//     // if (du){
//     //     printf("ERROR; return code from pthread_create() is %d\n", du);
//     //     exit(-1);
//     // }

//     // adsbMsg* node = NULL;
//     // adsbMsg *teste = LIST_create("E491A1", &node);
//     // strcpy(teste->callsign, "A");
//     // teste->callsign[strlen(teste->callsign)] = '\0';
//     // teste->Latitude = -4.764374;
//     // teste->Longitude = -39.116211;
//     // teste->Altitude = 22300;
//     // teste->verticalVelocity = -2880;
//     // teste->horizontalVelocity = 460.508423;
//     // teste->groundTrackHeading = 354.017029;
//     // teste->oeTimestamp[teste->lastTime] = 619987240;
//     // strcpy(teste->messageID, "8DE491A120501373E39C603B0D55");
//     // strcpy(teste->oeMSG[0], "8DE491A1587570D2112DC1359251");
//     // strcpy(teste->oeMSG[1], "8DE491A1587544E06F65595BF018");
//     // strcpy(teste->messageVEL, "8DE491A19914313968B837E83E8D");

//     // int count = 0;
//     // while(1){
//     //         int pm = pthread_create(&thread, NULL, NET_postMsg, (void *)teste);				//Se a mensagem já contiver os dados suficientes, ela será enviada.
// 	// 	    if (pm){
// 	//  			printf("ERROR; return code from pthread_create() is %d\n", pm);
//  	// 			exit(-1);
//  	// 		}
        
//     //     usleep(500000);
//     // }	

// /*================ TESTING LOGGING ===============
//     char *source = "main_code";
//     char *content = "testing log functionasdasdasdaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

//     if(LOG_add(source, content) == -1){
//         printf("Log couldn't be added!\n");
//     }else{
//         printf("Log added!\n");
//     }

// =========================================*/

// /*=========== Semaphore ===================*/
//     adsbMsg *node = NULL;
//     adsbMsg *teste1 = LIST_create("E491A1", &node);
//     adsbMsg *teste2 = LIST_create("E491A1", &node);
//     adsbMsg *teste3 = LIST_create("E491A1", &node);
//    //---------------------------------------------------------
//     teste1->Latitude = -4.764374;
//     teste1->Longitude = -39.116211;
//     teste1->Altitude = 22300;
//     teste1->verticalVelocity = -2880;
//     teste1->horizontalVelocity = 460.508423;
//     teste1->groundTrackHeading = 354.017029;
//     teste1->oeTimestamp[teste1->lastTime] = 619987240;
//     strcpy(teste1->messageID, "8DE491A120501373E39C603B0D55");
//     teste1->messageID[strlen(teste1->messageID)] = '\0';
//     strcpy(teste1->oeMSG[0], "8DE491A1587570D2112DC1359251");
//     teste1->oeMSG[0][strlen(teste1->oeMSG[0])] = '\0';
//     strcpy(teste1->oeMSG[1], "8DE491A1587544E06F65595BF018");
//     teste1->oeMSG[1][strlen(teste1->oeMSG[1])] = '\0';
//     strcpy(teste1->messageVEL, "8DE491A19914313968B837E83E8D");
//     teste1->messageVEL[strlen(teste1->messageVEL)] = '\0';
//     strcpy(teste1->callsign, "A");
//     teste1->callsign[strlen(teste1->callsign)] = '\0';
// //-------------------------------------------------------------------------
//     teste2->Latitude = -4.794374;
//     teste2->Longitude = -39.226211;
//     teste2->Altitude = 22300;
//     teste2->verticalVelocity = -2880;
//     teste2->horizontalVelocity = 460.508423;
//     teste2->groundTrackHeading = 354.017029;
//     teste2->oeTimestamp[teste2->lastTime] = 619987240;
//     strcpy(teste2->messageID, "8DE491A120501373E39C603B0D55");
//     teste2->messageID[strlen(teste2->messageID)] = '\0';
//     strcpy(teste2->oeMSG[0], "8DE491A1587570D2112DC1359251");
//     teste2->oeMSG[0][strlen(teste2->oeMSG[0])] = '\0';
//     strcpy(teste2->oeMSG[1], "8DE491A1587544E06F65595BF018");
//     teste2->oeMSG[1][strlen(teste2->oeMSG[1])] = '\0';
//     strcpy(teste2->messageVEL, "8DE491A19914313968B837E83E8D");
//     teste2->messageVEL[strlen(teste2->messageVEL)] = '\0';
//     strcpy(teste2->callsign, "B");
//     teste2->callsign[strlen(teste2->callsign)] = '\0';
//     //--------------------------------------------------------------
//     teste3->Latitude = -5.764374;
//     teste3->Longitude = -39.116211;
//     teste3->Altitude = 22300;
//     teste3->verticalVelocity = -2880;
//     teste3->horizontalVelocity = 460.508423;
//     teste3->groundTrackHeading = 354.017029;
//     teste3->oeTimestamp[teste3->lastTime] = 619987240;
//     strcpy(teste3->messageID, "8DE491A120501373E39C603B0D55");
//     teste3->messageID[strlen(teste3->messageID)] = '\0';
//     strcpy(teste3->oeMSG[0], "8DE491A1587570D2112DC1359251");
//     teste3->oeMSG[0][strlen(teste3->oeMSG[0])] = '\0';
//     strcpy(teste3->oeMSG[1], "8DE491A1587544E06F65595BF018");
//     teste3->oeMSG[1][strlen(teste3->oeMSG[1])] = '\0';
//     strcpy(teste3->messageVEL, "8DE491A19914313968B837E83E8D");
//     teste3->messageVEL[strlen(teste3->messageVEL)] = '\0';
//     strcpy(teste3->callsign, "C");
//     teste3->callsign[strlen(teste3->callsign)] = '\0';
    
// //------------------------------------------------------------------------

//     char *jsonFinal = (char *)malloc(sizeof(char));
//     jsonFinal[0]='\0';

//     SEM_init();
//     pthread_t t1,t2;
//     pthread_create(&t2,NULL,NET_addBuffer,(void*)teste1);
//     NET_addBuffer((void *)teste2);
//     NET_addBuffer((void *)teste3);
//     pthread_join(t2,NULL);

//    //jsonFinal =  NET_readBuffer(jsonFinal);
//    //printf("JsonFinal:\n%s\n%p\n", jsonFinal,jsonFinal);
//    //jsonFinal[0] = '\0';

//     NET_postMsg();
  
//     SEM_destroy();  
// /*==================================== */
// /*========== List insert 2 ================

// adsbMsg *list = (adsbMsg*)malloc(sizeof(adsbMsg));
// adsbMsg *node = NULL;
// adsbMsg *teste = LIST_create("E491A1", &node);
// strcpy(teste->callsign, "A");
// teste->callsign[strlen(teste->callsign)] = '\0';
// teste->Latitude = -4.764374;
// teste->Longitude = -39.116211;
// teste->Altitude = 22300;
// teste->verticalVelocity = -2880;
// teste->horizontalVelocity = 460.508423;
// teste->groundTrackHeading = 354.017029;
// teste->oeTimestamp[teste->lastTime] = 619987240;
// strcpy(teste->messageID, "8DE491A120501373E39C603B0D55");
// strcpy(teste->oeMSG[0], "8DE491A1587570D2112DC1359251");
// strcpy(teste->oeMSG[1], "8DE491A1587544E06F65595BF018");
// strcpy(teste->messageVEL, "8DE491A19914313968B837E83E8D");

//  *list = *teste;
//  list = LIST_insert2(list, teste);
//  list = LIST_insert2(list, teste);
//  list = LIST_insert2(list, teste);

// adsbMsg *aux = list;

// printf("ola\n");
// while(aux != NULL){
//         printf("\nICAO: %s\n", aux->ICAO);
//         printf("CallSign: %s\n", aux->callsign);
//         printf("Latitude: %lf\n", aux->Latitude);
//         printf("Longitude: %lf\n", aux->Longitude);
//         printf("Altitude: %d\n", aux->Altitude);
//         printf("HV: %lf\n", aux->horizontalVelocity);
//         printf("VV: %d\n", aux->verticalVelocity);
//         printf("Update: %lf\n\n", aux->uptadeTime);

//         aux = aux->next;
//         sleep(1);
//     }
// =================================================*/

// }
//     // printf("Add:\n");
//     // adsbMsg*aux = sendList;
//     // for(aux = sendList; aux!=NULL; aux = aux->next){
//     //     printf("\nICAO: %s\n", aux->ICAO);
//     //     printf("CallSign: %s\n", aux->callsign);
//     //     printf("Latitude: %lf\n", aux->Latitude);
//     //     printf("Longitude: %lf\n", aux->Longitude);
//     //     printf("Altitude: %d\n", aux->Altitude);
//     //     printf("HV: %lf\n", aux->horizontalVelocity);
//     //     printf("VV: %d\n", aux->verticalVelocity);
//     //     printf("Update: %lf\n", aux->uptadeTime);
//     //     printf("Even: %s\n", aux->oeMSG[0]);
//     //     printf("Odd: %s\n", aux->oeMSG[1]);
//     //     printf("Id: %s\n", aux->messageID);
//     //     printf("Vel: %s\n\n\n", aux->messageVEL);
//     // }