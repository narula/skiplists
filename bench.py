#!/usr/bin/python
import os
import sys
import time
import commands

ITR = (1000, 50000, 100000, 200000, 500000, 1000000, 2000000, 10000000)

def one(datafile, fanout):
    f = open(datafile, 'w')
    itr = 3
    for i in ITR:
        d = {}
        for k in range(0, itr):
            t = commands.getoutput("./skiplist %d %d" % (i, fanout))
            print t
            var = t.split(';')
            for v in var:
                pair = v.split(':')
                key = pair[0].lstrip().rstrip()
                try:
                    val = int(pair[1].lstrip().rstrip())
                except (IndexError):
                    print "Error in output! "
                    print "datafile: %s, fanout: %d" % (datafile, fanout)
                    print t
                    return
                if d.has_key(key):
                    d[key] = d[key] + val
                else:
                    d[key] = val
        f.write("%d\t%d\t%d\n" % (d['size']/itr, d['insert']/itr, d['lookup']/itr))
    f.close()
            
if __name__ == "__main__":
    datadir = time.strftime("%Y-%m-%d.%H:%M:%S", time.localtime())
    os.system("mkdir graphs/%s" % datadir)
    os.system("cp graphs/plain.gp graphs/%s/plain.gp" % datadir)
    for f in (2, 4):
        datafile = "graphs/"+datadir+"/plain.dat%d" % f
        one(datafile, f)
    os.system("cd graphs/%s && ./plain.gp && cd ../.. && evince graphs/%s/plain.pdf &" % (datadir, datadir))
