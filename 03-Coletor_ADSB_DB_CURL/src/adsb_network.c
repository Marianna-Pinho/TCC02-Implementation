#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>
#include </usr/include/json-c/json.h>
#include "adsb_createLog.h"
#include "adsb_lists.h"
#include "adsb_time.h"
#include "adsb_userInfo.h"
#include "adsb_network.h"

/*==============================================
FUNCTION: CURL_init
INPUT: a curl pointer to pointer
OUTPUT: void
DESCRIPTION: this function receives a curl pointer
to pointer and tries to save into it a handler for
curl operations.
================================================*/
void CURL_init(CURL **handler){

    //It initialize a curl handle
	*handler = curl_easy_init();				
    
    if (*handler == NULL) {
        LOG_add("CURL_init", "CURL couldn't initialize a handler"); 
	    return;
	}
	
	//Setting CURL options.
	curl_easy_setopt(*handler, CURLOPT_USERPWD, USER_AUTH);

}

/*==============================================
FUNCTION: CURL_serialize
INPUT: an adsbMsg pointer and a char pointer to pointer
OUTPUT: void
DESCRIPTION: this function receives an adsbMsg node,
converts its fields to a json representation and 
saves the string generated into a char vector passed
by refecence.
================================================*/
void CURL_serialize(adsbMsg *msg, char **jsonMsg){

    json_object *json;
    json = json_object_new_object();

    json_object_object_add(json,"collectorKey", json_object_new_string(msg->COLLECTOR_ID));
    json_object_object_add(json,"modeSCode",json_object_new_string(msg->ICAO));
    json_object_object_add(json,"callsign",json_object_new_string(msg->callsign));

    json_object_object_add(json,"latitude",json_object_new_double(msg->Latitude));
    json_object_object_add(json,"longitude",json_object_new_double(msg->Longitude));
    json_object_object_add(json,"altitude",json_object_new_double(msg->Altitude));

    json_object_object_add(json,"verticalVelocity",json_object_new_double(msg->verticalVelocity));
    json_object_object_add(json,"horizontalVelocity",json_object_new_double(msg->horizontalVelocity));
    json_object_object_add(json,"groundTrackHeading",json_object_new_double(msg->groundTrackHeading));

    double timestamp1 = (msg->oeTimestamp[0] > msg->oeTimestamp[1])? msg->oeTimestamp[0] : msg->oeTimestamp[1];
    json_object_object_add(json,"timestamp",json_object_new_double(timestamp1));
    float timestamp2 = getCurrentTime();
    json_object_object_add(json,"timestampSent",json_object_new_double(timestamp2));

    json_object_object_add(json,"messageDataId",json_object_new_string(msg->messageID));//20
    json_object_object_add(json,"messageDataPositionEven",json_object_new_string(msg->oeMSG[0]));//52
    json_object_object_add(json,"messageDataPositionOdd",json_object_new_string(msg->oeMSG[1]));//51
    json_object_object_add(json,"messageDataVelocity", json_object_new_string(msg->messageVEL)); //48

    //This converts the json object to a string
   	*jsonMsg = (char *)json_object_to_json_string(json);
    
    printf("JSON: %s\n", *jsonMsg);
}

/*==============================================
FUNCTION: CURL_transfer
INPUT: a curl pointer and a char vector
OUTPUT: an integer
DESCRIPTION: this function receives a curl handler
and an URL and tries to send a request to that URL
passing the paremeters configured in the handler.
================================================*/
int CURL_transfer(CURL *handler, const char *url){
	
    CURLcode returnCode;

	if(handler == NULL){
        //The handler is invalid
		return -1;
	}

     //It sets the URL
     curl_easy_setopt(handler, CURLOPT_URL, url);
     //It sets the agent
     curl_easy_setopt(handler, CURLOPT_USERAGENT, "libcurl-agent/1.0");
     //It sets the timeout in seconds
     curl_easy_setopt(handler, CURLOPT_TIMEOUT, 5);
     //It sets the locations redirects to any location
     curl_easy_setopt(handler, CURLOPT_FOLLOWLOCATION, 1);
     //It sets the maximum allowed redirects
   	 curl_easy_setopt(handler, CURLOPT_MAXREDIRS, 1);

   	 //It does the transferring in a blocking way
   	 returnCode = curl_easy_perform(handler);
		
   	 return returnCode;
}

