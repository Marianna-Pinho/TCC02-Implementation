import logging as log
import os

from receptor.rootConfig import COLLECTOR_ID, LOG_DIR
from models import ADSBInfo
from time import time as systemTimestamp

from network import ClientSocket

log.basicConfig(level=log.DEBUG, filemode="w", filename=os.path.join(LOG_DIR, "receptor.log"))

def parseFloat(n):
    try:
        return float(n)
    except:
        return None

class DataInput(ClientSocket):

    __adsbBuffer = {}

    def onServerMessage(self, msg):
        try:
            entries = msg.split("\n")
            # log.info("DataInput: Server message %d" % len(entries))
            for entry in entries:
                entry = entry.split(",")
                if len(entry) >= 17:
                    info = ADSBInfo(
                        collector=COLLECTOR_ID,
                        modeSCode=entry[4],
                        callsign=entry[10],
                        latitude=parseFloat(entry[14]),
                        longitude=parseFloat(entry[15]),
                        altitude=parseFloat(entry[11]),
                        horizontalVelocity=parseFloat(entry[12]),
                        groundTrackHeading=parseFloat(entry[13]),
                        verticalVelocity=parseFloat(entry[16]),
                        messagDataId="",
                        messagDataPositionEven="",
                        messagDataPositionOdd="",
                        messagDataVelocity="",
                        timestamp=int(systemTimestamp() * 1000),
                        timestampSent=int(systemTimestamp() * 1000)
                    )

                    info = self.__addToBuffer(info)
                    if info.modeSCode and info.callsign and info.latitude and info.longitude:
                        # log.info("DataInput: A complete info has received from server!")
                        self.onADSBInfoReceived(info)
                        del self.__adsbBuffer[info.modeSCode]

        except Exception as err:
            log.info("DataInput: %s!" % err)

    def onADSBInfoReceived(self, info):
        log.info("DataInput: New adsb Info received!")

    def __addToBuffer(self, info):
        if info.modeSCode in self.__adsbBuffer:
            old = self.__adsbBuffer[info.modeSCode]
            attrs = ["collector","modeSCode", "callsign", "latitude", "longitude", "altitude", "horizontalVelocity", "groundTrackHeading", "verticalVelocity", "messagDataId", "messagDataPositionEven", "messagDataPositionOdd", "messagDataVelocity", "timestamp", "timestampSent"]
            for attr in attrs:
                try:
                    if getattr(info, attr):
                        setattr(old, attr, getattr(info, attr))
                except Exception as err:
                    log.error("DataInput: setting attribute %s: %s" % (str(attr), str(err)))

            return old
        else:
            self.__adsbBuffer[info.modeSCode] = info
            return info

