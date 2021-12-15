#ifndef GOBNILP_HEURISTIC_H
#define	GOBNILP_HEURISTIC_H

#include "heuristic.h"
#include "score_cache.h"


namespace heuristics {

class GobnilpHeuristic : public Heuristic {
    
public:
    GobnilpHeuristic();
    GobnilpHeuristic(scoring::ScoreCache &cache, int variableThreshold, int popsThreshold);
    ~GobnilpHeuristic();
    
    int size();
    float h(const varset &variables, bool &complete);
    void print() {}
    void printStatistics() {
        printf("gobnilp heuristic does not collect statistics\n");
    }
    
private:
    scoring::ScoreCache &cache;
    int variableThreshold;
    int popsThreshold;
};
}

#endif	/* GOBNILP_HEURISTIC_H */

