CC = g++
CFLAGS = -Wall -g -std=c++0x
LDFLAGS = -lm

all: parser

clean: 
	rm *.o parser

bayesian_network.o : bayesian_network.cpp bayesian_network.h
	${CC} ${CFLAGS} -c bayesian_network.cpp

score_cache.o : score_cache.cpp score_cache.h
	${CC} ${CFLAGS} -c score_cache.cpp

parser_main.o : parser_main.cpp bayesian_network.h score_cache.h
	${CC} ${CFLAGS} -c parser_main.cpp

parser : parser_main.o bayesian_network.o score_cache.o
	${CC} ${CFLAGS} bayesian_network.o score_cache.o parser_main.o ${LDFLAGS} -o parser
