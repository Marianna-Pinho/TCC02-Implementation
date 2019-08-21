#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
//#include <curl/curl.h>
#include </usr/include/json-c/json.h>
#include "listFunctions.h"
#include "getTime.h"
#include "userConfigs.h"
#include "curlFunctions.h"


void serializer(adsbMsg *msg, char **jsonMsg){
    json_object *json;
    json = json_object_new_object();

    json_object_object_add(json,"collectorKey", json_object_new_string(msg->COLLECTOR_ID));
    json_object_object_add(json,"modeSCode",json_object_new_string(msg->ICAO));
    json_object_object_add(json,"callsign",json_object_new_string(msg->callsign));

    json_object_object_add(json,"latitude",json_object_new_double(msg->Latitude));
    json_object_object_add(json,"longitude",json_object_new_double(msg->Longitude));
    json_object_object_add(json,"altitude",json_object_new_double(msg->Altitude));

    json_object_object_add(json,"verticalVelocity",json_object_new_double(msg->verticalVelocidade));
    json_object_object_add(json,"horizontalVelocity",json_object_new_double(msg->horizontalVelocidade));
    json_object_object_add(json,"groundTrackHeading",json_object_new_double(msg->groundTrackHeading));

    double timestamp1 = (msg->oeTimestamp[0] > msg->oeTimestamp[1])? msg->oeTimestamp[0] : msg->oeTimestamp[1];
    json_object_object_add(json,"timestamp",json_object_new_double(timestamp1));
    float timestamp2 = getCurrentTime();
    json_object_object_add(json,"timestampSent",json_object_new_double(timestamp2));

    json_object_object_add(json,"messageDataId",json_object_new_string(msg->ICAO));//20
    json_object_object_add(json,"messageDataPositionEven",json_object_new_string(msg->oeMSG[0]));//52
    json_object_object_add(json,"messageDataPositionOdd",json_object_new_string(msg->oeMSG[1]));//51
    json_object_object_add(json,"messageDataVelocity", json_object_new_string(msg->mensagemVEL)); //48

   	*jsonMsg = (char *)json_object_to_json_string(json);
    printf("JSON: %s\n", *jsonMsg);
    //Cleaning json object.
    //json_object_put(json);
}

void curlInitialization(CURL **handle){

	*handle = curl_easy_init();				//Initializing handle.
    if (*handle == NULL) {
    	fprintf(stderr, "ERROR: Failed to create curl handle.");
	    //return NULL;
	    return;
	}
	
	//Setting CURL options.
	curl_easy_setopt(*handle, CURLOPT_USERPWD, authentication);

	//return handle;
}

CURLcode transfer(CURL *handle, const char *url){
	CURLcode returnCode;

	if(handle == NULL){
		printf("Handle NULL\n");
	}
     //Set URL.
     curl_easy_setopt(handle, CURLOPT_URL, url);
     //Set agent.
     curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
     //Set timeout.
     curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5);
     //Set locations redirects.
     curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
     //Set maximum allowed redirects.
   	 curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 1);

   	 //Do the transfer
   	 returnCode = curl_easy_perform(handle);
		
   	 return returnCode;
}


int postRequest(CURL *handle, char *url, adsbMsg *message){
	CURLcode returnCode;
	char *jsonMsg;

	serializer(message, &jsonMsg);
	char listFormat[strlen(jsonMsg) + 3];			
	sprintf(listFormat, "[%s]", jsonMsg);		//Como o servidor espera receber uma lista, colocamos os colchetes que a representam.
	listFormat[strlen(listFormat)] = '\0';

	//Setting headers variable
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, listFormat);
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

	returnCode = transfer(handle, url);

   	//Cleaing the handle features.
    curl_easy_cleanup(handle);
    //Cleaning the headers.
    curl_slist_free_all(headers);
	//Freeing json string here, instead of using "json_object_put(json)" in serializer, because we were missing the jsonMsg information.
	free(jsonMsg);

    if (returnCode != CURLE_OK /*|| pData->size < 1*/) {
	    fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s", url, curl_easy_strerror(returnCode));
    	return 2;
    }
    printf("\nPost done!%d\n", returnCode);
}

int putRequest(CURL *handle, char *url, char*message){
	CURLcode returnCode;

	char aux[strlen(url) + strlen(message)];
	sprintf(aux,"%s%s",url,message);
	aux[strlen(aux)] = '\0';
	printf("URL: %s\n", aux);

	//Setting headers variable
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");

	//Setting CURL options.
	curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
 

	returnCode = transfer(handle, aux);

	//Cleaning the headers.
    curl_slist_free_all(headers);
	curl_easy_cleanup(handle);

	if (returnCode != CURLE_OK /*|| pData->size < 1*/) {
	    fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s", url, curl_easy_strerror(returnCode));
    	return 2;
    }
}

