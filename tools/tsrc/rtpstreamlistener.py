
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


class RTPStreamListener(TSSrc, threading.Thread):
    """
    RTP stream receiver
    """
    listeners = set()
    stop_event = threading.Event()
    uri = ""
    active_socket = None

    def __stop_thread(self):
        """
        Stop the running thread
        """
        self.stop_event.set()

    def __open_stream(self):
        multicast_group = self.parsed_uri.hostname
        multicast_port = self.parsed_uri.port

        net_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        
        try:
            net_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        except AttributeError:
            pass
        
        net_sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 32) 
        net_sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_LOOP, 1)

        net_sock.bind((multicast_group, multicast_port))
        host = socket.gethostbyname("0.0.0.0")
        net_sock.setsockopt(socket.SOL_IP, socket.IP_MULTICAST_IF, socket.inet_aton(host))
        net_sock.setsockopt(socket.SOL_IP, socket.IP_ADD_MEMBERSHIP, 
                   socket.inet_aton(multicast_group) + socket.inet_aton(host))
        net_sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_LOOP, 0)
        
        self.active_socket = net_sock

        while not self.stop_event.is_set():
            try:
                data, addr = net_sock.recvfrom(RECEIVER_SIZE)
            except  Exception:
                print("Exception : " + e)

            for l in self.listeners:
                l.new_ts_packet(data)

        self.active_socket = None

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
        self.parsed_uri = urlparse(uri)
        self.stop_event.clear()
        self.start()

    def __del__(self):
        self.close()

    def close(self):
        self.__stop_thread()

        if (self.active_socket != None):
            try:
                self.active_socket.shutdown(socket.SHUT_RDWR)
            except Exception:
                pass
        if (self.is_alive()):
            self.join()




if __name__ == "__main__":
    test = RTPStreamListener()
    from tsanalyzer import TSAnalyzer, TSErrorCode, ErrorListener

    class ContErroListener(ErrorListener):
        count = 0;
        def on_error_detected(self, error: TSErrorCode, packet: str):
            self.counter +=1
            print(self.counter)
            print("Got continuity error: " + str(error))
            print(packet)

    error_listener = ContErroListener();

    a = TSAnalyzer(error_listener)

    test.register_listener(a)
    test.open("rtp://234.80.160.50:5900")
    input("Press Enter to continue...")
    print("Exiting")

    a.close()
    test.close()

