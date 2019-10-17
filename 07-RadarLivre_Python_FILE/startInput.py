import atexit

from network.dataInput import DataInput
from network.dataUploader import DataUploader

__DATA_INPUT = DataInput(host="127.0.0.1", port=30004, autoReconnect=True)
__UPLOADER = DataUploader(serverHost="www.radarlivre.com")
# __UPLOADER = DataUploader(serverHost="localhost:8000")

@atexit.register
def onExit():
    __DATA_INPUT.disconnect()
    __UPLOADER.stop()


def onInfo(info):
    __UPLOADER.addADSBInfo(info)


try:
    __UPLOADER.start()
    __DATA_INPUT.onADSBInfoReceived = onInfo
    __DATA_INPUT.connect()

    while True:
        pass

except KeyboardInterrupt:
    onExit()
