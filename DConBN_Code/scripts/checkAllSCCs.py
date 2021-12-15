#! /usr/bin/env python3

import argparse

import numpy as np

import ScoreCache
import TarjansAlgorithm
import TopPPopsConstraint

def countSCCs(pss):
    # read the score cache
    sc = ScoreCache.ScoreCache(pss)

    # get the adjacency matrix
    tppc = TopPPopsConstraint.TopPPopsConstraint()
    adjacencyMatrix = tppc.findAdjacencyMatrix(sc, p=0)
    tarjans = TarjansAlgorithm.TarjansAlgorithm(len(adjacencyMatrix))
    sccs = tarjans.getSCCs(adjacencyMatrix)

    print("{},{}".format(pss, len(sccs)))


parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("pss", help="pss file used to find the structure (input)", nargs='+')
args = parser.parse_args()

for pss in args.pss:
    try:
        countSCCs(pss)
    except:
        pass
