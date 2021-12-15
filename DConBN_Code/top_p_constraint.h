#ifndef TOP_P_CONSTRAINT_H
#define	TOP_P_CONSTRAINT_H

#include <vector>

#include "typedefs.h"

namespace bestscorecalculators {
    class BestScoreCalculator;
}

namespace heuristics {
    
    class TopPConstraint {
    
    public:
        
    std::vector<bitset> findParentSets(std::vector<bestscorecalculators::BestScoreCalculator*> &spgs, int maxScc);
    
    };
}


#endif	/* TOP_P_CONSTRAINT_H */

