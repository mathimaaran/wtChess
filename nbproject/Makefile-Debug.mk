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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Service/SFController.o \
	${OBJECTDIR}/Service/SFProcessPool.o \
	${OBJECTDIR}/UI/SetFENPopup.o \
	${OBJECTDIR}/UI/TrainingWidget.o \
	${OBJECTDIR}/UI/WChessWidget.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++11
CXXFLAGS=-std=c++11

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/lib

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wtchess

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wtchess: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wtchess ${OBJECTFILES} ${LDLIBSOPTIONS} -lwt -lwthttp -l:libboost_thread.so.1.74.0 -lpthread

${OBJECTDIR}/Service/SFController.o: Service/SFController.cpp 
	${MKDIR} -p ${OBJECTDIR}/Service
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Service/SFController.o Service/SFController.cpp

${OBJECTDIR}/Service/SFProcessPool.o: Service/SFProcessPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/Service
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Service/SFProcessPool.o Service/SFProcessPool.cpp

${OBJECTDIR}/UI/SetFENPopup.o: UI/SetFENPopup.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/SetFENPopup.o UI/SetFENPopup.cpp

${OBJECTDIR}/UI/TrainingWidget.o: UI/TrainingWidget.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/TrainingWidget.o UI/TrainingWidget.cpp

${OBJECTDIR}/UI/WChessWidget.o: UI/WChessWidget.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/WChessWidget.o UI/WChessWidget.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wtchess

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
