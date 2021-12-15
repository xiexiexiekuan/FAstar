#include <cstdlib>
#include <ctime>
#include <math.h>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>  

#include "record_file.h"
#include "bayesian_network.h"
#include "variable.h"
#include "ad_node.h"
#include "ad_tree.h"
#include "markov_network_scoring_function.h"
#include "markov_network_score_calculator.h"
#include "markov_network_scoring_function_creator.h"
#include "constraints.h"

namespace po = boost::program_options;

/**
 * The file containing the data.
 */
std::string inputFile;

/**
 * The delimiter in each line.
 */
char delimiter = ',';

/**
 * The file to write the scores.
 */
std::string outputFile;

/**
 * File specifying constraints on the scores.
 */
std::string constraintsFile;

/**
 * The minimum number of records in the AD-tree.
 */
int rMin = 5;

/**
 * The scoring function to use.
 */
std::string sf = "BDeu";

/**
 * A reference to the scoring function object.
 */
scoring::MarkovNetworkScoringFunction *scoringFunction;

/**
 * The argument for the scoring function (e.g., ess for BDeu).
 */
std::string sfArgument;

/**
 * A hard limit on the size of cliques.
 */
int maxClique = -1;

/**
 * The number of threads to use.
 */
int threadCount = 1;

/**
 * The maximum amount of time to use.
 */
int runningTime = -1;

/**
 * Whether the data file has variable names in the first row.
 */
bool hasHeader = false;

/**
 * The variable information.
 */
datastructures::BayesianNetwork network;

/**
 * Constraints on the allowed scores.
 */
scoring::Constraints *constraints;

inline std::string getTime() {
        time_t now = time(0);
        tm *gmtm = gmtime(&now);
        std::string dt(asctime(gmtm));
        boost::trim(dt);
        return dt;
}

void scoringThread(int thread) {
    scoring::MarkovNetworkScoreCalculator scoreCalculator(scoringFunction, maxClique, network.size(), runningTime, constraints);

    for (int i = 0; i < threadCount; i++) {
        printf("Thread: %d, Time: %s\n", thread, getTime().c_str());

        FloatMap sc;
        init_map(sc);
        scoreCalculator.calculateScores(sc);

        //#ifdef DEBUG
        int size = sc.size();
        printf("Thread: %d, Size before pruning: %d, Time: %s\n", thread, size, getTime().c_str());
        //#endif

        //scoreCalculator.prune(sc);
        printf("Thread: %d, Score pruning is not implemented.\n", thread);


        //#ifdef DEBUG
        int prunedSize = sc.size();
        printf("Thread: %d, Size after pruning: %d, Time: %s\n", thread, prunedSize, getTime().c_str());
        //#endif

        std::string threadFilename = outputFile + "." + TO_STRING(thread);
        FILE *threadOut = fopen(threadFilename.c_str(), "w");

        for (auto score = sc.begin(); score != sc.end(); score++) {
            varset clique = (*score).first;
            float s = (*score).second;

            fprintf(threadOut, "%f ", s);

            for (int p = 0; p < network.size(); p++) {
                if (VARSET_GET(clique, p)) {
                    fprintf(threadOut, "%s ", network.get(p)->getName().c_str());
                }
            }

            fprintf(threadOut, "\n");
        }

        fprintf(threadOut, "\n");
        fclose(threadOut);

        sc.clear();
    }
}

