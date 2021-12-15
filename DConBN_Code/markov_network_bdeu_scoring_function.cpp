#include "markov_network_bdeu_scoring_function.h"
#include "contingency_table_node.h"

#include <math.h>

scoring::MarkovNetworkBDeuScoringFunction::MarkovNetworkBDeuScoringFunction(float ess, datastructures::BayesianNetwork& network, ADTree *adTree, Constraints *constraints) {
    this->network = network;
    this->adTree = adTree;
    this->constraints = constraints;
    this->ess = ess;
    
    // find the number of records
    VARSET_NEW(empty, network.size());
    ContingencyTableNode *ct = adTree->makeContab(empty);
    int n = ct->getValue();

    for (int x = 0; x < network.size(); x++) {
        scratch *s = new scratch();
        s->lg_ess = lgamma(ess);
        s->n = n;
        s->lg_ess_plus_n = lgamma(ess + n);
        scratchSpace.push_back(s);
    }
}

void scoring::MarkovNetworkBDeuScoringFunction::lg(varset clique, scratch* s) {
    int r = 1;
    for (int pa = 0; pa < network.size(); pa++) {
        if (VARSET_GET(clique, pa)) {
            r *= network.getCardinality(pa);
        }
    }
    
    s->a_j = ess / r;
    s->lg_j = lgamma(s->a_j);
}

float scoring::MarkovNetworkBDeuScoringFunction::calculateScore(varset clique, FloatMap& cache) {
    // only single-threaded right now
    scratch *s = scratchSpace[0];

    lg(clique, s);

    s->score = s->lg_ess - s->lg_ess_plus_n;

    ContingencyTableNode *ct = adTree->makeContab(clique);
    calculate(ct, clique, -1, s);

    delete ct;

    return s->score;
}

void scoring::MarkovNetworkBDeuScoringFunction::calculate(ContingencyTableNode* ct, varset &clique, int previousVariable, scratch *s) {

    // if this is a leaf in the AD-tree
    if (ct->isLeaf()) {
        // update the instantiation count of this set of parents
        int count = ct->getValue();

        if (count > 0) {
            // update the score for this variable, parent instantiation
            s->score -= s->lg_j;
            s->score += lgamma(s->a_j + ct->getValue());
        }
        return;
    }

    // which actual variable are we looking at
    int thisVariable = previousVariable + 1;
    for (; thisVariable < network.size(); thisVariable++) {
        if (VARSET_GET(clique, thisVariable)) break;
    }

    // recurse
    for (int k = 0; k < network.getCardinality(thisVariable); k++) {
        ContingencyTableNode *child = ct->getChild(k);
        if (child != NULL) {
            calculate(child, clique, thisVariable, s);
        }
    }

}
