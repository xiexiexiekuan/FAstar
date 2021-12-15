#include <vector>

#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fstream>
#include <iostream>

#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "typedefs.h"

#include <stdio.h>

#include "score_cache.h"

#include "file_pattern_database.h"

#include "best_score_creator.h"
#include "best_score_calculator.h"

namespace po = boost::program_options;

/*
 * 
 */
int main(int argc, char** argv) {

    boost::timer::auto_cpu_timer act;
    std::string scoreFile;
    std::string pdFile;
    std::string sampleFile;
    std::string outputFile;
    std::string bestScoreCalculator;

    std::string description = std::string("Evaluate the quality of static pattern databases contained in a file against a set of samples.  Example usage: ") + argv[0] + " iris.pss iris.pds iris.pds.samples";
    po::options_description desc(description);

    desc.add_options()
            ("scoreFile", po::value<std::string > (&scoreFile)->required(), "The file containing the local scores in pss format. First positional argument.")
            ("pdFile", po::value<std::string > (&pdFile)->required(), "The file containing the static pattern database specifications. Second positional argument.")
            ("sampleFile", po::value<std::string > (&sampleFile)->required(), "The file containing the known distances. Third positional argument.")
            ("outputFile", po::value<std::string > (&outputFile)->required(), "The file to which the evaluation results will be written. Fourth positional argument.")
            ("bestScoreCalculator,b", po::value<std::string > (&bestScoreCalculator)->default_value("list"), "The data structure to use for bestScore calculations. [\"list\", \"tree\", \"bitwise\"]")
            ("help,h", "Show this help message.")
            ;

    po::positional_options_description positionalOptions;
    positionalOptions.add("scoreFile", 1);
    positionalOptions.add("pdFile", 1);
    positionalOptions.add("sampleFile", 1);
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

    printf("URLearning, Pattern Database Evaluator\n");
    printf("Dataset: '%s'\n", scoreFile.c_str());
    printf("Pattern databases: '%s'\n", pdFile.c_str());
    printf("Samples: '%s'\n", sampleFile.c_str());
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

    printf("Reading in pattern databases\n");
    act.start();
    
    heuristics::FilePatternDatabase fpdb(cache.getVariableCount(), pdFile);
    fpdb.initialize(spgs);
    
    act.stop();
    act.report();
    
    printf("Evaluating pattern databases\n");
    act.start();
    
    std::ifstream in(sampleFile.c_str());
    
    // make sure we found the file
    if (!in.is_open()) {
        throw std::runtime_error("Could not open the sample file: '" + sampleFile + "'");
    }
    
    std::vector<std::string> tokens;
    std::string line;
    
    // skip the first line
    std::getline(in, line);
    
    // maintain the error of each pattern database
    std::vector<float> errors;
    for (int i = 0; i < fpdb.size(); i++) {
        errors.push_back(0);
    }

    // read each sample and get the error of each pattern database
    while (!in.eof()) {
        std::getline(in, line);

        // skip empty lines and comments
        if (line.size() == 0 || line.compare(0, 1, "#") == 0) {
            continue;
        }
        
        // otherwise, assume this is a sample
        
        // split on commas
        boost::trim(line);
        boost::split(tokens, line, boost::is_any_of(","), boost::token_compress_on);
        
        varset start = stringToVarset(tokens[0]);
        varset goal = stringToVarset(tokens[1]);
        float cost = atof(tokens[2].c_str());
        
        bool complete;
        
        for (int i = 0; i < fpdb.getCount(); i++) {
            // calculate the estimate as h(goal) - h(start)
            heuristics::Heuristic *h_i = fpdb.getHeuristic(i);
            float h_goal = h_i->h(goal, complete);
            float h_start = h_i->h(start, complete);
            float h_cost = h_start - h_goal;
            
            //printf("h_cost: %f, cost: %f\n", h_cost, cost);
            
            float error = fabs(h_cost - cost);
            errors[i] += error;
        }
    }
    
    act.stop();
    act.report();
    
    FILE *out = fopen(outputFile.c_str(), "w");
    // now, print the error of each pd
    for (int i = 0; i < fpdb.getCount(); i++) {
        fprintf(out, "%d,%f\n", i, errors[i]);
    }
    fclose(out);

    for (int i = 0; i < variableCount; i++) {
        delete spgs[i];
    }

    return 0;
}

