# OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
#
# Main python module.
#

import sys
from ctypes import *
from ctypes.util import find_library

_lib = CDLL(find_library("ojrand"))
_lib.ojr_algorithm_name.restype = c_char_p
_lib.ojr_new.restype = c_void_p
_lib.ojr_next16.restype = c_int16
_lib.ojr_next32.restype = c_int32
_lib.ojr_next64.restype = c_int64
_lib.ojr_next_double.restype = c_double
_lib.ojr_next_signed_double.restype = c_double
_lib.ojr_next_gaussian.restype = c_double

def algorithms():
    for i in range(_lib.ojr_algorithm_count()):
        yield _lib.ojr_algorithm_name(i + 1).decode()

def algorithm_id(name):
    return _lib.ojr_algorithm_id(name.encode())

def algorithm_name(id):
    return _lib.ojr_algorithm_name(id).decode()

def algorithm_seedsize(id):
    if isinstance(id, str):
        id = algorithm_id(id)
    return _lib.ojr_algorithm_seedsize(id)

def algorithm_statesize(id):
    if isinstance(id, str):
        id = algorithm_id(id)
    return _lib.ojr_algorithm_statesize(id)


class Generator(object):
    def __init__(self, id):
        if isinstance(id, str):
            id = algorithm_id(id)
        self.gen = _lib.ojr_new(id)

    def __del__(self):
        _lib.ojr_close(self.gen)

    def seed(self, s = None):
        if s is None:
            return _lib.ojr_seed(self.gen, c_void_p(0), 0)
        if not hasattr(s, "__len__"):
            s = [s]
        arr = (c_int32 * len(s))(*s)
        return _lib.ojr_seed(self.gen, arr, len(s))

    def reseed(self, s = None):
        if s is None:
            return _lib.ojr_reseed(self.gen, c_void_p(0), 0)
        if not hasattr(s, "__len__"):
            s = [s]
        arr = (c_int32 * len(s))(*s)
        return _lib.ojr_reseed(self.gen, arr, len(s))

    def get_seed(self):
        size = _lib.ojr_get_seedsize(self.gen)
        if 0 != size:
            arr = (c_int32 * size)()
            _lib.ojr_get_seed(self.gen, arr, size)
            for i in range(size):
                yield arr[i]

    def algorithm(self):
        return _lib.ojr_get_algorithm(self.gen)

    def next16(self):
        return _lib.ojr_next16(self.gen)

    def next32(self):
        return _lib.ojr_next32(self.gen)

    def next64(self):
        return _lib.ojr_next64(self.gen)

    def next_double(self):
        return _lib.ojr_next_double(self.gen)

    def next_signed_double(self):
        return _lib.ojr_next_signed_double(self.gen)

    def next_gaussian(self):
        return _lib.ojr_next_gaussian(self.gen)

    def rand(self, limit):
        return _lib.ojr_rand(self.gen, limit)
