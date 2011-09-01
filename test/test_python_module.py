#!/usr/bin/env python

from __future__ import print_function

import os
import sys

sys.path.insert(0, 'src')
sys.path.insert(0, 'build/src')

import plfit
DATA_DIR = "data"

if not os.path.isdir(DATA_DIR):
    print("Can't find the data directory!", file=sys.stderr)
    sys.exit(1)

print("Testing continuous data...")
data = [float(line) for line in open(os.path.join(DATA_DIR, "continuous_data.txt"))]
result = plfit.plfit_continuous(data)
print("alpha =", result.alpha)
print("xmin =", result.xmin)
print("L =", result.L)
print()

print("Testing discrete data...")
data = [float(line) for line in open(os.path.join(DATA_DIR, "discrete_data.txt"))]
result = plfit.plfit_discrete(data)
print("alpha =", result.alpha)
print("xmin =", result.xmin)
print("L =", result.L)

