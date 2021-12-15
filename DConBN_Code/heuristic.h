#ifndef HEURISTIC_H
#define	HEURISTIC_H

#include "typedefs.h"

namespace bestscorecalculators {
    class BestScoreCalculator;
}

namespace heuristics {
    
    class Heuristic {
        
    public:
        virtual float h(const varset &subnetwork, bool &complete) = 0;
        virtual void initialize(std::vector<bestscorecalculators::BestScoreCalculator*> &spgs) = 0;
        virtual int size() = 0;
        virtual void print() = 0;
        virtual void printStatistics() = 0;
    };
}


#endif	/* HEURISTIC_H */

