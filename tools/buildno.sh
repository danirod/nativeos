#!/bin/sh

if ! git status >/dev/null 2>/dev/null ; then
	exit 1
fi

git log --oneline --first-parent | wc -l | awk '{ print "Build " $1 }'
