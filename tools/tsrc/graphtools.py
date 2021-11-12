import os
import threading
import time
import subprocess
from errorlistener import ErrorListener, TSErrorCode
from multiprocessing import  Lock

from datetime import datetime


def get_cpu_load(sleep_time : int) -> float:
    """
    Get CPU load average across all CPU cores.

    """
    p = subprocess.Popen(['top', '-b', '-d', str(sleep_time), '-n', '1'], stdout=subprocess.PIPE,
                                    universal_newlines=True,
                                    stderr=subprocess.PIPE)
    out, err = p.communicate()
    print(out[0:100])
    print("******************************** ----------------------------------------------------------------------------")
    
class ContErroListener(threading.Thread, ErrorListener):
    graph_depth = 20
    cpu_load = 0
    SLEEP_TIME = 1
    last_idle = last_total = 0

    stats = {
        TSErrorCode.CONTINUITY_ERROR     : [0] * graph_depth,
        TSErrorCode.DEMUXER_AUDIO_ERROR  : [0] * graph_depth,
        TSErrorCode.DEMUXER_VIDEO_ERROR  : [0] * graph_depth,
        TSErrorCode.CPU_LOAD             : [0.0] * graph_depth,
    }
    
    current_time =  int(time.time())

    mutex = Lock()

    fmon = open("monitor.log", "w")

    def get_cpu_load(self) -> float:
        """
        Get CPU load average across all CPU cores.
    
        """
        with open('/proc/stat') as f:
            fields = [float(column) for column in f.readline().strip().split()[1:]]
        f.close()

        idle, total = fields[3], sum(fields)
        idle_delta, total_delta = idle - self.last_idle, total - self.last_total
        self.last_idle, self.last_total = idle, total
        return  100.0 * (1.0 - idle_delta / total_delta)
        
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
            bar = "CPU : {:5.2f} |".format(self.stats[TSErrorCode.CPU_LOAD][i])

            if (self.stats[TSErrorCode.CONTINUITY_ERROR][i] != 0):
                bar += 'C' * self.stats[TSErrorCode.CONTINUITY_ERROR][i]

            if (self.stats[TSErrorCode.DEMUXER_AUDIO_ERROR][i] != 0):
                bar += 'A' * self.stats[TSErrorCode.DEMUXER_AUDIO_ERROR][i]

            if (self.stats[TSErrorCode.DEMUXER_VIDEO_ERROR][i] != 0):
                bar += 'V' * self.stats[TSErrorCode.DEMUXER_VIDEO_ERROR][i]


            print ( "%s: %s" %(cnt, bar))
            i+=1

        print("-------------------------------------")
        
    def run(self):

        while (True):
            time.sleep(self.SLEEP_TIME)

            with self.mutex:
                epoch_time = int(time.time())

                if (self.current_time != epoch_time):
                    self.current_time = epoch_time
                    for v in self.stats.values():
                        v.pop(0)
                        v.append(0)
                self.stats[TSErrorCode.CPU_LOAD][-1] = self.get_cpu_load()
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


                    