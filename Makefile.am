INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KSYCOCA) $(LIB_KFILE)

COMPILE_FIRST = libatlantic libatlantikclient libatlantikui
SUBDIRS = $(COMPILE_FIRST) client pics

EXTRA_DIST = atlantik.desktop

KDE_ICON = atlantik

appsdir = $(kde_appsdir)/Games/Board
apps_DATA = atlantik.desktop

METASOURCES = AUTO 

rcdir = $(kde_datadir)/atlantik
rc_DATA = atlantikui.rc

messages: rc.cpp
	$(XGETTEXT) `find . -name '*.cpp'` -o $(podir)/atlantik.pot
