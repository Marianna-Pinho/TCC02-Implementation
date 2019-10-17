import re
import os
import sys
from serial import Serial

from serial.serialutil import PARITY_NONE
from threading import Thread
from time import sleep

import logging as log

from receptor.rootConfig import LOG_DIR

log.basicConfig(level=log.DEBUG, filemode="w", filename=os.path.join(LOG_DIR, "receptor.log"))

class AsyncTask(Thread): 				
    doInBackground = None

    def __init__(self, doInBackground):
        Thread.__init__(self)
        self.doInBackground = doInBackground

    def run(self):
        if self.doInBackground:
            try:
                self.doInBackground()
            except:
                pass

class MicroADSB():

    __SERIAL = None 						
    __BUFFER = ''

    online = False
    firmware = None
    id = None

    device = None 							
    baudrate = 115200 						
    databits = 8
    parity = PARITY_NONE 					
    stopbits = 1 							

    frameids = False 									
    timestamps = False
    heartbeats = False
    raw = False
    mode = 2 								

    __running = False
    asyncTask = None

    autoReconnect = False

    def __init__(self, device="receptor/teste01_Todas.txt", **kwargs):		 
        self.device = device									
        for k, v in kwargs.items():
            setattr(self, k, v)
    

    def __commandFormat(self, command): 								
        data = ''														
        if isinstance(command, str):
            for letter in command:
                data += ('00' + hex(ord(letter)).upper())[-2:] + '-'
            data = data[0: len(data) - 1]

        else:
            for c in command:
                data += ('00' + c.upper())[-2:] + '-'

            data = data[0: len(data) - 1]

        return '#' + data + "\r"


    def __microADSBASCIIToObject(self, data): 
        result = { 														
            'frameid': None,
            'timestamp': {'string': None, 'integer': None},
            'type': None,
            'downlinkformat': None,
            'data': [],
            'frame': None
        }

        result["type"] = data[0:1] 										
        data = data[1:]														

        pattern = re.compile(";#([0-9a-f]+);", flags=re.IGNORECASE)
        matches = pattern.findall(data)
        matches = None if not matches else matches[0]
        if matches:
            result["frameid"] = int(matches[0], 16)
            data = re.sub(";#([0-9a-f]+);", ";", flags=re.IGNORECASE)

        result["frame"] = re.sub("[^0-9a-f]", "", data, flags=re.IGNORECASE|re.MULTILINE)      

        if len(result["frame"]) % 2 != 0:
            return None
         
        for i in range(0, len(result["frame"]), 2):
            result["data"].append(int(result["frame"][i:i+2], 16))
        if result["type"] == '@':
            result["timestamp"]["string"] = '0x'
            result["timestamp"]["string"] += ('00' + str(result["data"][0]))[-2:]
            result["timestamp"]["string"] += ('00' + str(result["data"][1]))[-2:]
            result["timestamp"]["string"] += ('00' + str(result["data"][2]))[-2:]
            result["timestamp"]["string"] += ('00' + str(result["data"][3]))[-2:]
            result["timestamp"]["string"] += ('00' + str(result["data"][4]))[-2:]
            result["timestamp"]["string"] += ('00' + str(result["data"][5]))[-2:]
            result["timestamp"]["integer"] = int(result["timestamp"]["string"], 16)
            result["data"] = result["data"][6:]
            result["frame"] = result["frame"][12:]

            result["type"] = '*'

        result["frame"] = result["type"] + result["frame"] + ';'
        result["downlinkformat"] = (result["data"][0] & 0xf8) >> 3

        return result							

    def __mode(self, raw, heartbeats, frameNumbers, timestamps, mode):
        result = 0x00

        if raw:
            result |= 0x80
        if heartbeats:
            result |= 0x40
        if frameNumbers:
            result |= 0x20
        if timestamps:
            result |= 0x10
        if mode and (mode >= 2) and (mode <= 4):
            result |= mode & 0x0f

        result = hex(result)
        return re.sub("0x(\d)", "0x0\\1", result)

    def close(self):
        self.__running = False

        if self.__SERIAL:
            try:
                if self.__SERIAL.is_open:
                    self.__SERIAL.write([0xff])
                    self.__SERIAL.close()
                self.__SERIAL = None
                self.__BUFFER = None
                self.online = False
                self.firmware = None

            except Exception as err:
                if self.__SERIAL.is_open:
                    self.__SERIAL.close()
                return

    def open(self):
        self.asyncTask = AsyncTask(self.__open)
        self.asyncTask.start()


    def __open(self):

        if self.__running:
            self.onOpen("MicroADSB: the receptor is already running!")
            return
        else:
            self.__running = True

        while self.__running:

            # Try open connection with receptor
            # ...
            try:
                """self.__SERIAL = Serial(
                    port=self.device, baudrate=self.baudrate, bytesize=self.databits, parity=self.parity, timeout=0
                )

                # Try initialize data receive
                try:
                    self.__SERIAL.write(self.__commandFormat(['0x00']))
                except Exception as err:
                    self.__SERIAL.close()
                    self.__SERIAL = None
                    self.__BUFFER = ""
                    self.online = False
                    self.firmware = None
                    self.onOpen(err)
                """
                # Try main loop
                try:
                    self.__running = True
                    file = open(self.device, "r")
                    msgs = file.readlines()

                    for x in msgs:
                        result = None
                        
                        data = x           

                        self.__BUFFER += str(data)
                        

                        while self.__running:
                            self.__BUFFER = re.sub('^(\r\n|\n\r?|\n|\r)+', '', self.__BUFFER)

                            pattern1 = re.compile("^(\x1a(?:\x32.{14}|\x33.{21}))")
                            matches1 = pattern1.findall(self.__BUFFER)
                            matches1 = None if not matches1 else matches1[0]
                            pattern2 = re.compile("^([^\r\n]*)(\r\n?|\n\r?)")
                            matches2 = pattern2.findall(self.__BUFFER)
                            matches2 = None if not matches2 else matches2[0]
                            if matches1:
                                self.__BUFFER = self.__BUFFER[len(matches1[0]):]
                                
                                # ...

                            elif matches2:
                                self.__BUFFER = self.__BUFFER[len(matches2[0]) + len(matches2[1]):]

                                result = self.__microADSBASCIIToObject(matches2[0])
                                if result:
                                    if result["type"] == "#":
                                        if result["data"][0] == 0x00:
                                            self.firmware = result["data"][2]
                                            self.id = result["data"][3]

                                            toWrite = self.__commandFormat(
                                                            [
                                                                '0x43',
                                                                self.__mode(
                                                                    self.raw,
                                                                    self.heartbeats,
                                                                    self.frameids,
                                                                    self.timestamps,
                                                                    self.mode
                                                                )
                                                            ]
                                                        )
                                            """try:
                                                #self.__SERIAL.write(toWrite)   
                                            except Exception as err:
                                                self.__stopListening()
                                                self.__SERIAL.close()
                                                self.__SERIAL = None
                                                self.__BUFFER = ""
                                                self.online = False
                                                self.firmware = None
                                                self.onOpen(err)"""
                                        elif result["data"][0] == 0x43:
                                            if True or result["data"][1] == self.__mode(self.raw, self.heartbeats, self.frameids, self.timestamps, self.mode):
                                                self.online = True
                                                self.onOpen(None)

                                            else:
                                                self.__stopListening()
                                                self.__SERIAL.close()
                                                self.__SERIAL = None
                                                self.__BUFFER = ""
                                                self.online = False
                                                self.firmware = None
                                                err = "Failed to set ADS-B mode"
                                                self.onOpen(err)


                                    else:
                                        if result["downlinkformat"] == 23:
                                            self.onHeartBeat()

                                        else:
                                            self.onMessage(result)

                            else:
                                break

                        sleep(1) #0.001
                    file.close()
                    sys.exit()

                except Exception as err:
                    self.__SERIAL = None
                    self.__BUFFER = ''
                    self.online = False
                    self.firmware = None
                    self.onOpen(err)

            except Exception as err:
                self.__SERIAL = None
                self.__BUFFER = ""
                self.online = False
                self.firmware = None
                self.onOpen(err)


            """if not self.autoReconnect:
                self.close()
                break
            else:
                log.info("MicroADSB: Reconnecting in 3 seconds...")
                sleep(3)
                log.info("MicroADSB: Reconnecting...")
                self.onReconnecting()
            """
        self.onClose(None)

    def onOpen(self, err):
        if err:
            log.error("MicroASDB: %s" % str(err))

    def onMessage(self, msg):
        if msg:
            log.error("MicroASDB: %s" % str(msg))

    def onHeartBeat(self):
        pass

    def onError(self, err):
        if err:
            log.error("MicroASDB: %s" % str(err))

    def onClose(self, err):
        if err:
            log.error("MicroASDB: %s" % str(err))

    def onReconnecting(self):
        pass
