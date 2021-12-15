#ifndef MARKOV_NETWORK_SCORING_FUNCTION_CREATOR_H
#define	MARKOV_NETWORK_SCORING_FUNCTION_CREATOR_H

#include "markov_network_bdeu_scoring_function.h"
#include "bayesian_network.h"

namespace scoring {
    
    class ADTree;
    class Constraints;
    
    
    inline scoring::MarkovNetworkScoringFunction *createMarkovNetworkScoringFunction(std::string scoringFunction, std::string argument, datastructures::BayesianNetwork& network, ADTree *adTree, Constraints *constraints) {
        MarkovNetworkScoringFunction *f = NULL;

        boost::algorithm::to_lower(scoringFunction);


        if (scoringFunction == "bdeu") {
            float ess = atof(argument.c_str());
            f = new MarkovNetworkBDeuScoringFunction(ess, network, adTree, constraints);
        } else {
            throw std::runtime_error("Invalid Markov network scoring function: '" + scoringFunction + "'.  Valid options are: 'bdeu'.");
        }
        
        return f;
    }
}


#endif	/* MARKOV_NETWORK_SCORING_FUNCTION_CREATOR_H */

