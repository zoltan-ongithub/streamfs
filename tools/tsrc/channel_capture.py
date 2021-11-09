
from os import error
import unittest
from rtpstreamlistener import RTPStreamListener
from streamfssocketclient import SocketStreamListener
from test_vectors import CHANNEL_DR1, CHANNEL_TV2_NEWS, STREAMFS_HOST, STREAMFS_PORT, TS_CAPTURE_TIME_S, channel_to_uri
from tsanalyzer import ErrorTracker

from tslistener import TSListener

import time

class StreamDump(TSListener):
    byte_count = 0

    def __init__(self, file_name : str) -> None:
        super().__init__()
        self.f = open(file_name, "wb")
        
    def new_ts_packet(self, data : bytes):
        self.byte_count += len(data)
        self.f.write(data)
        pass

    def close(self):
        print("Got bytes : " + str(self.byte_count))
        self.f.close()

class StreamContCheck(unittest.TestCase):
    """
    Stream continuity tests
    """
    def test_SocketStreamCapture(self):
        """
        Check currently tuned channel for continuity test.

        """
        error_counter = ErrorTracker()

        streamfsListener = SocketStreamListener()

        streamfsListener.open("telnet://" + STREAMFS_HOST + ":" +str(STREAMFS_PORT))

        # Register listener only after the channel switch to DR1
        streamfsListener.register_listener(error_counter.get_error_listener())

        # Capture for some time
        time.sleep(TS_CAPTURE_TIME_S)

        # Close Streamfs Stream

        error_counter.close()

        #sd.close()
        streamfsListener.close()
        print("Errors detected:" + str(error_counter.error_count))

        self.assertEqual(len(error_counter.error_count), 0)

        pass


if __name__ == """__main__""":
    unittest.main()