int main(int argc, char** argv) {
    boost::timer::auto_cpu_timer t;

    std::string description = std::string("Compute the clique scores for a Markov network from a csv file.  Example usage: ") + argv[0] + " iris.csv iris.css";
    po::options_description desc(description);

    desc.add_options()
            ("input", po::value<std::string > (&inputFile)->required(), "The input file. First positional argument.")
            ("output", po::value<std::string > (&outputFile)->required(), "The output file. Second positional argument.")
            ("delimiter,d", po::value<char> (&delimiter)->required()->default_value(','), "The delimiter of the input file.")
            ("constraints,c", po::value<std::string > (&constraintsFile), "The file specifying constraints on the scores. Constraints are currently unsupported.")
            ("rMin,m", po::value<int> (&rMin)->default_value(5), "The minimum number of records in the AD-tree nodes.")
            ("function,f", po::value<std::string > (&sf)->default_value("BDeu"), "The scoring function to use.")
            ("sfArgument,a", po::value<std::string> (&sfArgument)->default_value("1.0"), "The equivalent sample size, if BDeu is used.")
            ("maxClique,m", po::value<int> (&maxClique)->default_value(0), "The maximum number of variables in any clique (inclusive). A value less than 1 means no limit.")
            ("threads,t", po::value<int> (&threadCount)->default_value(1), "The number of separate threads to use for score calculations.  Multi-threading is currently unsupported.")
            ("time,r", po::value<int> (&runningTime)->default_value(-1), "The maximum amount of time to use. A value less than 1 means no limit.")
            ("hasHeader,s", "Add this flag if the first line of the input file gives the variable names.")
            ("help,h", "Show this help message.")
            ;

    po::positional_options_description positionalOptions;
    positionalOptions.add("input", 1);
    positionalOptions.add("output", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc)
            .positional(positionalOptions).run(),
            vm);

    if (vm.count("help") || argc == 1) {
        std::cout << desc;
        return 0;
    }

    po::notify(vm);

    hasHeader = vm.count("hasHeader");

    // no multithreading right now
    threadCount = 1;
    if (threadCount < 1) {
        threadCount = 1;
    }

    printf("URLearning, Markov Network Score Calculator\n");
    printf("Input file: '%s'\n", inputFile.c_str());
    printf("Output file: '%s'\n", outputFile.c_str());
    printf("Delimiter: '%c'\n", delimiter);
    printf("Constraints file: '%s'\n", constraintsFile.c_str());
    printf("r_min: '%d'\n", rMin);
    printf("Scoring function: '%s'\n", sf.c_str());
    printf("Scoring function argument: '%s'\n", sfArgument.c_str());
    printf("Maximum clique size: '%d'\n", maxClique);
    printf("Threads: '%d'\n", threadCount);
    printf("Running time (per variable): '%d'\n", runningTime);
    printf("Has header: '%s'\n", (hasHeader ? "true" : "false"));


    printf("Parsing input file.\n");
    datastructures::RecordFile recordFile(inputFile, delimiter, hasHeader);
    recordFile.read();

    printf("Initializing data specifications.\n");
    network.initialize(recordFile);

    printf("Creating AD-tree.\n");
    scoring::ADTree *adTree = new scoring::ADTree(rMin);
    adTree->initialize(network, recordFile);
    adTree->createTree();

    if (maxClique > network.size() || maxClique < 1) {
        maxClique = network.size();
    }

    scoring::Constraints *constraints = NULL;
    if (constraintsFile.length() > 0) {
        constraints = scoring::parseConstraints(constraintsFile, network);
    }

    scoringFunction = scoring::createMarkovNetworkScoringFunction(sf, sfArgument, network, adTree, constraints);

    std::vector<boost::thread*> threads;
    for (int thread = 0; thread < threadCount; thread++) {
        boost::thread *workerThread = new boost::thread(scoringThread, thread);
        threads.push_back(workerThread);
    }

    for (auto it = threads.begin(); it != threads.end(); it++) {
        (*it)->join();
    }


    // concatenate all of the files together
    std::ofstream out(outputFile, std::ios_base::out | std::ios_base::binary);

    // first, the header information
    std::string header = "META css_version = 0.1\nMETA input_file=" + inputFile + "\nMETA num_records=" + TO_STRING(recordFile.size()) + "\n";
    header += "META clique_limit=" + TO_STRING(maxClique) + "\nMETA score_type=" + sf + "\nMETA score_argument=" + TO_STRING(sfArgument) + "\n\n";
    out.write(header.c_str(), header.size());
    
    // now, write all of the variable information
    for (int variable = 0; variable < network.size(); variable++) {

        datastructures::Variable *var = network.get(variable);
        std::string variableHeader = "VAR " + var->getName() + "\n";
        variableHeader += "META arity=" + TO_STRING(var->getCardinality()) + "\n";
        
        variableHeader += "META values=";
        for (int i = 0; i < var->getCardinality(); i++) {
            variableHeader += var->getValue(i).c_str();
            variableHeader += " ";
        }
        variableHeader += "\n";
        
        out.write(variableHeader.c_str(), variableHeader.size());
    }
    

    for (int thread = 0; thread < threadCount; thread++) {
        std::string threadFilename = outputFile + "." + TO_STRING(thread);
        std::ofstream threadFile(threadFilename, std::ios_base::in | std::ios_base::binary);

        out << threadFile.rdbuf();
        threadFile.close();

        // and remove the variable file
        remove(threadFilename.c_str());
    }

    out.close();
}
