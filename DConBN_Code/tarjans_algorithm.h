#ifndef TARJANS_ALGORITHM_H
#define	TARJANS_ALGORITHM_H

#include <stack>
#include <vector>

#include "typedefs.h"

namespace datastructures {

    class BayesianNetwork;

    class TarjansAlgorithm {
    public:
        TarjansAlgorithm(int size);
        std::vector< std::vector<int> > getSCCs(std::vector<varset> &parents);

    protected:
        void initChildMatrix(std::vector<varset> &parents);
        void strongconnect(int v);
        
        int size;

        /**
         * A child adjacency list representation that is useful for Tarjan's
         * algorithm.
         */
        std::vector<varset> childMatrix;
        /**
         * The index of the next variable in the DFS.
         */
        int index;
        /**
         * A stack to keep track of the variables in the current SCC.
         */
        std::stack<int> S;
        /**
         * An efficient data structure to track the current contents of the S stack.
         */
        varset sContents;

        /**
         * A mapping from a variable to its index value.
         */
        std::vector<int> indices;
        /**
         * A mapping from a variable to its lowlink value.
         */
        std::vector<int> lowlink;
        /**
         * A list of all discovered strongly connected components (cycles).
         */
        std::vector< std::vector<int> > stronglyConnectedComponents;

    };


}

#endif	/* TARJANS_ALGORITHM_H */

