#!/usr/bin/python3
#
# OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
#
# To the extent possibile under law, Lee Daniel Crocker has waived all
# copyright and related or neighboring rights to this work.
# <http://creativecommons.org/publicdomain/zero/1.0/>
#
# Basic Python usage example
#

import sys, os
sys.path.insert(1, ".")

import ojrandlib as rl

g = rl.Generator("mwc8222")
g.seed()

print("  {:d}".format(g.rand(100)), " {:d}".format(g.next16()), end="")
print(" {:d}".format(g.next32()), " {:f}".format(g.next_double()))
