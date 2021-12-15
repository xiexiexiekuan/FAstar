#include "top_p_constraint.h"

#include "tarjans_algorithm.h"
#include "best_score_calculator.h"

int getMaxScc(std::vector< std::vector<int> > &sccs) {
    int max = 0;
    
    for (auto it = sccs.begin(); it != sccs.end(); it++) {
        if ( (*it).size() > max) {
            max = (*it).size();
        }
    }
    
    return max;
}

std::vector<bitset> heuristics::TopPConstraint::findParentSets(std::vector<bestscorecalculators::BestScoreCalculator*>& spgs, int maxScc) {
    
    std::vector<bitset> included; // the parent sets included
    std::vector<varset> prg; // the current parent relation graph
    std::vector<int> cur; // the next parent set to consider for each variable
    std::vector< std::vector<int> > sccs; // the number of SCCs in the current prg
    int curScc = 0;
    int variableCount = spgs.size();
    datastructures::TarjansAlgorithm ta(variableCount);
    
    for (int i = 0; i < variableCount; i++) {
        included.push_back(BITSET_NEW(spgs[i]->size()));
        prg.push_back(VARSET(variableCount));
        cur.push_back(0);
    }
    
    int inCount = 0;
    
    // fist, add the best parent set for each
    for (int i = 0; i < variableCount; i++) {
        VARSET_SET_VALUE(prg[i], spgs[i]->getParents(0));
        cur[i] = 1;
        inCount++;
    }
    
    sccs = ta.getSCCs(prg);
    curScc = getMaxScc(sccs);
    
    while(curScc <= maxScc) {
        // find the next best score and include it
        int best = 0;
        float bestScore = spgs[0]->getScore(cur[0]);
        for (int i = 1; i < variableCount; i++) {
            float thisScore = spgs[i]->getScore(cur[i]);
            if (thisScore < bestScore) {
                bestScore = thisScore;
                best = i;
            }
        }
        
        // try to add the new parent set
        prg[best] = VARSET_OR(prg[best], spgs[best]->getParents(cur[best]));
        cur[best]++;
        
        sccs = ta.getSCCs(prg);
        curScc = getMaxScc(sccs);
        inCount++;
        
    }
    
    printf("inCount: %d\n", inCount);
    
    
    
    return included;
    
}
