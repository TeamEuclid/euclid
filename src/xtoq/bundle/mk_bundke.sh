#!/bin/sh
#
# 'Cause xcodebuild is hard to deal with

SRCDIR=$1
BUILDDIR=$2
BUNDLE_ROOT=$3

if [ -d ${SRCDIR}/Resources ] ; then
	/usr/bin/ditto ${SRCDIR}/Resources ${BUNDLE_ROOT}/Resources
fi

if [ -d ${SRCDIR}/Resources ] ; then
	/usr/bin/ditto ${BUILDDIR}/Resources ${BUNDLE_ROOT}/Resources
fi

mkdir -p ${BUNDLE_ROOT}/Contents/MacOS

install -m 644 ${SRCDIR}/Info.plist ${BUNDLE_ROOT}/Contents
install -m 644 ${SRCDIR}/PkgInfo ${BUNDLE_ROOT}/Contents

