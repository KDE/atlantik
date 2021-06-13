#! /bin/sh
$EXTRACTRC `find . -name \*.ui -o -name \*.rc` >> rc.cpp || exit 11
$XGETTEXT `find . -name '*.cpp'` -o $podir/atlantik.pot
rm -f rc.cpp
