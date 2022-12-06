#!/usr/bin/bash


#check if user supplied only 2 arguments
if [[ "$#" -ne 2 ]];
then
        echo "Supply 2 arguments only";
        echo "Usage: ./copy.sh <source> <destination>"; 
        exit 22; #Invalid arguments
fi

#check if arguments 1 and 2 are directories
if [[ -d $1 ]] | [[ -d $2 ]];
then
        cp -u $1/* $2 2>/dev/null; #copy only when the SOURCE file is newer than the destination file
        echo "Files copied to $2";
else
        echo "Enter correct path";
        echo "Usage: ./copy.sh <source> <destination>"; 
        exit 20; #Not a directory

fi
