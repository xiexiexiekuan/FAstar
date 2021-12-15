/* 
 * File:   variable.h
 * Author: malone
 *
 * Created on November 22, 2012, 10:21 PM
 */

#ifndef VARIABLE_H
#define	VARIABLE_H

#include <string>
#include <vector>
#include <boost/unordered_map.hpp>

#include "typedefs.h"
#include "bayesian_network.h"

namespace datastructures {

    class Variable {
    public:

        Variable(BayesianNetwork *network) {
            index = -1;
            this->network = network;
            parents = VARSET(network->size());
        }

        Variable(BayesianNetwork *network, int index) {
            this->index = index;
            this->network = network;
            parents = VARSET(network->size());
        }
        
        void setArity(int arity) {
            values.clear();
            for (int i = 0; i < arity; i++) {
                values.push_back("Value_" + i);
            }
        }

        void addValue(std::string value) {
            if (valueToIndex.count(value) == 0) {
                valueToIndex[value] = getCardinality();
                values.push_back(value);
            }
        }

        void setValues(std::vector<std::string> values) {
            this->values.clear();
            this->valueToIndex.clear();
            for (auto it = values.begin(); it != values.end(); it++) {
                addValue(*it);
            }
        }

        void setName(std::string &name) {
            this->name = name;
        }

        void setParents(varset parents) {
            VARSET_SET_VALUE(this->parents, parents);
        }

        void setDefaultParentOrder() {
            parentOrder.clear();
            for (int i = 0; i < network->size(); i++) {
                if (VARSET_GET(parents, i)) {
                    parentOrder.push_back(i);
                }
            }
        }

        void addParent(int parent) {
            VARSET_SET(parents, parent);
        }

        std::string &getName() {
            return name;
        }

        int getIndex() {
            return index;
        }

        int getCardinality() {
            return values.size();
        }

        int getValueIndex(std::string value) {
            return valueToIndex[value];
        }

        std::string &getValue(int index) {
            return values[index];
        }

        varset &getParents() {
            return parents;
        }

        std::vector<std::vector<double> > &getParameters() {
            return parameters;
        }

        /**
         * Based on the parent cardinalities, update the size of the parameters
         * array. This will overwrite any existing parameters.
         */
        void updateParameterSize() {
            int count = 1;
            for (int p = 0; p < network->size(); p++) {
                if (VARSET_GET(parents, p)) {
                    count *= network->get(p)->getCardinality();
                }
            }

            parameters.clear();
            for (int i = 0; i < count; i++) {
                std::vector<double> p;
                for (int j = 0; j < getCardinality(); j++) {
                    p.push_back(0);
                }
                parameters.push_back(p);
            }
        }

        void setUniformProbabilities() {
            double uniformValue = 1.0 / getCardinality();
            for (auto it = parameters.begin(); it != parameters.end(); it++) {
                for (int i = 0; i < getCardinality(); i++) {
                    (*it)[i] = uniformValue;
                }
            }
        }

        /**
         * Return the number of parameters for each instantiation of this variable.
         * This is also the number of parent instantiations for this variable.
         *
         * @return the number of distinct parent instantiations
         */
        int getParametersSize() {
            return parameters.size();
        }
        
        void updateMetaInformation(std::string key, std::string value) {
            metaInformation[key] = value;
        }

    private:
        std::string name;
        int index;
        varset parents;
        boost::unordered_map<std::string, int> valueToIndex;
        std::vector<std::string> values;
        BayesianNetwork *network;
        std::vector<std::vector<double> > parameters;
        std::vector<int> parentOrder;
        boost::unordered_map<std::string, std::string> metaInformation;

    };

}

#endif	/* VARIABLE_H */

