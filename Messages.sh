#! /bin/sh
$EXTRACTRC `find . -name \*.ui` >> rc.cpp || exit 11
$XGETTEXT `find . -name '*.cpp' | grep -v '/kio_atlantik/'` -o $podir/atlantik.pot
rm -f rc.cpp
