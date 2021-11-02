
# Syslog analyzer
# Author: Zoltan Kuscsik <zoltan@zkres.com>

from abc import abstractmethod
import subprocess
from errorlistener import DVBSNOOP_SEPARATOR, ErrorListener

from tslistener import TSListener
import threading
import signal
from multiprocessing import  Lock
import os


class SysLogAnalyzer(threading.Thread):
    stop_event = threading.Event()
    mutex = Lock()

    def __init__(self, listener : ErrorListener) -> None:
        '''
        @param listener - error Listener
        '''
        super().__init__()
        self.listener = listener
        self.process = subprocess.Popen(['journalctl', '-f'],
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
                    # Match errors
                    for k in self.listener.sylog_error_list.keys():
                        if (k in data):
                            self.listener.on_error_detected(self.listener.sylog_error_list[k], data)

                    if (self.stop_event.is_set()):
                        return