/*==============================================
FUNCTION: CURL_put
INPUT: a curl pointer
OUTPUT: an integer
DESCRIPTION: this function receives a curl handler
and tries to send a put request to the path specified
by url and info.
================================================*/
int CURL_put(CURL *handler, char *url, char *info){
	
    CURLcode returnCode;

    //It concatenates the URL and the info
	char path[strlen(url) + strlen(info)];
	sprintf(path,"%s%s",url, info);

	path[strlen(path)] = '\0';
	printf("URL: %s\n", path);

	//Setting header configuration
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");

	//Setting CURL options
	curl_easy_setopt(handler, CURLOPT_CUSTOMREQUEST, "PUT");
	curl_easy_setopt(handler, CURLOPT_HTTPHEADER, headers);
 
    //It makes the request
	returnCode = CURL_transfer(handler, path);

	if (returnCode != CURLE_OK) {
        LOG_add("CURL_put", (char *)curl_easy_strerror(returnCode));
	    fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s", path, curl_easy_strerror(returnCode));
    }

    //It clears the headers and handler
    curl_slist_free_all(headers);
	curl_easy_cleanup(handler);

    return returnCode;
}

/*==============================================
FUNCTION: CURL_post
INPUT: a curl pointer, a char vector and an adsbMsg pointer
OUTPUT: an integer
DESCRIPTION: this function receives a curl handler,
an URL and an adsbMsg node and tries to send the
information contained in the node to the url, through
a post request.
================================================*/
int CURL_post(CURL *handler, char *url, adsbMsg *message){
	
    CURLcode returnCode;
	char *jsonMsg;

    //It transforms an adsbMsg node in a json string
	CURL_serialize(message, &jsonMsg);

    //It adapts the message to the format the server expects, that is, a list
	char listFormat[strlen(jsonMsg) + 3];			
	sprintf(listFormat, "[%s]", jsonMsg);
	listFormat[strlen(listFormat)] = '\0';

	//Setting header configuration
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");

    //Setting CURL options
	curl_easy_setopt(handler, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(handler, CURLOPT_POSTFIELDS, listFormat);
	curl_easy_setopt(handler, CURLOPT_HTTPHEADER, headers);

    //It makes the request
	returnCode = CURL_transfer(handler, url);

    if (returnCode != CURLE_OK /*|| pData->size < 1*/) {
        LOG_add("CURL_post", (char *)curl_easy_strerror(returnCode));
	    fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s", url, curl_easy_strerror(returnCode));
    }

    //Cleaing the handler features.
    curl_easy_cleanup(handler);
    //Cleaning the headers.
    curl_slist_free_all(headers);
	//Freeing json string here, instead of using "json_object_put(json)" in serializer, because we were missing the jsonMsg information.
	free(jsonMsg);

    printf("\nPost done!%d\n", returnCode);

    return returnCode;
}

/*==============================================
FUNCTION: NET_putMsg
INPUT: void
OUTPUT: void
DESCRIPTION: this function sends a put request to
the path specified by PUT_URL and COLLECTOR_URL.
================================================*/
void* NET_putMsg(){             
    CURL *handler = NULL;  

    CURL_init(&handler);
    if(handler == NULL){
        LOG_add("NET_putMsg", "CURL couldn't initialize a handler");
        printf("NET: CURL couldn't initialize a handler\n");

    }else{
        printf("NET: handler initialized\n");
        CURL_put(handler, PUT_URL, COLLECTOR_URL);   
    } 
}

/*==============================================
FUNCTION: NET_dataUpload
INPUT: void
OUTPUT: void
DESCRIPTION: this function sends a put to the server
every 1 minute.
================================================*/
void* NET_dataUpload(){			
	int timeCount = 0;

	while(1){
		if((timeCount % PUT_WAIT) == 0){
            printf("Sending hello to the server... %d\n", timeCount);
			
            NET_putMsg();
            timeCount = 0;
		}

        usleep(1000);
        timeCount += 1;
	}
}

/*==============================================
FUNCTION: NET_postMsg
INPUT: an adsbMsg node
OUTPUT: void
DESCRIPTION: this function receives an adsbMsg node
and tries to send its information to the path
specified by POST_URL.
================================================*/
void* NET_postMsg(void *node){             
    adsbMsg *msg = (adsbMsg*) node;
    CURL *handler = NULL;  

    CURL_init(&handler);
    if(handler == NULL){
        LOG_add("NET_postMsg", "CURL couldn't initialize a handler");
        printf("NET: CURL couldn't initialize a handler\n");

    }else{
        printf("NET: handler initialized\n");
        CURL_post(handler, POST_URL, msg);   
    }

    pthread_exit(NULL); 
}