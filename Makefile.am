INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KSYCOCA) $(LIB_KFILE)

SUBDIRS = src pics

EXTRA_DIST = atlantik.desktop

KDE_ICON = atlantik

appsdir = $(kde_appsdir)/Games/Board
apps_DATA = atlantik.desktop

METASOURCES = AUTO 

rcdir = $(kde_datadir)/atlantik
rc_DATA = atlantikui.rc

messages: rc.cpp
	$(XGETTEXT) rc.cpp src/*.cpp -o $(podir)/atlantik.pot

install-data-local:
	rm -rf $(kde_appsdir)/Games/Board/kmonop.desktop
