from enum import Enum
from abc import ABC, abstractmethod

DVBSNOOP_SEPARATOR="=========================================================="

class TSErrorCode(Enum):
    '''
    Transport stream error codes
    '''
    CONTINUITY_ERROR = 1
    DEMUXER_AUDIO_ERROR = 2
    DEMUXER_VIDEO_ERROR = 3 
    CPU_LOAD = 4

class ErrorListener(ABC):
    '''
    Error listner class
    '''

    # errors in dvbsnoop
    error_list = {
        'continuity error':  TSErrorCode.CONTINUITY_ERROR,
    }

    # errors in syslog
    sylog_error_list = {
        'AUDIO_ISSUE':  TSErrorCode.DEMUXER_AUDIO_ERROR,
        'VIDEO_ISSUE':  TSErrorCode.DEMUXER_VIDEO_ERROR,
    }

    @abstractmethod
    def on_error_detected(self, error: TSErrorCode, packet : str):
        pass
