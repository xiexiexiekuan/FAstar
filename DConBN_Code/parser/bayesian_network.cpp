#include "bayesian_network.h"
#include "variable.h"
#include <stdexcept>

datastructures::BayesianNetwork::BayesianNetwork() {
    name = "";
}

datastructures::BayesianNetwork::BayesianNetwork(int size) {
    for (int i = 0; i < size; i++) {
        datastructures::Variable *v = new datastructures::Variable(this, i);
        std::string name = "Variable_" + i;
        v->setName(name);
        nameToIndex[v->getName()] = variables.size();
        variables.push_back(v);
    }
}

datastructures::Variable *datastructures::BayesianNetwork::get(int variable) {
    return variables[variable];
}

datastructures::Variable *datastructures::BayesianNetwork::get(std::string variable) {
#ifdef DEBUG
    printf("nameToIndex.size(): %d\n", nameToIndex.size());
#endif
    return variables[nameToIndex[variable]];
}

int datastructures::BayesianNetwork::getVariableIndex(std::string variable) {
    return nameToIndex[variable];
}

datastructures::Variable *datastructures::BayesianNetwork::addVariable(std::string name) {
    if (nameToIndex.find(name) != nameToIndex.end()) {
        throw std::runtime_error("Duplicate variable name: '" + name + "'.");
    }
    
    nameToIndex[name] = variables.size();
    Variable *v = new Variable(this, variables.size());
    v->setName(name);
    variables.push_back(v);
    
    return v;
}

void datastructures::BayesianNetwork::setParents(std::vector<varset>& parents) {
    int i = 0;
    for (auto it = variables.begin(); it != variables.end(); it++) {
        (*it)->setParents(parents[i++]);
    }

    setDefaultParentOrder();
}

/**
 * Set the parent order for each variable to a default ordering of
 * increasing parent index. For example, if $X_0$ has parents $X_1$, $X_3$,
 * $X_7$, then the ordering will be set to $X_1$,$X_3$,$X_7$. This is useful
 * if the network was created by, e.g., structure learning or random
 * generation. If network was read in from a file, though, the order is
 * already set by the order parents appear in the file and should not be
 * changed.
 */
void datastructures::BayesianNetwork::setDefaultParentOrder() {
    for (int i = 0; i < size(); i++) {
        get(i)->setDefaultParentOrder();
    }
}

/**
 * Based on the current structure, generate uniform parameters.
 */
void datastructures::BayesianNetwork::setUniformProbabilities() {

    updateParameterSizes();

    // for each variable
    for (int i = 0; i < size(); i++) {
        get(i)->setUniformProbabilities();
    }
}

/**
 * Update the conditional probability distributions of all variables in the
 * network based on their current parent sets. This just calls
 * {@link Variable#updateParameterSize()} for each variable.
 */
void datastructures::BayesianNetwork::updateParameterSizes() {
    for (int i = 0; i < size(); i++) {
        get(i)->updateParameterSize();
    }
}

int datastructures::BayesianNetwork::getCardinality(int variable) {
    return variables[variable]->getCardinality();
}

int datastructures::BayesianNetwork::getMaxCardinality() {
    int max = 0;
    for(int i = 0; i < size(); i++) {
        int c = getCardinality(i);
        if (c > max) {
            max = c;
        }
    }
    return max;
}

int datastructures::BayesianNetwork::size() {
    return variables.size();
}
