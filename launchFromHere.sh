#!/bin/bash
whereami=`pwd`
g++ $whereami/rogue.c -o $whereami/rogue
konsole -qwindowgeometry 1000x760+400+160 -e $whereami/rogue
