#ifndef MARKOV_NETWORK_BDEU_SCORING_FUNCTION_H
#define	MARKOV_NETWORK_BDEU_SCORING_FUNCTION_H

#include <boost/unordered_set.hpp>
#include <vector>

#include "typedefs.h"

#include "markov_network_scoring_function.h"
#include "bayesian_network.h"
#include "ad_tree.h"
#include "constraints.h"
#include "contingency_table_node.h"

namespace scoring {
    
    struct scratch {
        /**
         * Scratch variables for calculating the most recent score across different
         * function calls.
         */
        float score;
        float lg_ess;
        float a_j;
        float lg_j;
        
        int n;
        float lg_ess_plus_n;
        
        /**
         * A cache to store all of the parent sets which were pruned because they
         * violated the constraints. We need to store these because they cannot be
         * used when checking for weak pruning.
         */
        boost::unordered_set<varset> invalidParents;
    };
    
    class MarkovNetworkBDeuScoringFunction : public MarkovNetworkScoringFunction {
        
    public:
        MarkovNetworkBDeuScoringFunction(float ess, datastructures::BayesianNetwork &network, ADTree *adTree, Constraints *constraints);

        ~MarkovNetworkBDeuScoringFunction() {
            // no pointers 
        }

        float calculateScore(varset clique, FloatMap &cache);
        
    private:
        void lg(varset clique, scratch *s);
        void calculate(ContingencyTableNode* ct, varset &clique, int previousVariable, scratch *s);
        
        /**
         * A reference to the AD-tree used for finding the sufficient statistics.
         */
        ADTree *adTree;
        /**
         * A reference to the Bayesian network (and data specifications) for the
         * dataset.
         */
        datastructures::BayesianNetwork network;
        /**
         * A list of all the constraints on valid scores for this dataset.
         */
        Constraints *constraints;
        /**
         * The equivalent sample size to use in this version of BDeu.
         */
        float ess;
        /**
         * The scratch space for score calculations of each thread.
         */
        std::vector<scratch*> scratchSpace;
    };
}


#endif	/* MARKOV_NETWORK_BDEU_SCORING_FUNCTION_H */

