# 
# Channel selector
#
import enum

from test_vectors import STREAMFS_CHAN_SELECT0_PATH, STREAMFS_CHANNEL_SELECT0_PATH

class SelectorType(enum.Enum):
    INPUT_INJECTION  = enum.auto(),
    STREAMFS_CHANNEL_SELECTION  = enum.auto(),

class SourceType(enum.Enum):
    MULTICAST = enum.auto
    MSTV = enum.auto


class ServiceType(enum.Enum):
    MPEG = enum.auto
    HEVC_HD = enum.auto
    MPEG4_HD = enum.auto
    RADIO  = enum.auto

class ChannelId(enum.Enum):
    CHAN_DR1 = enum.auto()
    CHAN_DR1_SYNSTOLKNING = enum.auto()
    CHAN_TV_2 = enum.auto()
    CHAN_TV_2_ZULU = enum.auto()
    CHAN_TV_2_FRI = enum.auto()
    CHAN_NATIONAL_GEOGRAPHIC = enum.auto()
    CHAN_BBC_WORLD_NEWS = enum.auto()
    CHAN_DISNEY_XD = enum.auto()
    CHAN_TV_2_NEWS = enum.auto()
    CHAN_CNN = enum.auto()
    CHAN_DR_P1 = enum.auto()
    CHAN_DR_P3 = enum.auto()
    CHAN_RADIO4 = enum.auto()
    CHAN_NOVA_FM = enum.auto()
    CHAN_THE_VOICE = enum.auto()
    CHAN_DR_P5 = enum.auto()
    CHAN_RADIO_100 = enum.auto()
    CHAN_DR_P4_NORDJYLLAND = enum.auto()
    CHAN_TV3 = enum.auto()
    CHAN_DR2 = enum.auto()
    CHAN_DR2_SYNSTOLKNING = enum.auto()
    CHAN_TV_2_DANMARK = enum.auto()
    CHAN_NORD = enum.auto()
    CHAN_ARD = enum.auto()
    CHAN_SVT2 = enum.auto()
    CHAN_DISNEY_JUNIOR = enum.auto()
    CHAN_HBO_NORDIC_TV = enum.auto()
    CHAN_C_MORE_HITS = enum.auto()
    CHAN_DANSK_FILMSKAT_TV = enum.auto()
    CHAN_TV_2_SPORT = enum.auto()
    CHAN_NORDISK_FILM_TV = enum.auto()
    CHAN_TV_2_CHARLIE = enum.auto()
    CHAN_EVENTKANALEN = enum.auto()
    CHAN_C_MORE_STARS = enum.auto()
    CHAN_C_MORE_SERIES = enum.auto()
    CHAN_TV2NOR = enum.auto()
    CHAN_TV_MID = enum.auto()
    CHAN_ZDF = enum.auto()
    CHAN_NDR = enum.auto()
    CHAN_RTL = enum.auto()
    CHAN_DK4 = enum.auto()
    CHAN_XEE = enum.auto()
    CHAN_SVT1 = enum.auto()
    CHAN_TV4 = enum.auto()
    CHAN_NRK1 = enum.auto()
    CHAN_FOLKETINGET = enum.auto()
    CHAN_PARAMOUNT_NETWORK = enum.auto()
    CHAN_DR_RAMASJANG = enum.auto()
    CHAN_INFOKANALEN = enum.auto()
    CHAN_EKSTRAKANALEN = enum.auto()
    CHAN_TRAILERKANALEN = enum.auto()
    CHAN_NATIONAL_GEOGRAPHIC_WILD = enum.auto()
    CHAN_C_MORE_FIRST = enum.auto()
    CHAN_TV_2_SPORT_X = enum.auto()
    CHAN_DISNEY_CHANNEL = enum.auto()
    CHAN_DISNEY_CHANNEL_HEVC_HD = enum.auto()
    CHAN_TV_2_ZULU_HEVC_HD = enum.auto()
    CHAN_TV_2_FRI_HEVC_HD = enum.auto()
    CHAN_TV_2_NEWS_HEVC_HD = enum.auto()
    CHAN_TV3_HEVC_HD = enum.auto()
    CHAN_DR2_HEVC_HD = enum.auto()
    CHAN_TV_2_DANMARK_HEVC_HD = enum.auto()
    CHAN_TV_2_SPORT_HEVC_HD = enum.auto()
    CHAN_TV_2_CHARLIE_HEVC_HD = enum.auto()
    CHAN_XEE_HEVC_HD = enum.auto()
    CHAN_TV_2_SPORT_X_HEVC_HD = enum.auto()

