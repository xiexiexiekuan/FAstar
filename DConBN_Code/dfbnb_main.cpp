// #define DEBUG is handled in the NetBeans configuration

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <string>
#include <vector>

#include <system_error>

#include <boost/asio.hpp>
#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>

#include "typedefs.h"
#include "dfs_node.h"
#include "sparse_parent_bitwise.h"

namespace po = boost::program_options;

/**
 * The tolerance allowed for floating point precision, measured in Ulps.
 * See: http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
 */
float epsilon = -1e2;

/**
 * The score cache file.
 */
std::string scoreFile;

/**
 * The sparse parent graphs for each variable.
 */
std::vector<bestscorecalculators::SparseParentBitwise*> spgs;

/**
 * The number of variables in the problem.
 */
int variableCount;

/**
 * The currently expanding node at each layer of the search.
 */
std::vector<DFSNode*> nodeStack;
/**
 * The valid parent sets for all variables at each layer of the search.
 */
std::vector<std::vector<bitset > > valid;

/**
 * The successors of the currently expanding nodes at each layer of the search.
 */
std::vector<std::vector<DFSNode* >> successors;

/**
 * Comparator to sort the successors before expanding.
 */
CompareDFSNodeStar cmp;

/**
 * All nodes which have been generated in the search.
 */
DFSNodeMap nodes;
/**
 * The current iteration of search.
 */
int iteration;

/**
 * The repair list for the current iteration of search.
 */
DFSNodeMap toRepair;

/**
 * The score of the best network found so far.
 */
float optimal;

/**
 * A pointer to the goal node, to make checking it easy.
 */
DFSNode *goalNode;

/**
 * The total number of nodes expanded, including re-expansions.
 */
int nodesExpanded;

/**
 * The file to write the learned network.
 */
std::string netFile;

/**
 * A timer to keep track of how long the algorithm has been running.
 */
boost::asio::io_service io;

/**
 * A variable to check if the user-specified time limit has expired.
 */
bool outOfTime;

/**
 * Timer to see how long the program has been running.
 */
boost::timer::auto_cpu_timer act;

/**
 * The maximum running time for the algorithm.
 */
int runningTime;

float h(varset &U, int layer) {
    float hVal = 0;
    // for each remaining variable
    for (byte Y = 0; Y < variableCount; Y++) {
        // make sure this variable was not already present
        if (!VARSET_GET(U, Y)) continue;
        int fsb = BITSET_FIRST_SET_BIT(valid[layer][Y]);
        float bestScore = spgs[Y]->getScore(fsb);
        hVal += bestScore;
    }
    return hVal;
}

