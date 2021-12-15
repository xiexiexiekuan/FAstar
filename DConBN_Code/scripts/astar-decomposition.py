#! /usr/bin/env python3

import argparse
import subprocess

import numpy as np

import ScoreCache
import TarjansAlgorithm
import TopPPopsConstraint

def getString(variables):
    l = list()
    for i in range(len(variables)):
        if variables[i] != 0:
            l.append(str(i))
    return ",".join(l)

parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("pss", help="pss file used to find the structure (input)")
args = parser.parse_args()

# read the score cache
sc = ScoreCache.ScoreCache(args.pss)

# get the adjacency matrix
tppc = TopPPopsConstraint.TopPPopsConstraint()
adjacencyMatrix = tppc.findAdjacencyMatrix(sc, p=0)
tarjans = TarjansAlgorithm.TarjansAlgorithm(len(adjacencyMatrix))
sccs = tarjans.getSCCs(adjacencyMatrix)

ancestors = np.zeros( len(adjacencyMatrix) )

totalScore = 0

for scc in sccs:
    ancestorsString = getString(ancestors)
    ancestorsArgument = ""
    if len(ancestorsString) > 0:
        ancestorsArgument = "-p {}".format(ancestorsString)

    sccString = getString(scc)
    sccArgument = "-s {}".format(sccString)

    cmd = "astar {} {} {}".format(args.pss, ancestorsArgument, sccArgument)
    print(cmd)
    out = subprocess.check_output(cmd, shell=True)
    s = out.decode().split("\n")

    for line in s:
        if line.startswith("Found solution:"):
            score = float(line.partition(": ")[2])
            print("Score: {}".format(score))
            totalScore += score

    #print(out)

    ancestors = np.maximum(ancestors, scc)

print("Total score: {}".format(totalScore))

#3501.629395
