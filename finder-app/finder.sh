#!/bin/bash
# Assignment 1 script
# Author: Shishir Pandey

set -e

if [ $# -lt 2 ];
then
  echo Not all input provided.
  exit 1
fi

filesdir=$1
searchstr=$2


if ! [ -d "$filesdir" ];
then
  echo "There is no directory with this path $1"
  exit 1
else
   X=$(find "$filesdir" -type f | wc -l)
   Y=$(grep "$searchstr" -r "$filesdir" | wc -l)
   echo The number of files are $X and the number of matching lines are $Y.
fi
