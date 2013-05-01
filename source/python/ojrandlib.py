# OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
#
# Main python module.
#

import sys
from ctypes import *
from ctypes.util import find_library

_lib = CDLL(find_library("ojrand"))
_lib.ojr_algorithm_name.restype = c_char_p
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

def algorithm_bufsize(id):
    if isinstance(id, str):
        id = algorithm_id(id)
    return _lib.ojr_algorithm_bufsize(id)


class Generator(object):
    def __init__(self, id):
        if isinstance(id, str):
            id = algorithm_id(id)
        if 0 == id:
            id = 1

        self._seed = None
        self._gen = create_string_buffer(_lib._ojr_structure_size())
        _lib._ojr_init(self._gen)
        _lib._ojr_set_algorithm(self._gen, id)

        ss = algorithm_statesize(id)
        self._state = (c_int32 * ss)()
        _lib._ojr_set_state(self._gen, self._state, ss);

        bs = algorithm_bufsize(id)
        self._buf = (c_int32 * bs)()
        _lib._ojr_set_buffer(self._gen, self._buf, bs);
        _lib._ojr_call_open(self._gen)

    def __del__(self):
        _lib._ojr_call_close(self._gen)

    def seed(self, s = None):
        first = (self._seed is None)
        if (s is not None) and hasattr(s, "__len__"):
            size = len(s)
            self._seed = (c_int32 * size)(*s)
        else:
            if s is None:
                size = algorithm_seedsize(_lib._ojr_get_algorithm())
            else:
                size = 1
            self._seed = (c_int32 * size)()
            _lib.ojr_get_system_entropy(newseed, size)

        _lib._ojr_set_seed(self._gen, self._seed, size)
        _lib._ojr_call_seed(self._gen)
        _lib._ojr_set_status(self._gen, 0xb1e55ed2 if first else 0xb1e55ed3)
        return size;

    def reseed(self):
        _lib.ojr_reseed(self._gen)

    def get_seed(self):
        size = _lib.ojr_get_seedsize(self._gen)
        if 0 != size:
            arr = (c_int32 * size)()
            _lib.ojr_get_seed(self._gen, arr, size)
            for i in range(size):
                yield arr[i]

    def algorithm(self):
        return _lib._ojr_get_algorithm(self._gen)

    def next16(self):
        return _lib.ojr_next16(self._gen)

    def next32(self):
        return _lib.ojr_next32(self._gen)

    def next64(self):
        return _lib.ojr_next64(self._gen)

    def next_double(self):
        return _lib.ojr_next_double(self._gen)

    def next_signed_double(self):
        return _lib.ojr_next_signed_double(self._gen)

    def next_gaussian(self):
        return _lib.ojr_next_gaussian(self._gen)

    def rand(self, limit):
        return _lib.ojr_rand(self._gen, limit)
