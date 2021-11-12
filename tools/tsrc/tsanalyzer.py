
# Transport stream analyzer
# Author: Christo Joseph <christo.j@tataelxsi.co.in>

from errorlistener import ErrorListener, TSErrorCode
from sysloganalyzer import SysLogAnalyzer
from tslistener import TSListener

MPEG_TS_PACKET_SIZE=188
MPEG_TS_NULL_PACKET_PID=8191

class TSAnalyzer(TSListener):
    residue_buffer = bytearray(0)
    continuity_map = {}
    def __init__(self, listener : ErrorListener) -> None:
        '''
        @param listener - error Listener
        '''
        super().__init__()
        self.listener = listener

    def close(self):
        '''
        Close listener
        '''

    def check_discontinuity(self, data : bytes, packet_idx : int):
            pid = ((data[packet_idx+1] << 8) | data[packet_idx+2]) & 0x1FFF
            current_cc = data[packet_idx+3] & 0x0F
            if(pid != MPEG_TS_NULL_PACKET_PID):
                payload_present = data[packet_idx+3] & 0x10
                if (payload_present):
                    #print("Pid: ", pid, " Continuity counter : ", current_cc)
                    prev_cc = self.continuity_map.get(pid)
                    if (prev_cc != None):
                        expected_cc = (prev_cc +1) & 0x0F
                        if (expected_cc != current_cc):
                            packet = data[packet_idx:packet_idx+188]
                            print("discontinuity detected of Pid: ", pid, " Continuity counter : ", current_cc, " expected cc: ", expected_cc)
                            self.listener.on_error_detected(self.listener.error_list['continuity error'], packet.hex())
                            self.continuity_map.clear()
                    else:
                        print("First packet of Pid: ", pid, " Continuity counter : ", current_cc)
                    self.continuity_map[pid] = current_cc

    def new_ts_packet(self, data : bytes):
        '''
        New ts packet received
        '''
        
        residue_buffer_len = len(self.residue_buffer)
        packet_idx = 0
        if (residue_buffer_len):
            if (self.residue_buffer[0] != 0x47):
                print("residue buffer : wrong start code : ", hex(self.residue_buffer[0]))
            else:
                remaining = MPEG_TS_PACKET_SIZE - residue_buffer_len
                self.residue_buffer += data[:remaining]
                self.check_discontinuity(self.residue_buffer, 0)
                packet_idx = remaining
        size = len(data)
        packet_sync_error  = False
        while (packet_idx <= (size - MPEG_TS_PACKET_SIZE)):
            if (data[packet_idx] != 0x47):
                if (False == packet_sync_error):
                    packet = data[packet_idx:packet_idx+MPEG_TS_PACKET_SIZE]
                    print("Error : missing sync bytes packet[0]: ", hex(packet[0]), " idx : ", packet_idx)
                    packet_sync_error = True
                    self.listener.on_error_detected(self.listener.error_list['continuity error'], packet.hex())
                packet_idx += 1
                continue
            if (packet_sync_error):
                print("Sync restored, idx : ", packet_idx)
                packet_sync_error = False
            self.check_discontinuity(data, packet_idx)
            packet_idx += MPEG_TS_PACKET_SIZE
        if (packet_idx < size):
            self.residue_buffer = bytearray(data[packet_idx:])
        else:
            self.residue_buffer = bytearray(0)


class ErrorTracker(ErrorListener):
    """
    TS stream error counter.
    Count number of different errors
    """

    error_count =  {} # [] () {}

    def __init__(self):
        self._syslog_an = SysLogAnalyzer(self)
        self._tsAnalyzer = TSAnalyzer(self)
        pass

    def on_error_detected(self, error: TSErrorCode, packet : str):
        if (error not in self.error_count.keys()):
            self.error_count[error] = 1
        else:
            self.error_count[error] += 1   
        

    def clear(self):
        """
        Clear errors
        """
        self.error_count.clear()

    def get_error_listener(self):
        """
        Return the error listener
        """
        return self._tsAnalyzer

    def close(self):
        """
        Close
        """
        self._syslog_an.close()
        self._tsAnalyzer.close()

