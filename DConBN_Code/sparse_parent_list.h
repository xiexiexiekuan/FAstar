#ifndef SPARSEPARENTLIST_H
#define	SPARSEPARENTLIST_H

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <stdexcept>

#include "best_score_calculator.h"
#include "score_cache.h"
#include "typedefs.h"

namespace bestscorecalculators {

    class SparseParentList : public BestScoreCalculator {
    public:
        SparseParentList(const int variable, const int variableCount);
        ~SparseParentList();
        void initialize(const scoring::ScoreCache &scoreCache);

        float getBestScore() const {
            return scores[0];
        }
        float getScore(varset &pars);

        varset &getParents() {
            return parents[bestIndex];
        }
        float getScore(int index);
        varset &getParents(int index);

        int size() {
            return parents.size();
        }
        
        void print() {
            printf("Sparse Parent Bitwise, variable: %d, size: %d\n", variable, size());
        }
        
    private:
        int variableCount;
        int variable;
        int bestIndex;
        std::vector<varset> parents;
        std::vector<float> scores;
    };

}

#endif	/* SPARSEPARENTLIST_H */

