# Get the git version, such that it could be 
# passed during the compilation and later used 
# as an proxy to the code version.
GIT_VERSION = $(shell git describe --abbrev=6 --dirty --always --tags)

# Main executable name, which will be used both in the 
# makefile as well as in the main function file for generating
# the usage massage.
EXE=hasher

# Compiler #
CC 		= g++
CFLAGS	= -std=c++11 -Wall -DVERSION=\"$(GIT_VERSION)\" -DEXE_NAME=\"$(EXE)\" 
OPT 	= 
# OPT 	= -O2 -g

# My directories
HEADERS	= include
SRC 	= src
BUILD   = build

# OpenSSL library linking 
# !! If OpenSSL files are 
# !! not in the path, add 
# !! the information here.
OpenSSL = -lssl -lcrypto



all: ${EXE} test 

test: ${SRC}/test.cpp ${HEADERS}/*.hpp ${BUILD}
	${CC} ${CFLAGS} ${OPT} -o ${BUILD}/test ${SRC}/test.cpp ${OpenSSL} -I${HEADERS}

${EXE}: ${SRC}/${EXE}.cpp ${HEADERS}/*.hpp ${BUILD}
	${CC} ${CFLAGS} ${OPT} -o ${BUILD}/${EXE} ${SRC}/${EXE}.cpp ${OpenSSL} -I${HEADERS}

${BUILD}:
	mkdir -p ${BUILD}


clean:
	rm ${BUILD}/*

