
from os import close
from tsanalyzer import ErrorListener
from tslistener import TSListener
from tssrc import TSSrc
import socket
import binascii
from queue import Queue
import threading
from urllib.parse import urlparse
RECEIVER_SIZE = 10 * 188


class StreamFSStreamListener(TSSrc, threading.Thread):
    """
    RTP stream receiver
    """
    listeners = set()
    stop_event = threading.Event()
    uri = ""
    stream_file = None

    def __stop_thread(self):
        """
        Stop the running thread
        """
        print("stopping htread")
        self.stop_event.set()

    def __open_stream(self):
        self.stream_file = open(self.uri, 'rb')
        print("opened file")
        while not self.stop_event.is_set():
            data = self.stream_file.read(RECEIVER_SIZE)
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
        
        self.close()
        self.start()

    def __del__(self):
        self.close()

    def close(self):

        if (self.stream_file != None):
            self.__stop_thread()
            try:
                self.stream_file.close()
            except Exception:
                pass

        if (self.is_alive()):
            self.join()



if __name__ == "__main__":
    test = StreamFSStreamListener()
    from tsanalyzer import TSAnalyzer, TSErrorCode, ErrorListener

    class ContErroListener(ErrorListener):
        counter = 0
        def on_error_detected(self, error: TSErrorCode, packet: str):
            self.counter +=1
            print(self.counter)
            print("Got continuity error")
    error_listener = ContErroListener();

    a = TSAnalyzer(error_listener)

    test.register_listener(a)
    test.open("/mnt/streamfs/fcc/stream0.ts")
    input("Press Enter to continue...")

    a.close()
    test.close()

