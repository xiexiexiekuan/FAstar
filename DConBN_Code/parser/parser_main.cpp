#include <cstdlib>

#include "score_cache.h"

int main(int argc, char** argv) {
    scoring::ScoreCache cache;
    cache.read(argv[1]);
    printf("Read in file '%s'.\n", argv[1]);
    printf("Number of variables: '%d'.\n", cache.getVariableCount());
}
