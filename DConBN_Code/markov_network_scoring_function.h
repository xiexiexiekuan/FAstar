#ifndef MARKOV_NETWORK_SCORING_FUNCTION_H
#define	MARKOV_NETWORK_SCORING_FUNCTION_H

#include <string>
#include <stdexcept>

#include "score_cache.h"
#include "typedefs.h"

namespace scoring {

    class MarkovNetworkScoringFunction {
        
    public:
        virtual float calculateScore(varset clique, FloatMap &cache) = 0;
    };
    
    
}

#endif	/* MARKOV_NETWORK_SCORING_FUNCTION_H */