void expand(int layer) {
    if (outOfTime) {
        return;
    }

    DFSNode *U = nodeStack[layer];
    successors[layer].clear();

#ifdef DEBUG
    printf("Expanding: %s\n", U->toString().c_str());
#endif
    nodesExpanded++;

    // check if we made it to the goal
    if (layer == variableCount) {
        return;
    }

    for (byte leaf = 0; leaf < variableCount; leaf++) {
        // make sure this variable is still remaining
        if (!VARSET_GET(U->getSubnetwork(), leaf)) continue;

        // get the new variable set
        VARSET_COPY(U->getSubnetwork(), newVariables);
        VARSET_CLEAR(newVariables, leaf);

        // get BestScore(leaf, U) and calculate g
        int fsb = BITSET_FIRST_SET_BIT(valid[layer][leaf]);
        float bestScore = spgs[leaf]->getScore(fsb);
        float g = U->getG() + bestScore;

        // have we already seen this node this iteration?
        DFSNode *successor = nodes[newVariables];

        // is this the first time we've ever seen this node
        if (successor == NULL) {
            float g = std::numeric_limits<float>::max();
            float hVal = h(newVariables, layer);
            successor = new DFSNode(g, hVal, newVariables, leaf);
            nodes[newVariables] = successor;
        }

        // check if we can use this node to find a better path to the goal
        float newHExact = bestScore + successor->getHExact();
        if (U->getHExact() > newHExact) {
#ifdef DEBUG
            printf("Setting hExact: %s, newHExact: %f\n", varsetToString(U->getSubnetwork()).c_str(), newHExact);
#endif
            U->setHExact(newHExact);
        }

        // check if we need to repair this node in the next iteration
        bool seenThisIteration = (successor->getLastIterationSeen() == iteration);
        if (seenThisIteration) {
            if (g < successor->getG()) {
                float oldG = successor->getG();
                int intDiff = (*(int*) &g - *(int*) &oldG);

                if (intDiff < epsilon) {
#ifdef DEBUG
                    printf("New node to repair, g: %f, old g: %f, intDiff: %d\n", g, successor->getG(), intDiff);
#endif
                    successor->setG(g);
                    successor->setLeaf(leaf);
                    toRepair[newVariables] = successor;
                }
            }
            continue;
        }

        // check if we have found a new best path to successor
        if (g < successor->getG()) {
            successor->setG(g);
            successor->setLeaf(leaf);
        } else {
            // we have already found a better path, so just skip this
            continue;
        }

        // check if we can prune this node
        float f = successor->getH() + g;
        if (optimal <= f) {
            continue;
        }

        // so this is the first time we have seen successor this iteration
        successor->setLastIterationSeen(iteration);

        // if we make it here, then we have to expand the node
        successors[layer].push_back(successor);
    }

    std::sort(successors[layer].begin(), successors[layer].end(), cmp);

    for (auto it = successors[layer].begin(); it != successors[layer].end(); ++it) {
        DFSNode *successor = *it;
        // remove leaf as a possible parent
        for (byte Y = 0; Y < variableCount; Y++) {
            // make sure this variable is still remaining
            if (!VARSET_GET(successor->getSubnetwork(), Y)) continue;

            // update the valid parents in the next layer
            BITSET_CLEAR(valid[layer + 1][Y]);
            BITSET_OR(valid[layer + 1][Y], valid[layer][Y]);
            BITSET_AND(valid[layer + 1][Y], spgs[Y]->getUsedParents(successor->getLeaf()));
        }

        // put the node information on the stack
        nodeStack[layer + 1] = successor;
        expand(layer + 1);

        // check if we found a better path to the goal through U
        float bestScore = successor->getG() - U->getG();
        float newHExact = bestScore + successor->getHExact();
        if (U->getHExact() > newHExact) {
#ifdef DEBUG
            printf("Setting hExact: %s, newHExact: %f\n", varsetToString(U->getSubnetwork()).c_str(), newHExact);
#endif
            U->setHExact(newHExact);
        }
    }

    // check if we found a better path to the goal
    float newPath = U->getHExact() + U->getG();
#ifdef DEBUG
    printf("Found new path: %f\n", newPath);
#endif
    if (newPath < optimal) {
        optimal = newPath;
        printf("New solution: %f, nodes expanded: %d\n", newPath, nodesExpanded);
        act.report();
    }
}

/**
 * Remove all parent sets at {@code layer} that have been removed from {@code U}.
 * 
 * @param U the node (presumably, a search will start from here)
 * @param layer the layer of U
 */
void initBitsets(DFSNode *U, int layer) {
    // first, set all of the bitsets completely
    for (int X = 0; X < variableCount; X++) {
        //bitset bs = ;
        BITSET_SET_ALL(valid[layer][X]);
    }

    // find variables which have been removed from U
    for (int X = 0; X < variableCount; X++) {
        if (VARSET_GET(U->getSubnetwork(), X)) continue;

        // X has already been removed from U
        // so remove it as a possible parent from everything that remains
        for (byte Y = 0; Y < variableCount; Y++) {
            // make sure this variable is remaining
            if (!VARSET_GET(U->getSubnetwork(), Y)) continue;

            // update the valid parents for Y to not include X
            BITSET_AND(valid[layer][Y], spgs[Y]->getUsedParents(X));
        }
    }
}

/**
 * Handler when out of time.
 */
void timeout(const boost::system::error_code& /*e*/) {
    printf("Out of time\n");
    outOfTime = true;
}