CHANNEL_LIST = { 
  ChannelId.CHAN_DR1:{"name" : "DR1", "uri" :  "234.80.160.1:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_DR1_SYNSTOLKNING:{"name" : "DR1_SYNSTOLKNING", "uri" :  "234.80.160.169:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2:{"name" : "TV_2_ZULU", "uri" :  "234.80.160.2:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_ZULU:{"name" : "TV_2_ZULU", "uri" :  "234.80.160.13:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_TV_2_FRI:{"name" : "TV_2_FRI", "uri" :  "234.80.160.27:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_NATIONAL_GEOGRAPHIC:{"name" : "NATIONAL_GEOGRAPHIC", "uri" :  "234.80.160.29:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_BBC_WORLD_NEWS:{"name" : "BBC_WORLD_NEWS", "uri" :  "234.80.160.197:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_DISNEY_XD:{"name" : "DISNEY_XD", "uri" :  "234.80.160.130:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_NEWS:{"name" : "TV_2_NEWS", "uri" :  "234.80.160.15:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_CNN:{"name" : "CNN", "uri" :  "234.80.160.207:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_DR_P1:{"name" : "DR_P1", "uri" :  "234.80.161.1:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.RADIO},

    ChannelId.CHAN_DR_P3:{"name" : "DR_P3", "uri" :  "234.80.161.2:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.RADIO},

    ChannelId.CHAN_RADIO4:{"name" : "ServiceType.RADIO4", "uri" :  "234.80.161.15:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.RADIO},

    ChannelId.CHAN_NOVA_FM:{"name" : "NOVA_FM", "uri" :  "234.80.161.16:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.RADIO},

    ChannelId.CHAN_THE_VOICE:{"name" : "THE_VOICE", "uri" :  "234.80.161.18:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.RADIO},

    ChannelId.CHAN_DR_P5:{"name" : "DR_P5", "uri" :  "234.80.161.14:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.RADIO},

    ChannelId.CHAN_RADIO_100:{"name" : "ServiceType.RADIO_100", "uri" :  "234.80.161.17:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.RADIO},

    ChannelId.CHAN_DR_P4_NORDJYLLAND:{"name" : "DR_P4_NORDJYLLAND", "uri" :  "234.80.161.8:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.RADIO},

    ChannelId.CHAN_TV3:{"name" : "TV3", "uri" :  "234.80.160.10:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_DR2:{"name" : "DR2", "uri" :  "234.80.160.24:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_DR2_SYNSTOLKNING:{"name" : "DR2_SYNSTOLKNING", "uri" :  "234.80.160.170:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_DANMARK:{"name" : "TV_2_DANMARK", "uri" :  "234.80.160.8:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_NORD:{"name" : "NORD", "uri" :  "234.80.160.179:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_ARD:{"name" : "ARD", "uri" :  "234.80.160.157:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_SVT2:{"name" : "SVT2", "uri" :  "234.80.160.154:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_DISNEY_JUNIOR:{"name" : "DISNEY_JUNIOR", "uri" :  "234.80.160.131:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_HBO_NORDIC_TV:{"name" : "HBO_NORDIC_TV", "uri" :  "234.80.160.212:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_C_MORE_HITS:{"name" : "C_MORE_HITS", "uri" :  "234.80.160.209:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_DANSK_FILMSKAT_TV:{"name" : "DANSK_FILMSKAT_TV", "uri" :  "234.80.160.214:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_TV_2_SPORT:{"name" : "TV_2_SPORT", "uri" :  "234.80.160.36:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_NORDISK_FILM_TV:{"name" : "NORDISK_FILM+_TV", "uri" :  "234.80.160.215:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_TV_2_CHARLIE:{"name" : "TV_2_CHARLIE", "uri" :  "234.80.160.14:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_EVENTKANALEN:{"name" : "EVENTKANALEN", "uri" :  "234.80.160.162:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_C_MORE_STARS:{"name" : "C_MORE_STARS", "uri" :  "234.80.160.196:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_C_MORE_SERIES:{"name" : "C_MORE_SERIES", "uri" :  "234.80.160.210:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_TV2NOR:{"name" : "TV2NOR", "uri" :  "234.80.160.173:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_MID:{"name" : "TV_MID", "uri" :  "234.80.160.174:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_ZDF:{"name" : "ZDF", "uri" :  "234.80.160.158:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_NDR:{"name" : "NDR", "uri" :  "234.80.160.159:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_RTL:{"name" : "RTL", "uri" :  "234.80.160.160:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_DK4:{"name" : "DK4", "uri" :  "234.80.160.192:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_XEE:{"name" : "XEE", "uri" :  "234.80.160.11:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_SVT1:{"name" : "SVT1", "uri" :  "234.80.160.153:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV4:{"name" : "TV4", "uri" :  "234.80.160.155:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_NRK1:{"name" : "NRK1", "uri" :  "234.80.160.156:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_FOLKETINGET:{"name" : "FOLKETINGET", "uri" :  "234.80.160.208:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_PARAMOUNT_NETWORK:{"name" : "PARAMOUNT_NETWORK", "uri" :  "234.80.160.18:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_DR_RAMASJANG:{"name" : "DR_RAMASJANG", "uri" :  "234.80.160.200:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_INFOKANALEN:{"name" : "INFOKANALEN", "uri" :  "234.80.160.190:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_EKSTRAKANALEN:{"name" : "EKSTRAKANALEN", "uri" :  "234.80.160.194:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_TRAILERKANALEN:{"name" : "TRAILERKANALEN", "uri" :  "234.80.160.146:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_NATIONAL_GEOGRAPHIC_WILD:{"name" : "NATIONAL_GEOGRAPHIC_WILD", "uri" :  "234.80.160.204:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_C_MORE_FIRST:{"name" : "C_MORE_FIRST", "uri" :  "234.80.160.195:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_SPORT_X:{"name" : "TV_2_SPORT_X", "uri" :  "234.80.160.37:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.MPEG4_HD},

    ChannelId.CHAN_DISNEY_CHANNEL_HEVC_HD:{"name" : "DISNEY_CHANNEL", "uri" :  "234.80.160.129:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_ZULU_HEVC_HD:{"name" : "TV_2_ZULU", "uri" :  "234.80.160.62:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_FRI_HEVC_HD:{"name" : "TV_2_FRI", "uri" :  "234.80.160.72:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_NEWS_HEVC_HD:{"name" : "TV_2_NEWS", "uri" :  "234.80.160.64:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV3_HEVC_HD:{"name" : "TV3", "uri" :  "234.80.160.59:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_DR2_HEVC_HD:{"name" : "DR2", "uri" :  "234.80.160.69:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_DANMARK_HEVC_HD:{"name" : "TV_2_DANMARK", "uri" :  "234.80.160.57:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_SPORT_HEVC_HD:{"name" : "TV_2_SPORT", "uri" :  "234.80.160.78:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_CHARLIE_HEVC_HD:{"name" : "TV_2_CHARLIE", "uri" :  "234.80.160.63:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_XEE_HEVC_HD:{"name" : "XEE", "uri" :  "234.80.160.60:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD},

    ChannelId.CHAN_TV_2_SPORT_X_HEVC_HD:{"name" : "TV_2_SPORT_X", "uri" :  "234.80.160.79:5900", "sourceType" : SourceType.MULTICAST, "serviceType" : ServiceType.HEVC_HD}
}
import keymaps

class ChannelSelector:

    def select_channel(self, channel , selectorType : SelectorType):
        '''
        Select channel. Depending on the selectorType the channel
        will be selected by injecting an input event to the UI
        or write directly into the streamfs node 

        Arguments:   
                channel -- int type for INPUT_INJECTION SelectorType
                           ChannelId type for STREAMFS_CHANNEL_SELECTION
                           string - valid channel_select0 input string for STREAMFS_CHANNEL_SELECTION
        
        '''
        if selectorType == SelectorType.INPUT_INJECTION:
            return keymaps.set_program(channel)
        elif selectorType == SelectorType.STREAMFS_CHANNEL_SELECTION:
            f = open(STREAMFS_CHANNEL_SELECT0_PATH, "w")
            if (not f):
                f.close()
                return False

            if (isinstance(channel, ChannelId)):
                uri = CHANNEL_LIST[channel]["uri"]
            else:
                uri = channel
            f.write(uri)
            f.close()
            return True

import unittest

class TestrChannelSelection(unittest.TestCase):
    selector = ChannelSelector()
    def _check_channel_selected(self, uri : str):
        f = open(STREAMFS_CHAN_SELECT0_PATH)
        chan = f.read().strip()
        self.assertEqual(chan, uri)
        f.close()

    def test_select_channel_with_ui(self):
        """
        Test channel switch using UI
          Steps: write channel 2 using UI
                 write channel 1
                 Verify in both cases that channel set correctly
        """
        self.assertEqual(self.selector.select_channel(2, SelectorType.INPUT_INJECTION), True)
        self._check_channel_selected(CHANNEL_LIST[ChannelId.CHAN_TV_2]['uri'])
        self.assertEqual(self.selector.select_channel(1, SelectorType.INPUT_INJECTION), True)
        self._check_channel_selected(CHANNEL_LIST[ChannelId.CHAN_DR1]['uri'])

    def test_select_multi_press_channel_with_ui(self):
        """
        Test channel switch using UI
          Steps: write channel 2 using UI
                 write channel 11 using
                 Verify in both cases that channel set correctly
        """
        self.assertEqual(self.selector.select_channel(2, SelectorType.INPUT_INJECTION), True)
        self._check_channel_selected(CHANNEL_LIST[ChannelId.CHAN_TV_2]['uri'])
        self.assertEqual(self.selector.select_channel(11, SelectorType.INPUT_INJECTION), True)
        self._check_channel_selected(CHANNEL_LIST[ChannelId.CHAN_PARAMOUNT_NETWORK]['uri'])

    def test_channel_switch_with_streamfs(self):
        """
        Test channel switch using streamfs interface
        """
        self.assertEqual(self.selector.select_channel(ChannelId.CHAN_TV_2, SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        self._check_channel_selected(CHANNEL_LIST[ChannelId.CHAN_TV_2]['uri'])
        self.assertEqual(self.selector.select_channel(ChannelId.CHAN_DR1, SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        self._check_channel_selected(CHANNEL_LIST[ChannelId.CHAN_DR1]['uri'])

    def test_channel_switch_with_streamfs_using_uri(self):
        """
        Test channel switch using streamfs interface using string uri
        """
        self.assertEqual(self.selector.select_channel(CHANNEL_LIST[ChannelId.CHAN_TV_2]['uri'], SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        self._check_channel_selected(CHANNEL_LIST[ChannelId.CHAN_TV_2]['uri'])
        self.assertEqual(self.selector.select_channel(CHANNEL_LIST[ChannelId.CHAN_DR1]['uri'], SelectorType.STREAMFS_CHANNEL_SELECTION), True)
        self._check_channel_selected(CHANNEL_LIST[ChannelId.CHAN_DR1]['uri'])
        
if __name__ == """__main__""":
    import unittest    
    unittest.main()    
