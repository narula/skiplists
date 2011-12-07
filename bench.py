import os
import commands

def main(args):
    for i in (25000, 50000, 100000, 200000, 300000, 400000, 500000):
        t = commands.getoutput("./skiplist %d" % i)
