INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KSYCOCA) $(LIB_KFILE)

SUBDIRS = src pics

EXTRA_DIST = kmonop.desktop

KDE_ICON = kmonop

appsdir = $(kde_appsdir)/Games/Board
apps_DATA = kmonop.desktop

METASOURCES = AUTO 

rcdir = $(kde_datadir)/kmonop
rc_DATA = kmonopui.rc

messages: rc.cpp
	$(XGETTEXT) rc.cpp src/*.cpp -o $(podir)/kmonop.pot
