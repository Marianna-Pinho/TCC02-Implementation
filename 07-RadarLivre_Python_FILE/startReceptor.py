#!/usr/bin/python

import atexit

import receptor

receptor.start()

@atexit.register
def exitHandler():
    receptor.stop()
