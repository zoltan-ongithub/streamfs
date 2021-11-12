# Test vectors for unit tests

# Channels
CHANNEL_DR1= {
    'ip' : '234.80.160.50',
    'port' : '5900'
}

CHANNEL_TV2_NEWS= {
    'ip' : '234.80.160.64',
    'port' : '5900'
}

def channel_to_uri(channel):
    '''
        Convert dictionary of channels to URI
    '''
    return 'rtp://' + channel['ip'] + ":" + channel['port'] 

# Time period for TS capture
TS_CAPTURE_TIME_S = 10


# Path to stream0.ts streamfs node
STREAMFS_STREAM0_PATH = "/mnt/streamfs/fcc/stream0.ts"
STREAMFS_CHANNEL_SELECT0_PATH = "/mnt/streamfs/fcc/chan_select0"

# Path to channel selection streamfs node
STREAMFS_CHAN_SELECT0_PATH = "/mnt/streamfs/fcc/chan_select0"

# Path to seek control streamfs node
STREAMFS_SEEK0_PATH = "/mnt/streamfs/fcc/seek0"

# Streamfs TCP server configuration
STREAMFS_HOST = '127.0.0.1'
STREAMFS_PORT = 9995
