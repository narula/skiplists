#!/usr/bin/python
import os
import commands

ITR = (50000, 100000, 200000, 500000, 1000000, 2000000, 3000000, 5000000, 10000000)

def main(args):
    f = open('graphs/plain.dat', 'w')
    itr = 3
    for i in ITR:
        d = {}
        for k in range(0, itr):
            t = commands.getoutput("./skiplist %d" % i)
            print t
            var = t.split(';')
            for v in var:
                pair = v.split(':')
                key = pair[0].lstrip().rstrip()
                val = int(pair[1].lstrip().rstrip())
                if d.has_key(key):
                    d[key] = d[key] + val
                else:
                    d[key] = val
        f.write("%d\t%d\t%d\n" % (d['size']/itr, d['insert']/itr, d['lookup']/itr))
    f.close()
    os.system("cd graphs && ./plain.gp && cd .. && evince graphs/plain.pdf")
            
if __name__ == "__main__":
    main(None)
