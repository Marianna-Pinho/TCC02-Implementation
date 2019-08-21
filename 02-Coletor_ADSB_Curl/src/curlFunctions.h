#ifndef CURLFUNCTIONS_H
#define CURLFUNCTIONS_H

#include <curl/curl.h>

void curlInitialization(CURL **handle);
CURLcode transfer(CURL *handle, const char *url);
int postRequest(CURL *handle, char *url, adsbMsg *message);
int putRequest(CURL *handle, char *url, char*message);
void serializer(adsbMsg *msg, char **jsonMsg);

#endif // CURLFUNCTIONS_H