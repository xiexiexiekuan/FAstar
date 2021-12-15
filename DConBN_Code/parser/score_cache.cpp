#include "score_cache.h"
#include "bayesian_network.h"
#include "variable.h"

#include <fstream>
#include <iostream>

#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
//
//scoring::ScoreCache::ScoreCache(int variableCount) {
//    this->variableCount = variableCount;
//
//    for (int i = 0; i < variableCount; i++) {
//        cache.push_back(new FloatMap());
//    }
//}


void scoring::ScoreCache::setVariableCount(int variableCount) {
    this->variableCount = variableCount;

    for (int i = 0; i < variableCount; i++) {
        cache.push_back(new FloatMap());
    }
}

std::vector<std::string> parse(std::string line, int start, std::string delimiters) {
    std::vector<std::string> tokens;
    std::string trimmedLine = line.substr(start);
    boost::trim(trimmedLine);
    boost::split(tokens, trimmedLine, boost::is_any_of(delimiters.c_str()), boost::token_compress_on);
    return tokens;
}

std::vector<std::string> parseMetaInformation(std::string line) {
    return parse(line, 4, "=");
}

std::vector<std::string> parseVariableValues(std::string line) {
    return parse(line, 0, " ,");
}

bool contains(std::string line, std::string str) {
    boost::algorithm::to_lower(line);
    boost::algorithm::to_lower(str);

    int index = line.find(str);

    return (index > -1);
}

void scoring::ScoreCache::read(std::string filename) {
    network = new datastructures::BayesianNetwork();
    
    std::ifstream in(filename.c_str());
    std::vector<std::string> tokens;
    std::string line;

    // read meta information until we hit the first variable
    while (!in.eof()) {
        std::getline(in, line);

        // skip empty lines and comments
        if (line.size() == 0 || line.compare(0, 1, "#") == 0) {
            continue;
        }

        // check if we reached the first variable
        if (contains(line, "variable ")) break;

        // make sure this is a meta line
        if (!contains(line, "meta")) {
            throw std::runtime_error("Error while parsing META information of network.  Expected META line or Variable.  Line: '" + line + "'");
        }

        tokens = parseMetaInformation(line);

        if (tokens.size() != 2) {
            throw std::runtime_error("Error while parsing META information of network.  Too many tokens.  Line: '" + line + "'");
        }

        boost::trim(tokens[0]);
        boost::trim(tokens[1]);
        updateMetaInformation(tokens[0], tokens[1]);
    }

    // line currently points to a variable name
    tokens = parse(line, 0, " ");
    datastructures::Variable *v = network->addVariable(tokens[1]);

    // read in the variable names
    while (!in.eof()) {
        std::getline(in, line);

        // skip empty lines and comments
        if (line.size() == 0 || line.compare(0, 1, "#") == 0) {
            continue;
        }

        if (contains(line, "meta")) {
            tokens = parseMetaInformation(line);

            if (contains(tokens[0], "arity")) {
                v->setArity(atoi(tokens[1].c_str()));
            } else if (contains(tokens[0], "values")) {
                std::vector<std::string> values = parseVariableValues(tokens[1]);
                v->setValues(values);
            } else {

                boost::trim(tokens[0]);
                boost::trim(tokens[1]);
                v->updateMetaInformation(tokens[0], tokens[1]);
            }
        }

        if (contains(line, "variable ")) {
            tokens = parse(line, 0, " ");
            v = network->addVariable(tokens[1]);
        }
    }

    in.close();
    setVariableCount(network->size());

    // now that we have the variable names, read in the parent sets
    in.open(filename.c_str());
    while (!in.eof()) {
        std::getline(in, line);
        
        if (line.size() == 0 || line.compare(0, 1, "#") == 0 || contains(line, "meta")) {
            continue;
        }

        tokens = parse(line, 0, " ");
        if (contains(line, "variable ")) {
            v = network->get(tokens[1]);
            continue;
        }

        // then parse the score for the current variable
        VARSET_NEW(parents, network->size());
        float score = -1 * atof(tokens[0].c_str()); // multiply by -1 to minimize

        for (int i = 1; i < tokens.size(); i++) {
            int index = network->getVariableIndex(tokens[i]);
            VARSET_SET(parents, index);
        }

        putScore(v->getIndex(), parents, score);
    }

    in.close();
}

scoring::ScoreCache::~ScoreCache() {
    variableCardinalities.clear();

    for (auto c : cache) {
        delete c;
    }

    cache.clear();
}
