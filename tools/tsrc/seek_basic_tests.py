#
# TSB basic seek tests
#
import unittest
from streamfssocketclient import SocketStreamListener, enableTsSocketDump
from test_vectors import STREAMFS_HOST, STREAMFS_PORT, STREAMFS_SEEK0_PATH
from tsanalyzer import ErrorTracker
from channelselector import ChannelSelector, SelectorType, CHANNEL_LIST, ChannelId
from keymaps import send_key_press
from seek_api import SeekControl

import time

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

    def simple_seek(self, channel):
        """
            Helper function for simple TSB testing:
            - Switch to a channel and fill TSB with content in 10 seconds
            - seek back 10 seconds
            - wait 3 seconds and clear error counter
            - wait 20 seconds
        """
        self.assertEqual(self.selector.select_channel(channel, SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        time.sleep(10)
        self.seek.setSeek(10)
        time.sleep(3)
        self.error_counter.clear()
        time.sleep(20)
        print("Errors detected:" + str(self.error_counter.error_count))
        self.assertEqual(len(self.error_counter.error_count), 0)

    def pause_resume(self, channel):
        """
            Helper function for simple pause/resume testing:
            - Switch to a channel, wait 1 second and clear error counter
            - pause
            - wait 3 seconds
            - resume
            - wait 20 seconds
        """
        self.assertEqual(self.selector.select_channel(channel, SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        time.sleep(1)
        self.error_counter.clear()
        send_key_press("pause")
        time.sleep(3)
        send_key_press("pause")
        time.sleep(20)
        print("Errors detected:" + str(self.error_counter.error_count))
        self.assertEqual(len(self.error_counter.error_count), 0)

    def pattern_seek(self, channel, pattern, fill_time: int, sample_time : int):
        """
            Helper function for changing seek position based on the pattern
            defined in the pattern-argument, which is an array of float values
            between [0,1] defining the relative TSB position relative
            to the current TSB size. Following is done:
            - Switch to a channel
            - Sleep according to the fill_time to fill up the TSB, then clear the error counter
            - Iterate over pattern-array and for each entry:
              * calculate and set the seek position relative to the current TSB size
              * wait 2 seconds and clear error counter
              * wait sample_time and evaluate the error counts
        """
        self.assertEqual(self.selector.select_channel(channel, SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        time.sleep(fill_time)
        self.error_counter.clear()
        for pos in pattern:
            position = int(pos * self.seek.getTsbSize())
            self.seek.setSeek(position)
            time.sleep(2)
            self.error_counter.clear()
            time.sleep(sample_time)
            print("Errors detected:" + str(self.error_counter.error_count))
            self.assertEqual(len(self.error_counter.error_count), 0)

    def test_simple_seek_clear(self):
        print("Executing test_simple_seek_clear")
        self.simple_seek(CHANNEL_LIST[ChannelId.CHAN_DR1]['uri'])
        pass
    
    def test_simple_seek_encrypted(self):
        print("Executing test_simple_seek_encrypted")
        self.simple_seek(CHANNEL_LIST[ChannelId.CHAN_TV_2]['uri'])
        pass

    def test_pause_resume_clear(self):
        print("Executing test_pause_resume_clear")
        self.pause_resume(CHANNEL_LIST[ChannelId.CHAN_DR_RAMASJANG]['uri'])
        print("Errors detected:" + str(self.error_counter.error_count))
        self.assertEqual(len(self.error_counter.error_count), 0)
        pass

    def test_pause_resume_encrypted(self):
        print("Executing test_pause_resume_encrypted")
        self.pause_resume(CHANNEL_LIST[ChannelId.CHAN_TV3]['uri'])
        print("Errors detected:" + str(self.error_counter.error_count))
        self.assertEqual(len(self.error_counter.error_count), 0)
        pass

if __name__ == """__main__""":
    unittest.main()