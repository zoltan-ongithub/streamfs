import socket
from urllib.parse import urlparse
from graphtools import ContErroListener
from sysloganalyzer import SysLogAnalyzer

from test_vectors import STREAMFS_HOST, STREAMFS_PORT, STREAMFS_TS_ENABLE_PATH
import threading

from tslistener import TSListener
from tssrc import TSSrc

from keymaps import set_program

def enableTsSocketDump():
    ## Open fsnode for enabling TS dumping over socket connection
    f = open(STREAMFS_TS_ENABLE_PATH, "w")
    if (not f):
        f.close()
    else:
        ## Enable TS dump
        f.write("1")
        f.close()
        ## Change channel to initiate the enabling
        set_program(1)

class SocketStreamListener(TSSrc, threading.Thread):

    """
    Socket stream receiver
    """
    listeners = set()
    stop_event = threading.Event()
    uri = ""
    stream_file = None

    socket = None

    def __stop_thread(self):
        """
        Stop the running thread
        """
        self.stop_event.set()

    def __open_stream(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as self.socket:
            self.socket.connect((self.parsed_uri.hostname, self.parsed_uri.port))
            while (not self.stop_event.is_set()):
                try:
                    data = self.socket.recv(42112)
                except ConnectionResetError:
                    # Connection closed
                    return

                for l in self.listeners:
                    l.new_ts_packet(data)

    def run(self):
        self.__open_stream()

    def register_listener(self, listener : TSListener):
        self.listeners.add(listener)

    def open(self, uri: str) -> str:
        """
        Open stream
        """
        ret = super().open(uri)
        self.uri = uri
        self.parsed_uri = urlparse(uri)
        self.stop_event.clear()
        self.close()
        self.start()

    def __del__(self):
        self.close()

    def close(self):

        if (self.socket != None):
            self.__stop_thread()
            try:
                self.socket.shutdown(socket.SHUT_RDWR)
            except Exception:
                pass

        if (self.is_alive()):
            self.join()

if __name__ == "__main__":
    tsFileListener = SocketStreamListener()
    from tsanalyzer import TSAnalyzer, ErrorListener

    import time
    from multiprocessing import  Lock
    enableTsSocketDump()

    ## Continous error monitoring
    error_listener = ContErroListener()
    error_listener.start()

    ## Syslog analyzer
    syslog_an = SysLogAnalyzer(error_listener)

    ## TS analyzer
    tsAnalyzer = TSAnalyzer(error_listener)

    tsFileListener.register_listener(tsAnalyzer)
    tsFileListener.open("telnet://" + STREAMFS_HOST + ":" +str(STREAMFS_PORT))

    input("Press Enter to continue...")

    tsAnalyzer.close()
    tsFileListener.close()
