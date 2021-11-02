import os
import threading
import time

from errorlistener import ErrorListener, TSErrorCode
from multiprocessing import  Lock

from datetime import datetime

class ContErroListener(threading.Thread, ErrorListener):
    graph_depth = 20
    stats = {
        TSErrorCode.CONTINUITY_ERROR  : [0] * graph_depth,
        TSErrorCode.DEMUXER_AUDIO_ERROR  : [0] * graph_depth,
        TSErrorCode.DEMUXER_VIDEO_ERROR  : [0] * graph_depth,
    }
    
    current_time =  int(time.time())

    mutex = Lock()

    fmon = open("monitor.log", "w")

    def graph(self):
        i = - self.graph_depth
        now = datetime.now()

        print("**************************************")
        print("# C - continuity error ")
        print("# A - audio error ")
        print("# V - video error ")
        print("# time: " + datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3])
        print("**************************************")

        for i in range(0, self.graph_depth):
            cnt = "{0:>{1}}|".format(-self.graph_depth + i, 5)
            bar = ""

            if (self.stats[TSErrorCode.CONTINUITY_ERROR][i] != 0):
                bar += 'C' * self.stats[TSErrorCode.CONTINUITY_ERROR][i]

            if (self.stats[TSErrorCode.DEMUXER_AUDIO_ERROR][i] != 0):
                bar += 'A' * self.stats[TSErrorCode.DEMUXER_AUDIO_ERROR][i]

            if (self.stats[TSErrorCode.DEMUXER_VIDEO_ERROR][i] != 0):
                bar += 'V' * self.stats[TSErrorCode.DEMUXER_VIDEO_ERROR][i]

            print ( "%s %s" %(cnt, bar))
            i+=1

        print("-------------------------------------")
        
    def run(self):
        while (True):
            time.sleep(1)

            with self.mutex:
                epoch_time = int(time.time())

                if (self.current_time != epoch_time):
                    self.current_time = epoch_time
                    for v in self.stats.values():
                        v.pop(0)
                        v.append(0)
                self.graph()

    def on_error_detected(self, error: TSErrorCode, packet: str):
        with self.mutex:
            if error == TSErrorCode.CONTINUITY_ERROR:
                self.stats[TSErrorCode.CONTINUITY_ERROR][-1] += 1

            elif error == TSErrorCode.DEMUXER_AUDIO_ERROR:
                self.stats[TSErrorCode.DEMUXER_AUDIO_ERROR][-1] += 1

            elif  error == TSErrorCode.DEMUXER_VIDEO_ERROR:
                self.stats[TSErrorCode.DEMUXER_VIDEO_ERROR][-1] += 1
            self.fmon.write(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n")
            self.fmon.write("# time: " + datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3] + "\n")
            self.fmon.write("# error type: " + error.name + "\n")
            self.fmon.write(packet)
            os.sync()


                    