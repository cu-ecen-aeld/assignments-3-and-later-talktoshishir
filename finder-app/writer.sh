#!/bin/bash
# Assignment 1 script
# Author: Shishir Pandey

#set -x
#set -e

if [ $# -lt 2 ];
then
  echo Not all input provided.
  exit 1
fi

writefile=$1
writestr=$2

dirPath=$(dirname "$writefile")
mkdir -p $dirPath
echo "$writestr" > "$writefile"

if ! [ -e "$writefile" ]  && ! [ -z "$writefile"];
then
  echo File could not be created.
  exit 1
fi
