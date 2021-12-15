/* 
 * File:   bayesian_network.h
 * Author: malone
 *
 * Created on November 22, 2012, 10:26 PM
 */

#ifndef BAYESIAN_NETWORK_H
#define	BAYESIAN_NETWORK_H

#include <string>
#include <vector>

#include <boost/unordered_map.hpp>

#include "typedefs.h"

namespace datastructures {
    
    class Variable;

    class BayesianNetwork {
    public:

        BayesianNetwork();
        BayesianNetwork(int size);

        int size();

        Variable *get(int variable);
        Variable *get(std::string variable);
        int getVariableIndex(std::string variable);
        void setParents(std::vector<varset> &parents);
        void setDefaultParentOrder();
        void setUniformProbabilities();
        void updateParameterSizes();
        Variable *addVariable(std::string name);

        int getCardinality(int variable);
        int getMaxCardinality();
    private:

        std::string name;
        std::vector<Variable*> variables;
        boost::unordered_map<std::string, int> nameToIndex;

    };

}


#endif	/* BAYESIAN_NETWORK_H */

