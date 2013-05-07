# OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
#
# Main python module.
#

import sys, os
from ctypes import *
from ctypes.util import find_library

# Work around bug in Windows Python3
if "nt" == os.name:
    _lib = CDLL(find_library("ojrand.dll"))
else:
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

        self.gen = create_string_buffer(_lib.ojr_structure_size())
        _lib.ojr_init(self.gen)
        _lib.ojr_set_algorithm(self.gen, id)

        ss = algorithm_statesize(id)
        self.state = (c_int32 * ss)()
        _lib.ojr_set_state(self.gen, self.state, ss);

        bs = algorithm_bufsize(id)
        self.buf = (c_int32 * bs)()
        _lib.ojr_set_buffer(self.gen, self.buf, bs);
        _lib.ojr_call_open(self.gen)

    def __del__(self):
        _lib.ojr_call_close(self.gen)

    def seed(self, s = None):
        if s is None:
            size = algorithm_seedsize(_lib.ojr_get_algorithm(self.gen))
            seed = (c_int32 * size)()
            _lib.ojr_get_system_entropy(seed, size)
        else:
            if hasattr(s, "__len__"):
                size = len(s)
                seed = (c_int32 * size)(*s)
            else:
                size = 1
                seed = (c_int32 * size)([s])

        _lib.ojr_call_seed(self.gen, seed, size)
        _lib.ojr_set_seeded(self.gen, 1)

    def reseed(self):
        size = algorithm_seedsize(_lib.ojr_get_algorithm(self.gen))
        seed = (c_int32 * size)()
        _lib.ojr_get_system_entropy(seed, size)
        _lib.ojr_call_reseed(self.gen, seed, size)

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
