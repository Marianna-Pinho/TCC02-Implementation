import logging as log
import subprocess

log.basicConfig(level=log.DEBUG)

import socket
import sys

from threading import Thread
from time import sleep


socket.setdefaulttimeout(.5)

class AsyncTask(Thread):

    running = False
    __doInBackground = None
    __args = None

    def __init__(self, doInBackground, args=None):
        Thread.__init__(self)
        self.__doInBackground = doInBackground
        self.__args = args

    def run(self):
        self.running = True
        if self.__args:
            self.__doInBackground(self.__args)
        else:
            self.__doInBackground()

        self.running = False


class AsyncServerSocket(Thread):
    __socket = None
    __host = None
    __port = None
    __listening = False
    __clients = {}

    def __init__(self, host="127.0.0.1", port=7685):
        Thread.__init__(self)
        self.__host = host
        self.__port = port

    def __setListening(self, listening):
        self.__listening = listening

    def isListening(self):
        return self.__listening

    def run(self):
        if self.__listening:
            log.error("Socket Server: Server is already listening!")
            return

        try:
            subprocess.call(["fuser", "-k", "%d/tcp" % self.__port])
            sleep(.1)
            self.__socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.__socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.__socket.bind((self.__host, self.__port))
            self.__socket.listen(1)

        except Exception as err:
            log.error("Socket Server: %s" % str(err))
            self.onStoped()
            return

        self.onStarted()

        self.__setListening(True)

        while self.__listening:
            try:
                task = AsyncTask(self.__handleConnection, self.__socket.accept())
                task.start()

            except KeyboardInterrupt as err:
                log.info("Socket Server: Keyboard interrupted by user!")
                self.stop()

            except Exception as err:
                pass

        self.__socket.close()

        self.onStoped()

    def __handleConnection(self, connetionAndClientAddress):
        self.__clients[connetionAndClientAddress[1]] = connetionAndClientAddress[0]
        conn = connetionAndClientAddress[0]
        self.onClienteConnected(connetionAndClientAddress[1])

        try:
            while self.__listening:
                try:
                    request = conn.recv(2048)
                    if not request:
                        break
                    else:
                        response = self.onClientMessage(request)
                        if response:
                            conn.send(response)
                except socket.timeout:
                    pass

                sleep(.001)

        except KeyboardInterrupt as err:
            log.error("Socket Server: Handling connection: Keyboard interrupted by user")
            self.stop()

        except Exception as err:
            log.error("Socket Server: Handling connection: %s" % str(err))

        finally:
            try:
                conn.send("")
            except:
                pass

            conn.close()
            del self.__clients[connetionAndClientAddress[1]]
            self.onClientDisconnected(connetionAndClientAddress[1])


    def stop(self):
        if self.__listening:
            self.__setListening(False)

    def getConnectionCount(self):
        return len(self.__clients)

    def sendBroadcast(self, msg):
        for a, c in self.__clients.iteritems():
            c.send(msg)

    def onClientMessage(self, msg):
        log.info("Server Socket: message from client: %s" % msg)
        return ""

    def onStarted(self):
        log.info("Socket Server: Server listening: waiting for client...")

    def onStoped(self):
        log.info("Socket Server: Server stoped!")

    def onClienteConnected(self, clientAddress):
        log.info("Socket Server: Connecting to: %s" % str(clientAddress))

    def onClientDisconnected(self, clientAddress):
        log.info("Socket Server: Disconnecting from: %s" % str(clientAddress))


class ClientSocket():
    __socket = None
    __host = None
    __port = None
    __running = False
    __autoReconnect = False

    def __init__(self, host="127.0.0.1", port=7685, autoReconnect=False):
        self.__host = host
        self.__port = port
        self.__autoReconnect=autoReconnect

    def __setRunning(self, running):
        self.__running = running

    def connect(self):
        self.__setRunning(True)
        while self.__running:
            try:
                self.__socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.__socket.connect((self.__host, self.__port))
                task = AsyncTask(self.__handleConnection)
                task.start()
                break
            except Exception as err:
                log.error("Client Socket: %s" % str(err))

            if not self.__autoReconnect:
                break
            else:
                log.info("Client Socket: reconnecting...")
                sleep(3)

    def __handleConnection(self):
        self.onConnected()
        try:
            while(self.__running):
                try:
                    request = self.__socket.recv(2048)
                    if not request:
                        break
                    else:
                        response = self.onServerMessage(request)
                        if response:
                            self.__socket.send(response)
                except socket.timeout:
                    pass

                sleep(0.001)
        except Exception as err:
            log.error("Client Socket: %s" % str(err))
        finally:
            self.__socket.close()
            self.onDisconnected()

    def sendMessage(self, msg):
        if self.__socket and self.__running:
            self.__socket.send(msg)

    def disconnect(self):
        self.__setRunning(False)
        self.__socket.close()

    def onServerMessage(self, msg):
        log.info("Client Socket: message from server: %s" % msg)
        return ""

    def onConnected(self):
        log.info("Client Socket: connected with server!")

    def onDisconnected(self):
        log.info("Client Socket: disconnected from server!")
