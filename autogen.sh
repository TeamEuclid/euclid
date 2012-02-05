#!/bin/sh

# Autogen script for the xtoq project. To be run in order to run
# autoreconf and automake. Automake is run simply to create files
# expected by ./configure.

autoreconf --install
automake --add-missing --copy >/dev/null 2>&1
