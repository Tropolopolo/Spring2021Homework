# -*- coding: utf-8 -*-
"""
Created on Wed Mar  3 19:46:17 2021

@author: keepc
"""

import socket
from time import sleep

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((socket.gethostname(), 7777))
#s.send(bytes("PUT mat lol"))
sleep(1)
#s.send(bytes("PUT CALEB BROTHER"))
sleep(1)
s.send(bytes("DUMP"))