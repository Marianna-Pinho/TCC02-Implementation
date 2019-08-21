#ifndef NETWORK_H
#define NETWORK_H

// typedef struct message{
// 	char nome[10];
// 	int idade;
// }adsbMsg;

#define sendHelloInterval 60000
#define sendADSBInfoInterval 500
#define bufferSizeLimit 2048
#define URL_POST "www.radarlivre.com:80/api/adsb_info/"
//#define URL_PUT "127.0.0.1:8000/api/collector/"
#define URL_PUT "www.radarlivre.com:80/api/collector/"
#define URL_COLLECTOR "5980d171-73fb-46c1-aea1-e7b5d5c1de60/"

typedef struct tosend{
	adsbMsg *no;
	int sockfd;
	struct sockaddr_in server;
}toSend;

typedef struct tosend toSend;
void *postMessage(void *packet);
void *putMessage();
void *dataUploader();

#endif // NETWORK_H