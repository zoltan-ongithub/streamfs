#
# Channel switch tests
#
import unittest
from streamfssocketclient import SocketStreamListener, enableTsSocketDump
from test_vectors import CHANNEL_DR1, CHANNEL_TV2_NEWS, STREAMFS_HOST, STREAMFS_PORT, TS_CAPTURE_TIME_S, channel_to_uri
from tsanalyzer import ErrorTracker
from channelselector import ChannelSelector, SelectorType, CHANNEL_LIST, ChannelId
from keymaps import send_key_press

import time

class ChannelSwitchTests(unittest.TestCase):
    error_counter = ErrorTracker()
    streamfsListener = SocketStreamListener()
    selector = ChannelSelector()
    

    @classmethod
    def setUpClass(self):
        enableTsSocketDump()
        self.streamfsListener.open("telnet://" + STREAMFS_HOST + ":" +str(STREAMFS_PORT))
        self.error_counter.clear()
        self.streamfsListener.register_listener(self.error_counter.get_error_listener())

    @classmethod
    def tearDownClass(self):
        print("Error list: " + str(self.error_counter.error_count))
        self.error_counter.close()
        self.streamfsListener.close()

    def switch_to_program(self, program : int):
        """
           Set channel
        """
        self.assertEqual(self.selector.set_program(program), True)

    def test_simple_switch_channel(self):
        """
        Simple channel switch
            - select channel 1
            - press program up
              Verify that there are not continuity, video and audio issues for 20 seconds
        """
        self.switch_to_program(1)
        self.assertEqual(self.selector.press_program_up(), True)
        time.sleep(2)
        self.error_counter.clear()
        time.sleep(20)
        self.assertEqual(len(self.error_counter.error_count), 0,  "Errors: " + str(self.error_counter.error_count))


    def test_simple_switch_channel_10x(self):
        """
        Error check after fast channel change

            - select channel 1
            - press program up 10x with 150ms delay
            - wait 1 second
            - verify that there are no video/audio/continuity errors for 20 seconds
        
        """
        self.switch_to_program(1)
        for i in range(0,10):
            time.sleep(0.150) # Fastes remote control switches
            self.assertEqual(self.selector.press_program_up(), True)
       
        time.sleep(5)
        self.error_counter.clear()
        time.sleep(20)
        self.assertEqual(len(self.error_counter.error_count), 0, "Errors: " + str(self.error_counter.error_count))


if __name__ == """__main__""":
    unittest.main()