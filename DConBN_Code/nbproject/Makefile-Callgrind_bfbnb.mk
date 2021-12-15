#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Callgrind_bfbnb
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/top_p_constraint.o \
	${OBJECTDIR}/priority_queue.o \
	${OBJECTDIR}/sparse_parent_bitwise.o \
	${OBJECTDIR}/bayesian_network.o \
	${OBJECTDIR}/static_pattern_database.o \
	${OBJECTDIR}/combined_pattern_database.o \
	${OBJECTDIR}/dynamic_pattern_database.o \
	${OBJECTDIR}/sparse_parent_tree.o \
	${OBJECTDIR}/bfbnb_main.o \
	${OBJECTDIR}/file_pattern_database.o \
	${OBJECTDIR}/score_cache.o \
	${OBJECTDIR}/tarjans_algorithm.o \
	${OBJECTDIR}/sparse_parent_list.o \
	${OBJECTDIR}/hugin_structure_writer.o


# C Compiler Flags
CFLAGS=-march=native -g

# CC Compiler Flags
CCFLAGS=-std=c++0x -march=native -g
CXXFLAGS=-std=c++0x -march=native -g

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L${HOME}/local/lib -lboost_timer -lboost_program_options -lboost_system -lboost_chrono -lboost_thread -lboost_serialization

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bfbnb-callgrind

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bfbnb-callgrind: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bfbnb-callgrind ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/top_p_constraint.o: top_p_constraint.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/top_p_constraint.o top_p_constraint.cpp

${OBJECTDIR}/priority_queue.o: priority_queue.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/priority_queue.o priority_queue.cpp

${OBJECTDIR}/sparse_parent_bitwise.o: sparse_parent_bitwise.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sparse_parent_bitwise.o sparse_parent_bitwise.cpp

${OBJECTDIR}/bayesian_network.o: bayesian_network.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/bayesian_network.o bayesian_network.cpp

${OBJECTDIR}/static_pattern_database.o: static_pattern_database.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/static_pattern_database.o static_pattern_database.cpp

${OBJECTDIR}/combined_pattern_database.o: combined_pattern_database.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/combined_pattern_database.o combined_pattern_database.cpp

${OBJECTDIR}/dynamic_pattern_database.o: dynamic_pattern_database.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/dynamic_pattern_database.o dynamic_pattern_database.cpp

${OBJECTDIR}/sparse_parent_tree.o: sparse_parent_tree.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sparse_parent_tree.o sparse_parent_tree.cpp

${OBJECTDIR}/bfbnb_main.o: bfbnb_main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/bfbnb_main.o bfbnb_main.cpp

${OBJECTDIR}/file_pattern_database.o: file_pattern_database.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/file_pattern_database.o file_pattern_database.cpp

${OBJECTDIR}/score_cache.o: score_cache.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/score_cache.o score_cache.cpp

${OBJECTDIR}/tarjans_algorithm.o: tarjans_algorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/tarjans_algorithm.o tarjans_algorithm.cpp

${OBJECTDIR}/sparse_parent_list.o: sparse_parent_list.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sparse_parent_list.o sparse_parent_list.cpp

${OBJECTDIR}/hugin_structure_writer.o: hugin_structure_writer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -I${HOME}/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/hugin_structure_writer.o hugin_structure_writer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bfbnb-callgrind

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
