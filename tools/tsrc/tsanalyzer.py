
# Transport stream analyzer
# Author: Zoltan Kuscsik <zoltan@zkres.com>

from abc import abstractmethod
import subprocess

from tslistener import TSListener
import threading
import signal
from multiprocessing import  Lock
import os
from enum import Enum


DVBSNOOP_SEPARATOR="=========================================================="

class TSErrorCode(Enum):
    '''
    Transport stream error codes
    '''
    CONTINUITY_ERROR = 1

class ErrorListener():
    '''
    Error listner class
    '''
    error_list = {
        'continuity error':  TSErrorCode.CONTINUITY_ERROR,
    }

    @abstractmethod
    def on_error_detected(self, error: TSErrorCode, packet : str):
        pass

class TSAnalyzer(TSListener, threading.Thread):
    stop_event = threading.Event()
    mutex = Lock()

    def __init__(self, listener : ErrorListener) -> None:
        '''
        @param listener - error Listener
        '''
        super().__init__()
        self.listener = listener
        self.process = subprocess.Popen(['dvbsnoop', '-s', 'ts', '-nph', '0x100',  '-if', '-'],
                     stdout = subprocess.PIPE, 
                     stderr = subprocess.STDOUT,
                     stdin = subprocess.PIPE)  
        
        self.start()

    def close(self):
        '''
        Close listener
        '''
        self.stop_event.set()
        self.process.terminate()
        self.process.kill()

    def run(self):
        running = self.process.poll() is None 
        packet = ""

        print_next_packet = False
        next_packet_error = None
        while ( not self.stop_event.is_set()):

            with self.mutex:
                for line in iter(self.process.stdout.readline, b''):
                    data = line.decode()

                    if ( DVBSNOOP_SEPARATOR in data):
                        if (print_next_packet):
                            print(packet)
                            self.listener.on_error_detected(next_packet_error, packet)
                            print_next_packet = False
                            next_packet_error = None
                        packet = ""

                    # Match errors
                    for k in self.listener.error_list.keys():
                        if (k in data):
                            print_next_packet = True
                            next_packet_error = self.listener.error_list[k]
                    
                    packet += data

                    if (self.stop_event.is_set()):
                        return
        
    def new_ts_packet(self, data : bytes):
        '''
        New ts packet received
        '''
        try:
            if (self.process.poll() is None):
                self.process.stdin.write(data)
        except Exception:
            pass