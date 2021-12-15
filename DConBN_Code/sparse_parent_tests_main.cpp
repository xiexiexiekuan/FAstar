#include <string>
#include <iostream>
#include <ostream>
#include <vector>
#include <cstdlib>

#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
    std::string scoreFile;
    std::string bestScoreCalculator;
    int maxLayer;

    std::string description = std::string("Test sparse parent representations.  Example usage: ") + argv[0] + " iris.pss";
    po::options_description desc(description);

    desc.add_options()
            ("scoreFile", po::value<std::string > (&scoreFile)->required(), "The file containing the local scores in pss format. First positional argument.")
            ("bestScoreCalculator,b", po::value<std::string > (&bestScoreCalculator)->default_value("list"), "The data structure to use for bestScore calculations. [\"list\", \"tree\", \"bitwise\"]")
            ("maxLayer,l", po::value<int> (&maxLayer)->default_value(10), "All BestScore calculations are made up to this layer (inclusive).")
            ("help,h", "Show this help message.")
            ;

    po::positional_options_description positionalOptions;
    positionalOptions.add("scoreFile", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc)
            .positional(positionalOptions).run(),
            vm);

    if (vm.count("help") || argc == 1) {
        std::cout << desc;
        return 0;
    }

    po::notify(vm);

    printf("URLearning, Sparse Parent Tests\n");
    printf("Dataset: '%s'\n", scoreFile.c_str());
    printf("Best score calculator: '%s'\n", bestScoreCalculator.c_str());
    printf("Max layer: '%d\n", maxLayer);

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


    VARSET_NEW(max, variableCount);
    VARSET_SET(max, variableCount);


    VARSET_NEW(empty, scoreCache.getVariableCount());
    VARSET_NEW(variables, scoreCache.getVariableCount());

    printf("Timing bestScore calculations\n");
    act.start();
    for (int variable = 0; variable < variableCount; variable++) {

        printf("Variable %d\n", variable);
        bestscorecalculators::BestScoreCalculator *bsc = spgs[variable];
        float score = bsc->getScore(empty);

        for (int layer = 1; layer < maxLayer && layer < variableCount; layer++) {
            printf("Layer: %d\n", layer);

            VARSET_CLEAR_ALL(variables);
            for (int i = 0; i < layer; i++) {
                VARSET_SET(variables, i);
            }

            while (VARSET_LESS_THAN(variables, max)) {
                if (!VARSET_GET(variables, variable)) {
                    score = bsc->getScore(variables);
                }

                // find the next combination
                variables = nextPermutation(variables);
            }
        }
    }

    act.stop();
    act.report();

    for (int i = 0; i < variableCount; i++) {
        delete spgs[i];
    }

    return 0;
}

