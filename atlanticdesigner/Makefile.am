INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KSYCOCA) $(LIB_KFILE)

bin_PROGRAMS = atlanticdesigner

atlanticdesigner_SOURCES = main.cpp designer.cpp \
	editor.cpp

METASOURCES = AUTO 

SUBDIRS = 

EXTRA_DIST = atlanticdesigner.desktop

KDE_ICON = atlanticdesigner

appsdir = $(kde_appsdir)/Games/Board
apps_DATA = atlanticdesigner.desktop

rcdir = $(kde_datadir)/atlantik
rc_DATA = atlanticdesignerui.rc

messages: rc.cpp
	$(XGETTEXT) rc.cpp src/*.cpp -o $(podir)/atlanticdesigner.pot
