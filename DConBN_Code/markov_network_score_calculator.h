#ifndef MARKOV_NETWORK_SCORE_CALCULATOR_H
#define	MARKOV_NETWORK_SCORE_CALCULATOR_H

#include <boost/asio.hpp>

#include "typedefs.h"

namespace scoring {
    
    class Constraints;
    class MarkovNetworkScoringFunction;

    class MarkovNetworkScoreCalculator {
    public:
        MarkovNetworkScoreCalculator(scoring::MarkovNetworkScoringFunction *scoringFunction, int maxClique, int variableCount, int runningTime, Constraints *constraints);

        ~MarkovNetworkScoreCalculator() {
            // no pointers that are not deleted elsewhere
            // these could possibly be "smart pointers" or something...
        }
        
        void calculateScores(FloatMap &cache);

    private:
        void calculateScores_internal(FloatMap &cache);
        void timeout(const boost::system::error_code& /*e*/);
        
        int highestCompletedLayer;
        
        /**
         * A timer to keep track of how long the algorithm has been running.
         */
        boost::asio::io_service io;
        boost::asio::deadline_timer *t;

        /**
         * A variable to check if the user-specified time limit has expired.
         */
        bool outOfTime;
        
        MarkovNetworkScoringFunction *scoringFunction;
        int maxClique;
        int variableCount;
        int runningTime;
        Constraints *constraints;
    };

}


#endif	/* MARKOV_NETWORK_SCORE_CALCULATOR_H */

