#!/usr/bin/env python
#
# Remote control constants and schemas
#

import json 
import urllib.request
import time
DEVICE_KEYMAP = {
    "1"         : "0x0002",
    "2"         : "0x0003",
    "3"         : "0x0004",
    "4"         : "0x0005",
    "5"         : "0x0006",
    "6"         : "0x0007",
    "7"         : "0x0008",
    "8"         : "0x0009",
    "9"         : "0x000A",
    "0"         : "0x000B",
    "back"      : "0x009E",
    "up"        : "0x0067",
    "down"      : "0x006C",
    "left"      : "0x0069",
    "right"     : "0x006A",
    "ok"        : "0x001C",
    "prog_up"   : "0x0192",
    "prog_down" : "0x0193",
    "pause"     : "0x000A4"
   };


KEY_SEND_RPC_SCHEMA ="""{
    "jsonrpc": "2.0",
    "id": 1234567890,
    "method": "RemoteControl.1.send",
    "params": {
        "device": "DevInput",
        "code": "%s"
    }
}"""


JSON_RPC_REMOTE_PATH="http://localhost/jsonrpc"

CHANNEL_SWITCH_MULTI_KEY_SLEEP_TIME_MS = 2000

def send_key_press(key : str) -> bool  :
    """
    Send key press
       Returns True on success
    """
    
    url = JSON_RPC_REMOTE_PATH
    params = (KEY_SEND_RPC_SCHEMA % (DEVICE_KEYMAP[key])).encode('utf8')
    req = urllib.request.Request(url, data=params,
                               headers={'content-type': 'application/json'})
    response = urllib.request.urlopen(req)

    return response.getcode() == 200


def set_program(program_number : int) -> bool:
    """
    Set program
       Returns True on success
    """
    stringProg = str(program_number)
    result = True
    for el in stringProg:
        val = send_key_press(el)
        if (not val):
            result = False
    time.sleep(CHANNEL_SWITCH_MULTI_KEY_SLEEP_TIME_MS / 1000.0)
    return True

if __name__=='__main__':
    import sys
    if len(sys.argv) != 2:
        print("Usage: ./%s <key> | <program_number>" % (sys.argv[0]))
        print("Valid keys: ")
        i = 0
        for p in DEVICE_KEYMAP.keys():
            print ("   %s " % p, end =' ')
            i+=1
            if (i%8 == 0):
                print("") #new line
        print("")        
        exit(0)

    keyPress = sys.argv[1]
    if (keyPress not in DEVICE_KEYMAP.keys()):
        print("Invalid key requested")
        exit(-1)

    # send_key_press(keyPress)
    set_program(1)
    set_program(12)