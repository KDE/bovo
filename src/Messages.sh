#! /bin/sh
$EXTRACTRC `find . -name "*.kcfg"` >> rc.cpp
$XGETTEXT rc.cpp `find . -name '*.cc'` -o $podir/bovo.pot
rm -f rc.cpp

