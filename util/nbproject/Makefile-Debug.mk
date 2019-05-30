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
CND_PLATFORM=GNU-Linux
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
	${OBJECTDIR}/WZTKUtil/Base64.o \
	${OBJECTDIR}/WZTKUtil/Date.o \
	${OBJECTDIR}/WZTKUtil/DateTime.o \
	${OBJECTDIR}/WZTKUtil/DateTimeSpan.o \
	${OBJECTDIR}/WZTKUtil/Des.o \
	${OBJECTDIR}/WZTKUtil/File.o \
	${OBJECTDIR}/WZTKUtil/Logger.o \
	${OBJECTDIR}/WZTKUtil/NonCopyable.o \
	${OBJECTDIR}/WZTKUtil/Path.o \
	${OBJECTDIR}/WZTKUtil/String.o \
	${OBJECTDIR}/WZTKUtil/StringList.o \
	${OBJECTDIR}/WZTKUtil/Time.o \
	${OBJECTDIR}/WZTKUtil/crypt.o \
	${OBJECTDIR}/WZTKUtil/md5.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32
CXXFLAGS=-m32

# Fortran Compiler Flags
FFLAGS=-m32

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libutil.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libutil.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libutil.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libutil.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libutil.a

${OBJECTDIR}/WZTKUtil/Base64.o: WZTKUtil/Base64.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/Base64.o WZTKUtil/Base64.cpp

${OBJECTDIR}/WZTKUtil/Date.o: WZTKUtil/Date.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/Date.o WZTKUtil/Date.cpp

${OBJECTDIR}/WZTKUtil/DateTime.o: WZTKUtil/DateTime.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/DateTime.o WZTKUtil/DateTime.cpp

${OBJECTDIR}/WZTKUtil/DateTimeSpan.o: WZTKUtil/DateTimeSpan.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/DateTimeSpan.o WZTKUtil/DateTimeSpan.cpp

${OBJECTDIR}/WZTKUtil/Des.o: WZTKUtil/Des.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/Des.o WZTKUtil/Des.cpp

${OBJECTDIR}/WZTKUtil/File.o: WZTKUtil/File.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/File.o WZTKUtil/File.cpp

${OBJECTDIR}/WZTKUtil/Logger.o: WZTKUtil/Logger.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/Logger.o WZTKUtil/Logger.cpp

${OBJECTDIR}/WZTKUtil/NonCopyable.o: WZTKUtil/NonCopyable.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/NonCopyable.o WZTKUtil/NonCopyable.cpp

${OBJECTDIR}/WZTKUtil/Path.o: WZTKUtil/Path.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/Path.o WZTKUtil/Path.cpp

${OBJECTDIR}/WZTKUtil/String.o: WZTKUtil/String.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/String.o WZTKUtil/String.cpp

${OBJECTDIR}/WZTKUtil/StringList.o: WZTKUtil/StringList.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/StringList.o WZTKUtil/StringList.cpp

${OBJECTDIR}/WZTKUtil/Time.o: WZTKUtil/Time.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/Time.o WZTKUtil/Time.cpp

${OBJECTDIR}/WZTKUtil/crypt.o: WZTKUtil/crypt.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/crypt.o WZTKUtil/crypt.cpp

${OBJECTDIR}/WZTKUtil/md5.o: WZTKUtil/md5.cpp
	${MKDIR} -p ${OBJECTDIR}/WZTKUtil
	${RM} "$@.d"
	$(COMPILE.cc) -g -IWZTKUtil/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WZTKUtil/md5.o WZTKUtil/md5.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
