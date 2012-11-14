#!/bin/sh
#
# Script to setup autoconf/automake build environment.
# Run this first to create the configure script

echo "Running aclocal ..." &&
aclocal --force &&

echo "Running autoheader ..." &&
autoheader --force &&

echo "Running libtool ..." &&
libtoolize --force && 

echo "Running automake ..." &&
automake --add-missing --force-missing &&

echo "Running autoconf ..." &&
autoconf --force &&

echo "You may now run ./configure"
