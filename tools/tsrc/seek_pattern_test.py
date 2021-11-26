#
# TSB pattern seek tests
#
import unittest
from streamfssocketclient import SocketStreamListener, enableTsSocketDump
from test_vectors import STREAMFS_HOST, STREAMFS_PORT, STREAMFS_SEEK0_PATH
from tsanalyzer import ErrorTracker
from channelselector import ChannelSelector, SelectorType, CHANNEL_LIST, ChannelId
from keymaps import send_key_press

import time

from seek_api import SeekControl

class SeekContCheck(unittest.TestCase):
    error_counter = ErrorTracker()
    streamfsListener = SocketStreamListener()
    selector = ChannelSelector()
    seek = SeekControl(STREAMFS_SEEK0_PATH)

    @classmethod
    def setUpClass(cls):
        enableTsSocketDump()
        time.sleep(3)
        cls.streamfsListener.open("telnet://" + STREAMFS_HOST + ":" +str(STREAMFS_PORT))
        cls.error_counter.clear()
        cls.streamfsListener.register_listener(cls.error_counter.get_error_listener())

    @classmethod
    def tearDownClass(cls):
        cls.error_counter.close()
        cls.streamfsListener.close()

    def pattern_seek(self, pattern, repeat: int, sleep):
        """
            Helper function for evaluating the error count when changing seek
            position based on the pattern defined in the pattern-argument, which
            must be an array of seek positions.
            The repeat argument defines the amount of times the seek pattern shall
            be repeated in a cyclic manner.
            The sleep time before changing seek position is defined by
            the sleep argument. 
        """
        for i in range(0, repeat):
            for pos in pattern:
                self.seek.setSeek(pos)
                time.sleep(0.75)
                self.error_counter.clear()
                time.sleep(sleep)
                print("Errors detected:" + str(self.error_counter.error_count))
                self.assertEqual(len(self.error_counter.error_count), 0)

    def test_toggle_search_pause_encrypted(self):
        print("Executing test_toggle_search_pause_encrypted")
        self.assertEqual(self.selector.select_channel(CHANNEL_LIST[ChannelId.CHAN_CNN]['uri'], SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        time.sleep(3)
        send_key_press("pause")
        time.sleep(10)
        self.error_counter.clear()
        self.pattern_seek([5,10], 25, 0.25)
        send_key_press("pause")
        time.sleep(3)
        pass

    def test_toggle_search_pause_clear(self):
        print("Executing test_toggle_search_pause_clear")
        self.assertEqual(self.selector.select_channel(CHANNEL_LIST[ChannelId.CHAN_DR2]['uri'], SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        time.sleep(3)
        send_key_press("pause")
        time.sleep(10)
        self.error_counter.clear()
        self.pattern_seek([5,10], 25, 0.25)
        send_key_press("pause")
        time.sleep(3)
    pass

    def test_forward_search_playing_clear(self):
        print("Executing test_forward_search_playing_clear")
        self.assertEqual(self.selector.select_channel(CHANNEL_LIST[ChannelId.CHAN_XEE]['uri'], SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        span = 60
        time.sleep(span)
        seek_values = list(range(span,0,-5))
        self.pattern_seek(seek_values, 1, 0.25)
        time.sleep(3)
    pass

    def test_forward_search_playing_encrypted(self):
        print("Executing test_forward_search_playing_encrypted")
        self.assertEqual(self.selector.select_channel(CHANNEL_LIST[ChannelId.CHAN_TV3]['uri'], SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        span = 60
        time.sleep(span)
        seek_values = list(range(span,0,-5))
        self.pattern_seek(seek_values, 1, 0.25)
        time.sleep(3)
    pass

if __name__ == """__main__""":

    unittest.main()