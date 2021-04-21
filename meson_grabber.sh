#!/bin/sh

ext="*.cpp"

if [ "$3" = "*.h" ]; then
	ext="*.h"
fi

if [ "$2" = "all" ]; then
	find $1 -name "$ext"
else
	find $1 -maxdepth 1 -name "$ext"
fi
