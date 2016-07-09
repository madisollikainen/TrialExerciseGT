GIT_VERSION = $(shell git describe --abbrev=6 --dirty --always --tags)
# -DVERSION=\"$(GIT_VERSION)\"  

# Compiler #
CC 		= g++
CFLAGS	= -std=c++11 -Wall 
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


all: test 

test: ${SRC}/test.cpp ${HEADERS}/*.hpp
	${CC} ${CFLAGS} ${OPT} -o ${BUILD}/test ${SRC}/test.cpp ${OpenSSL} -I${HEADERS}

