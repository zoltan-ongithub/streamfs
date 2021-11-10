#!/usr/bin/env python
#
# Remote control constants and schemas
#

import json 
import urllib.request
import time
DEVICE_KEYMAP = {
    "1"     :  "0x0021",
    "2"     :  "0x0022",
    "3"     :  "0x0023",
    "4"     :  "0x0024",
    "5"     :  "0x0025",
    "6"     :  "0x0026",
    "7"     :  "0x0027",
    "8"     :  "0x0028",
    "9"     :  "0x0029",
    "0"     :  "0x0020",
    "exit"  :  "0x0009",
    "a"     :  "0x8004",
    "b"     :  "0x8005",
    "c"     :  "0x8006",
    "d"     :  "0x8007",
    "e"     :  "0x8008",
    "f"     :  "0x8009",
    "g"     :  "0x800A",
    "h"     :  "0x800B",
    "i"     :  "0x800C",
    "back"  :  "0x0032",
    "j"     :  "0x800D",
    "k"     :  "0x800E",
    "l"     :  "0x800F",
    "m"     :  "0x8010",
    "n"     :  "0x8011",
    "o"     :  "0x8012",
    "p"     :  "0x8013",
    "q"     :  "0x8014",
    "r"     :  "0x8015",
    "up"    :  "0x0001",
    "ok"    :  "0x002B",
    "s"     :  "0x8016",
    "t"     :  "0x8017",
    "u"     :  "0x8018",
    "v"     :  "0x8019",
    "w"     :  "0x801A",
    "x"     :  "0x801B",
    "y"     :  "0x801C",
    "z"     :  "0x801D",
    "left"  :  "0x0003",
    "down"  :  "0x0002",
    "right" :  "0x0004",
    "prog_up" : "0x8014",
    "prog_down" : "0x8004"
   };


KEY_SEND_RPC_SCHEMA ="""{
    "jsonrpc": "2.0",
    "id": 1234567890,
    "method": "RemoteControl.1.send",
    "params": {
        "device": "Web",
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