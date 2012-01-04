#!/usr/bin/python
import os
import sys
import time
import commands

ITR = (9, 10, 11)

def one(datafile, lg):
    f = open(datafile, 'w')
    itr = 3
    for i in ITR:
        d = {}
        t = commands.getoutput("./skiplist%d %d" % (lg, 4**i))
        print t
        var = t.split(';')
        for v in var:
            pair = v.split(':')
            key = pair[0].lstrip().rstrip()
            try:
                val = int(pair[1].lstrip().rstrip())
            except (IndexError):
                print "Error in output! "
                print "datafile: %s" % datafile
                print t
                return
            if d.has_key(key):
                d[key] = d[key] + val
            else:
                d[key] = val
        f.write("%d\t%d\n" % (d['size']/itr, d['lookup']/itr))
    f.close()
            
if __name__ == "__main__":
    datadir = "hack-" + time.strftime("%Y-%m-%d.%H:%M:%S", time.localtime())
    os.system("mkdir graphs/%s" % datadir)
    os.system("cp graphs/hack.gp graphs/%s/hack.gp" % datadir)
    for f in (2, 4):
        datafile = "graphs/"+datadir+"/hack.dat%d" % f 
        one(datafile, f)
    os.system("cd graphs/%s && ./hack.gp && cd ../.. && evince graphs/%s/hack.pdf &" % (datadir, datadir))
