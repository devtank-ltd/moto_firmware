from __future__ import print_function
import os
import sys
import time
import serial
import weakref
import serial.tools.list_ports


_debug_fnc = lambda *args : print(*args, file=sys.stderr) if "DEBUG" in os.environ else None


def debug_print(msg):
    if _debug_fnc:
        _debug_fnc(msg)

def set_debug_print(_func):
    global _debug_fnc
    _debug_fnc = _func

def get_debug_print():
    return _debug_fnc


class io_board_py_t(object):
    __LOG_START_SPACER = b"============{"
    __LOG_END_SPACER   = b"}============"
    READTIME = 2
    NAME_MAP = {}

    def __init__(self, dev, loti_label = None):
        self.NAME_MAP = type(self).NAME_MAP
        self.comm_port = dev
        self.comm = serial.Serial(
                port=dev,
                baudrate=115200,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                timeout=1)
        while self.comm.in_waiting:
            self.comm.readline(self.comm.in_waiting)
        self.log_prefix = "%s: " % loti_label if loti_label else ""
        debug_print("%sDrained" % self.log_prefix)

    def __getattr__(self, item):
        debug_print("%sBinding name lookup %s" %(self.log_prefix, item))
        getter = self.NAME_MAP.get(item, None)
        if getter:
            return getter(self)
        raise AttributeError("Attribute %s not found" % item)

    def _read_line(self):
        line = self.comm.readline().strip()
        debug_print("%s>> : %s" % (self.log_prefix, line))
        return line

    def read_response(self):
        line = self._read_line()
        start = time.time()
        while line != type(self).__LOG_START_SPACER:
            if (time.time() - start) > type(self).READTIME:
                raise ValueError("Comms read took too long.")
            line = self._read_line()
            assert time.time() - start < type(self).READTIME

        line = self._read_line()
        data_lines = []

        while line != type(self).__LOG_END_SPACER:
            if (time.time() - start) > type(self).READTIME:
                raise ValueError("Comms read took too long.")
            data_lines += [line]
            line = self._read_line()
            assert time.time() - start < type(self).READTIME

        return data_lines

    def command(self, cmd):
        self.comm.write(cmd)
        self.comm.write(b'\n')
        self.comm.flush()
        debug_print("%s<< %s" % (self.log_prefix, cmd))
        return self.read_response()
