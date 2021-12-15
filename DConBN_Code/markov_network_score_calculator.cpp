#include <algorithm>
#include <math.h>

#include <boost/dynamic_bitset.hpp>
#include <limits>

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "markov_network_score_calculator.h"
#include "markov_network_scoring_function.h"

scoring::MarkovNetworkScoreCalculator::MarkovNetworkScoreCalculator(scoring::MarkovNetworkScoringFunction *scoringFunction, int maxClique, int variableCount, int runningTime, scoring::Constraints *constraints) {
    this->scoringFunction = scoringFunction;
    this->maxClique = maxClique;
    this->variableCount = variableCount;
    this->runningTime = runningTime;
    this->constraints = constraints;
}

void scoring::MarkovNetworkScoreCalculator::timeout(const boost::system::error_code& /*e*/) {
    printf("Out of time\n");
    outOfTime = true;
}

void scoring::MarkovNetworkScoreCalculator::calculateScores(FloatMap &cache) {
    this->outOfTime = false;

    boost::asio::io_service io_t;
    t = new boost::asio::deadline_timer(io_t);
    if (runningTime > 0) {
        t->expires_from_now(boost::posix_time::seconds(runningTime));
        t->async_wait(boost::bind(&scoring::MarkovNetworkScoreCalculator::timeout, this, boost::asio::placeholders::error));
        boost::thread workerThread(boost::bind(&scoring::MarkovNetworkScoreCalculator::calculateScores_internal, this, boost::ref(cache)));
        io_t.run();
        workerThread.join();
        io_t.stop();
    } else {
        calculateScores_internal(cache);
    }
}

void scoring::MarkovNetworkScoreCalculator::calculateScores_internal(FloatMap &cache) {

    for (int layer = 1; layer <= maxClique && !outOfTime; layer++) {

        VARSET_NEW(clique, scoreCache.getVariableCount());
        for (int i = 0; i < layer; i++) {
            VARSET_SET(clique, i);
        }

        VARSET_NEW(max, variableCount);
        VARSET_SET(max, variableCount);

        while (VARSET_LESS_THAN(clique, max) && !outOfTime) {
            float score = scoringFunction->calculateScore(clique, cache);

#ifdef DEBUG
            printf("score, %" PRIu64 ": %f\n", clique, score);
#endif
            cache[clique] = score;

            // find the next combination
            clique = nextPermutation(clique);
        }

        if (!outOfTime) highestCompletedLayer = layer;
    }

    t->cancel();

#ifdef DEBUG
    printf("Scores\n");
    for (auto it = cache.begin(); it != cache.end(); it++) {
        printf("%" PRIu64 ": %f\n", (*it).first, (*it).second);
    }
#endif
}