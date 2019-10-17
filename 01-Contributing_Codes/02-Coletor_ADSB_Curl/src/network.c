///// Standard Libs ///////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include<unistd.h> 

////// Socket Libs ///////
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

////// Error Libs /////////
#include <errno.h>

//// CURL and Json Libs /////
/* libcurl (http://curl.haxx.se/libcurl/c) */
#include <curl/curl.h>
/* json-c (https://github.com/json-c/json-c) */
#include </usr/include/json-c/json.h>

//// Collector Libs //////
#include "listFunctions.h"
#include "curlFunctions.h"
#include "userConfigs.h"
#include "network.h"

//Essa função é responsável por enviar um hello para o servidor a cada 1 min.
void *dataUploader(){			
	int timeCount = 0;

	while(1){
		if((timeCount % sendHelloInterval) == 0){
            printf("Putting... %d\n", timeCount);
			putMessage();
		}

		timeCount += 1;
        usleep(1000);
	}
}

//Essa função é responsável por realizar uma requisição do tipo POST.
void *postMessage(void *packet){             
    adsbMsg *msg = (adsbMsg*) packet;
    CURL *handle = NULL;  

    curlInitialization(&handle);
    if(handle == NULL){
        printf("Initialization didn't done.\n");
     
    }else{
        printf("Initialization done.\n");
        postRequest(handle, URL_POST, msg);   
    }

    pthread_exit(NULL); 
}

//Essa função é responsável por realizar uma requisição do tipo PUT para o endereço do coletor.
void *putMessage(){             
    CURL *handle = NULL;  

    curlInitialization(&handle);
    if(handle == NULL){
        printf("Initialization didn't done.\n");
     
    }else{
        printf("Initialization done.\n");
        putRequest(handle, URL_PUT, URL_COLLECTOR);   
    } 
}