bin_PROGRAMS = atlantik
INCLUDES = -I$(top_srcdir)/atlantik/libatlantic -I$(top_srcdir)/atlantik/libatlantikclient -I$(top_srcdir)/atlantik/libatlantikui $(all_includes)
LDFLAGS = -L$(top_builddir)/atlantik/libatlantic -L$(top_builddir)/atlantik/libatlantikclient -L$(top_builddir)/atlantik/libatlantikui $(all_libraries) $(KDE_RPATH)
LDADD = -latlantikui -latlantiknetwork -latlantikui -lkdegames $(LIB_KIO) 

SUBDIRS = 

atlantik_SOURCES = atlantik.cpp configdlg.cpp main.cpp monopigator.cpp \
	selectconfiguration_widget.cpp selectgame_widget.cpp \
	selectserver_widget.cpp

KDE_ICON = atlantik

METASOURCES = AUTO 

data_DATA = designerui.rc
datadir = $(kde_datadir)/atlantik
