#
# TSB seek tests
#
import unittest
from streamfssocketclient import SocketStreamListener
from test_vectors import CHANNEL_DR1, CHANNEL_TV2_NEWS, STREAMFS_HOST, STREAMFS_PORT, TS_CAPTURE_TIME_S, channel_to_uri
from tsanalyzer import ErrorTracker
from channelselector import ChannelSelector, SelectorType, CHANNEL_LIST, ChannelId
from keymaps import send_key_press

import time

class SeekControl:

    def setSeek(self, position : int):
        f = open("/mnt/streamfs/fcc/seek0", "w")
        if (not f):
            f.close()
            return False

        f.write(str(position))
        f.close()
        return True

class SeekContCheck(unittest.TestCase):
    error_counter = ErrorTracker()
    streamfsListener = SocketStreamListener()
    selector = ChannelSelector()
    seek = SeekControl()

    @classmethod
    def setUpClass(cls):
        cls.selector.select_channel(CHANNEL_LIST[ChannelId.CHAN_BBC_WORLD_NEWS]['uri'], SelectorType.STREAMFS_CHANNEL_SELECTION)
        time.sleep(1)
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

    def test_simple_seek_clear(self):
        self.simple_seek(CHANNEL_LIST[ChannelId.CHAN_DR1]['uri'])
        print("Errors detected:" + str(self.error_counter.error_count))
        self.assertEqual(len(self.error_counter.error_count), 0)
        pass

    def test_simple_seek_encrypted(self):
        self.simple_seek(CHANNEL_LIST[ChannelId.CHAN_TV_2]['uri'])
        print("Errors detected:" + str(self.error_counter.error_count))
        self.assertEqual(len(self.error_counter.error_count), 0)
        pass

    def test_pause_resume_clear(self):
        self.pause_resume(CHANNEL_LIST[ChannelId.CHAN_DR_RAMASJANG]['uri'])
        print("Errors detected:" + str(self.error_counter.error_count))
        self.assertEqual(len(self.error_counter.error_count), 0)
        pass

    def test_pause_resume_encrypted(self):
        self.pause_resume(CHANNEL_LIST[ChannelId.CHAN_TV3]['uri'])
        print("Errors detected:" + str(self.error_counter.error_count))
        self.assertEqual(len(self.error_counter.error_count), 0)
        pass

if __name__ == """__main__""":
    unittest.main()