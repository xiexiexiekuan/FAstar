#include <algorithm>

#include "tarjans_algorithm.h"
#include "variable.h"

datastructures::TarjansAlgorithm::TarjansAlgorithm(int size) {
    this->size = size;
    
    for (int i = 0; i < size; i++) {
        childMatrix.push_back(VARSET(size));
        indices.push_back(-1);
        lowlink.push_back(-1);
    }    
}

std::vector< std::vector<int> > datastructures::TarjansAlgorithm::getSCCs(std::vector<varset> &parents) {
    initChildMatrix(parents);
    
    for (int i = 0; i < indices.size(); i++) {
        indices[i] = -1;
    }
    stronglyConnectedComponents.clear();
    
    /**
    * index := 0 S := empty for each v in V do if (v.index is undefined)
    * then strongconnect(v) end if repeat
    */
   index = 0;
   while (S.size() > 0) {
       S.pop();
   }

   for (int v = 0; v < size; v++) {
       if (indices[v] == -1) {
           strongconnect(v);
       }
   }
    
    return stronglyConnectedComponents;
}

void datastructures::TarjansAlgorithm::strongconnect(int v) {
    /**
         * Set the depth index for v to the smallest unused index
         *
         * v.index := index v.lowlink := index index := index + 1 S.push(v)
         *
         */
        indices[v] = index;
        lowlink[v] = index;
        index++;
        S.push(v);
        VARSET_SET(sContents, v);

        /**
         * Consider successors of v
         *
         * for each (v, w) in E do if (w.index is undefined) then // Successor w
         * has not yet been visited; recurse on it strongconnect(w) v.lowlink :=
         * min(v.lowlink, w.lowlink) else if (w is in S) then // Successor w is
         * in stack S and hence in the current SCC v.lowlink := min(v.lowlink,
         * w.index) end if repeat
         *
         */
        for (int w = 0; w < size; w++) {
            if (! VARSET_GET(childMatrix[v], w)) continue;
            
            if (indices[w] == -1) {
                strongconnect(w);
                int mi = std::min(lowlink[v], lowlink[w]);
                lowlink[v] = mi;
            } else if (VARSET_GET(sContents, w)) {
                int mi = std::min(lowlink[v], indices[w]);
                lowlink[v] = mi;
            }
        }

        /**
         * If v is a root node, pop the stack and generate an SCC
         *
         * if (v.lowlink = v.index) then start a new strongly connected
         * component repeat w := S.pop() add w to current strongly connected
         * component until (w = v) output the current strongly connected
         * component end if
         *
         */
        if (lowlink[v] == indices[v]) {
            std::vector<int> scc;
            int w = -1;
            do {
                w = S.top();
                S.pop();
                VARSET_CLEAR(sContents, w);
                scc.push_back(w);
            } while (w != v);

            stronglyConnectedComponents.push_back(scc);
        }
}

void datastructures::TarjansAlgorithm::initChildMatrix(std::vector<varset> &parents) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (VARSET_GET(parents[i], j)) {
                VARSET_SET(childMatrix[j], i);
            }
        }
    }
}