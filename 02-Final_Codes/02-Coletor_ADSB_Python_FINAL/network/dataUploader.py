import logging as log

from multiprocessing import RLock
import os

from time import sleep

from threading import Thread

import requests

from receptor.rootConfig import LOGIN, PASSWORD, COLLECTOR_ID, LOCAL_DATA_ENABLED, LOG_DIR

log.basicConfig(level=log.DEBUG, filemode="w", filename=os.path.join(LOG_DIR, "receptor.log"))


class DataUploader(Thread):
    __lockBuffer = RLock()
    __adsbInfoBuffer = []
    __serverHost = None
    __running = False

    sendHelloInterval = None
    sendADSBInfoInterval = None
    bufferSizeLimit = None


    def __init__(self, serverHost="www.radarlivre.com", sendHelloInterval=120, sendADSBInfoInterval=500, bufferSizeLimit=2048):
        Thread.__init__(self)
        self.__serverHost = serverHost
        self.sendHelloInterval = sendHelloInterval
        self.sendADSBInfoInterval = sendADSBInfoInterval
        self.bufferSizeLimit = bufferSizeLimit


    def run(self):
        self.onStart()
        self.__running = True

        timeCount = 0
        while self.__running:
            self.__sendADSBInfoToServer()

            if timeCount % self.sendHelloInterval == 0:
                self.__sendHelloToServer()
                timeCount = 0

            #if timeCount % self.sendADSBInfoInterval == 0:
                #self.__sendADSBInfoToServer()

            sleep(.450)
            timeCount += 1

        self.onStop()


    def addADSBInfo(self, adsbInfo):
        self.__lockBuffer.acquire()
        self.__adsbInfoBuffer.append(adsbInfo)
        if len(self.__adsbInfoBuffer) > self.bufferSizeLimit:
            del self.__adsbInfoBuffer[0]
        self.__lockBuffer.release()

        # log.info("DataUploader: Adding adsbInfo: %d" % len(self.__adsbInfoBuffer))
        

    def __sendHelloToServer(self):
        log.info("DataUploader: Sending hello to server...")
        try:
            response = requests.put("http://%s/api/collector/%s/" % (self.__serverHost, COLLECTOR_ID),
                json={"id": COLLECTOR_ID}, auth=(LOGIN, PASSWORD))

            if response.status_code >= 400:
                log.warning("DataUploader: %d: %s" % (response.status_code, str(response.json())))
        except Exception as err:
            log.error("DataUploader: %s" % str(err))


    def __sendADSBInfoToServer(self):
        self.__lockBuffer.acquire()

        if self.__adsbInfoBuffer:
            #log.info("DataUploader: Sending data to server: %d" % len(self.__adsbInfoBuffer))

            json = []
            for info in self.__adsbInfoBuffer:
                json.append(info.serialize())

            try:
                response = requests.post("http://%s/api/adsb_info/" % str(self.__serverHost),
                    json=json, auth=(LOGIN, PASSWORD),
                    timeout=30) 
                
                if response.status_code >= 400:
                    log.warning("DataUploader: %d: %s" % (response.status_code, str(response.json())))
                else:
                    del self.__adsbInfoBuffer[:]

            except Exception as err:
                log.error("DataUploader: %s" % str(err))

            #log.info("DataUploader: Sending data to server: Done!")

        self.__lockBuffer.release()

    def stop(self):
        log.info("DataUploader: Stoping...")
        self.__running = False


    def onStop(self):
        log.info("DataUploader: Stoped!")

    def onStart(self):
        log.info("DataUploader: Started!")
