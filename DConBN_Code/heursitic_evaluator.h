#ifndef HEURSITIC_EVALUATOR_H
#define	HEURSITIC_EVALUATOR_H

#include <vector>

#include "typedefs.h"

#include "best_score_calculator.h"
#include "heuristic.h"

namespace heuristics {
    
    class HeuristicEvaluator {
        
    public:
        HeuristicEvaluator(std::vector<bestscorecalculators::BestScoreCalculator*> &spgs, std::vector<varset> &varsets);
        
        void init();
        float evaluate(heuristics::Heuristic *heuristic);
        
    private:
        
        std::vector<bestscorecalculators::BestScoreCalculator*> spgs;
        std::vector<varset> varsets;
    };
    
}


#endif	/* HEURSITIC_EVALUATOR_H */

