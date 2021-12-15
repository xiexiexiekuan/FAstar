#include <vector>

#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <algorithm>
#include <cstdlib>

#include <stdio.h>

#include <boost/unordered_map.hpp>

#include "typedefs.h"

#include "score_cache.h"
#include "best_score_creator.h"
#include "best_score_calculator.h"

namespace po = boost::program_options;

/*
 * 
 */
int main(int argc, char** argv) {

    boost::timer::auto_cpu_timer act;
    int samples;
    int k;
    int seed;
    std::string scoreFile;
    std::string outputFile;
    std::string bestScoreCalculator;

    std::string description = std::string("Generate a set of random samples of pairs of nodes in the order graph and calculate the distance between them.  Example usage: ") + argv[0] + " iris.pss iris.pd_samples";
    po::options_description desc(description);

    desc.add_options()
            ("scoreFile", po::value<std::string > (&scoreFile)->required(), "The file containing the local scores in pss format. First positional argument.")
            ("outputFile", po::value<std::string > (&outputFile)->required(), "The file to which the samples will be written. Second positional argument.")
            ("samples,s", po::value<int> (&samples)->default_value(1000), "The number of samples to generate.")
            ("k,k", po::value<int> (&k)->default_value(4), "The distance in the order graph between the nodes in the samples.")
            ("randomSeed,r", po::value<int> (&seed)->default_value(0), "The seed for the random number generator. '0' means to use time().")
            ("bestScoreCalculator,b", po::value<std::string > (&bestScoreCalculator)->default_value("list"), "The data structure to use for bestScore calculations. [\"list\", \"tree\", \"bitwise\"]")
            ("help,h", "Show this help message.")
            ;

    po::positional_options_description positionalOptions;
    positionalOptions.add("scoreFile", 1);
    positionalOptions.add("outputFile", 1);
    

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc)
            .positional(positionalOptions).run(),
            vm);

    if (vm.count("help") || argc == 1) {
        std::cout << desc;
        return 0;
    }

    po::notify(vm);

    printf("URLearning, Sample Distance Generator\n");
    printf("Dataset: '%s'\n", scoreFile.c_str());
    printf("Output: '%s'\n", outputFile.c_str());
    printf("Samples: '%d'\n", samples);
    printf("K: '%d'\n", k);
    printf("Seed: '%d'\n", seed);
    printf("Best score calculator: '%s'\n", bestScoreCalculator.c_str());

    printf("Reading score cache\n");
    act.start();
    scoring::ScoreCache cache;
    cache.read(scoreFile);
    act.stop();
    act.report();

    int variableCount = cache.getVariableCount();

    printf("Creating best score calculator\n");
    act.start();
    std::vector<bestscorecalculators::BestScoreCalculator*> spgs = bestscorecalculators::create(bestScoreCalculator, cache);
    act.stop();
    act.report();

    printf("Generating and evaluating samples\n");
    act.start();
    
    FILE *out = fopen(outputFile.c_str(), "w");
    
    if (seed == 0) {
        std::srand(time(NULL));
    } else {
        std::srand(seed);
    }
    
    std::vector<int> indices;
    for (int i = 0; i < variableCount; i++) {
        indices.push_back(i);
    }
    
    for (int s = 0; s < samples; s++) {
        // first, randomize the order of the variables
        std::random_shuffle(indices.begin(), indices.end());
        
        // now, pick the layer start node
        int layer = std::rand() % (variableCount - k - 2);
        
        // create the variable sets
        VARSET_NEW(start, variableCount);
        VARSET_NEW(goal, variableCount);
        
        for (int x = 0; x < layer; x++) {
            VARSET_SET(start, indices[x]);
            VARSET_SET(goal, indices[x]);
        }
        
        for (int x = 0; x < k; x++) {
            VARSET_SET(goal, indices[layer+x]);
        }
        
        FloatMap prev;
        init_map(prev);
        
        prev[start] = 0;
        for (int l = 0; l < k; l++) {
            FloatMap cur;
            init_map(cur);
            
            for (auto it = prev.begin(); it != prev.end(); it++) {
                varset variables = it->first;
                float oldG = it->second;
                
                for (int leaf = 0; leaf < variableCount; leaf++) {
                    // make sure this is a variable we need to add
                    if (VARSET_GET(variables, leaf) || !VARSET_GET(goal, leaf)) continue;
                    
                    // so add it
                    VARSET_COPY(variables, newVariables);
                    VARSET_SET(newVariables, leaf);
                    float g = oldG + spgs[leaf]->getScore(variables);

                    auto s = cur.find(newVariables);
                    // check if this is the first time we have generated this node
                    if (s == cur.end()) {
                        cur[newVariables] = g;
                        continue;
                    }
                    
                    // otherwise, check if this is better
                    if (g < s->second) {
                        s->second = g;
                    }
                } // end for each leaf
            } // end for each in prev
            
            prev = cur;
        } // end for each layer
        
        fprintf(out, "%" PRIu64 ",%" PRIu64 ",%f\n", start, goal, prev[goal]);
    } // end for each sample
    
    fclose(out);
    
    act.stop();
    act.report();
    
    for (int i = 0; i < variableCount; i++) {
        delete spgs[i];
    }

    return 0;
}

