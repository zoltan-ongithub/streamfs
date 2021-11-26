#
# TSB seek control API
#

class SeekControl:

    fsnode : str

    # parameterized constructor
    def __init__(self, path : str):
        self.fsnode = path

    def setSeek(self, position : int):
        f = open(self.fsnode, "w")
        if (not f):
            f.close()
            return False

        f.write(str(position))
        f.close()
        return True

    def getTsbSize(self) -> int:
        f = open(self.fsnode)
        if (not f):
            f.close()
            return False

        tsbSize = int(f.read().strip().split(',')[1])
        f.close()
        return tsbSize