/**
 * The logic of the outer loop of DFS.//DFS外循环
 */
void dfs() {

    printf("URLearning, DFBnB\n");
    printf("Dataset: '%s'\n", scoreFile.c_str());
    printf("Net file: '%s'\n", netFile.c_str());
    
    printf("Reading score cache: '%s'\n", scoreFile.c_str());
    scoring::ScoreCache cache;
    cache.read(scoreFile);
    variableCount = cache.getVariableCount();
    
    printf("Creating sparse parent graphs\n");
    for (int i = 0; i < variableCount; i++) {
        bestscorecalculators::SparseParentBitwise *spg = new bestscorecalculators::SparseParentBitwise(i, variableCount);;
        
        spg->initialize(cache);
        spgs.push_back(spg);

        nodeStack.push_back(new DFSNode());
        successors.push_back(std::vector<DFSNode*>());
    }

    printf("Creating bitset data structures\n");
    for (int i = 0; i <= variableCount; i++) {
        std::vector<bitset> vec;

        for (int j = 0; j < variableCount; j++) {
            bitset bs = BITSET_NEW(spgs[j]->size());
            vec.push_back(bs);
        }
        valid.push_back(vec);
    }

    printf("Initializing search variables\n");
    init_map(nodes);
    iteration = 1;
    VARSET_NEW(allVariables, variableCount);
    VARSET_SET_ALL(allVariables, variableCount);
    float hVal = h(allVariables, 0);
    byte leaf(0);
    DFSNode *startNode = new DFSNode(0.0f, hVal, allVariables, leaf);

    VARSET_NEW(empty, variableCount);
    goalNode = new DFSNode(std::numeric_limits<float>::max(), 0.0f, empty, leaf);
    goalNode->setHExact(0.0f);
    nodes[empty] = goalNode;

    optimal = std::numeric_limits<float>::max();

    init_map(toRepair);
    toRepair[empty] = startNode;
    nodesExpanded = 0;

    while (toRepair.size() > 0 && !outOfTime) {
        //？迭代器是对谁迭代
        printf("Beginning search iteration: %d, nodes to repair: %d, nodes expanded: %d\n", iteration, toRepair.size(), nodesExpanded);
        act.report();
        std::vector<DFSNode*> prev;
        //？当前语句干嘛用的
        for (auto it = toRepair.begin(); it != toRepair.end(); ++it) {
            prev.push_back(it->second);
        }

        // sort the previous list to expand nodes closer to the goal first
        //        std::sort(prev.begin(), prev.end(), cmp);

        toRepair.clear();

        for (auto it = prev.begin(); it != prev.end(); ++it) {
            if (outOfTime) {
                break;
            }
            
            DFSNode *n = *it;
            int layer = variableCount - n->getLayer();
            nodeStack[layer] = n;//每一层的拓展节点

            // need to adust bitset data structures 调整bitset数据结构
            initBitsets(n, layer);

            expand(layer);
        }

        iteration++;
    }

    printf("Nodes expanded: %d\n", nodesExpanded);
    
    io.stop();
}

int main(int argc, char** argv) {
    

    std::string description = std::string("Learn an optimal Bayesian network using DFBnB.  Example usage: ") + argv[0] + " iris.pss";
    po::options_description desc(description);
    
    desc.add_options()
            ("scoreFile", po::value<std::string > (&scoreFile)->required(), "The file containing the local scores in pss format. First positional argument.")
            ("runningTime,r", po::value<int> (&runningTime)->default_value(0), "The maximum running time for the algorithm.  0 means no running time.")
            ("netFile,n", po::value<std::string > (&netFile)->default_value(""), "The file to which the learned network is written.  Leave blank to not create the file.")
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

    outOfTime = false;
    
    boost::asio::deadline_timer t(io);
    if (argc > 2) {
        printf("Maximum running time: %d\n", runningTime);
        t.expires_from_now(boost::posix_time::seconds(runningTime));
        t.async_wait(timeout);
        boost::thread workerThread(dfs);
        io.run();
        workerThread.join();
    } else {
        dfs();
    }

    return 0;
}
