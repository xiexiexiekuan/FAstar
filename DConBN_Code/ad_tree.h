#ifndef AD_TREE_H
#define	AD_TREE_H

#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "ad_node.h"
#include "bayesian_network.h"
#include "contingency_table_node.h"
#include "vary_node.h"
#include "record_file.h"
#include "typedefs.h"

namespace scoring {
    
    class ADTree {
    public:
        ADTree() {
            // do nothing
        }
        ADTree(int rMin);
        ~ADTree() {
            if (root != NULL) {
                delete root;
            }
            root = NULL;
        }

        void initialize(datastructures::BayesianNetwork &network, datastructures::RecordFile &recordFile);
        void createTree();
        
        ContingencyTableNode* makeContab(varset variables);

    private:
        ADNode* makeADTree(int i, bitset &recordNums, int depth, varset variables);
        VaryNode* makeVaryNode(int i, bitset &recordNums, int depth, varset variables);
        
        ContingencyTableNode* makeContab(varset remainingVariables, ADNode* node, int nodeIndex);
        ContingencyTableNode* makeContabLeafList(varset variables, bitset &records);

        std::vector< std::vector< bitset > > consistentRecords;

        datastructures::BayesianNetwork network;
        int recordCount;
        int rMin;

        ADNode *root;
        varset zero;
    };

}

#endif	/* AD_TREE_H */

