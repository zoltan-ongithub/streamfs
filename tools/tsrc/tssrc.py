# TS Src base class
# Author: Zoltan Kuscsik <zoltan@zkres.com>

from tslistener import TSListener
from abc import ABC, abstractmethod

class TSSrc(ABC):
    '''
    Abstract Base Class for various TS implementations
    '''
    @abstractmethod
    def register_listener(self, listener : TSListener):
        '''
        Register TS listener
        '''
        pass

    @abstractmethod
    def open(self, uri : str) -> bool:
        '''
        Open TS Stream
        '''
        pass
    