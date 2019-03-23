#############################################################################
#
# Project Makefile
#
# (c) Wouter van Ooijen (www.voti.nl) 2016
#
# This file is in the public domain.
# 
#############################################################################

SEARCH  += $(I2C)/code/headers
SEARCH  += $(I2C)/code/src


# source files in this library
SOURCES += $(wildcard $(I2C)/code/src/*.cpp)

# header files in this library
HEADERS += $(wildcard $(I2C)/code/headers/*.hpp)