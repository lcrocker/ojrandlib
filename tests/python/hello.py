#!/usr/bin/python3
#
# OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
#
# To the extent possibile under law, Lee Daniel Crocker has waived all
# copyright and related or neighboring rights to this work.
# <http://creativecommons.org/publicdomain/zero/1.0/>
#
# Basic Python module test
#

import sys, os
sys.path.insert(1, ".")

import ojrandlib as rl

g = rl.Generator("mwc256")
g.seed()

for i in range(100):
    print("{:10d} ".format(g.next32() & 0xFFFFFFFF), end = "")
    if 4 == (i % 5):
        print()

