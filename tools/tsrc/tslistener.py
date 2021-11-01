# TSListener
# Author: Zoltan Kuscsik <zoltan@zkres.com>

from abc import ABC, abstractmethod

class TSListener(ABC):

    @abstractmethod
    def new_ts_packet(self, data : bytes):
        pass