bin_PROGRAMS = atlantik
INCLUDES = -I$(top_srcdir)/atlantik/libatlantic -I$(top_srcdir)/atlantik/libatlantiknetwork $(all_includes)
LDFLAGS = -L$(top_builddir)/atlantik/libatlantic -L$(top_builddir)/atlantik/libatlantiknetwork $(all_libraries) $(KDE_RPATH)
LDADD = -latlantic -latlantiknetwork -lkdegames $(LIB_KSYCOCA) $(LIB_KFILE) 

SUBDIRS = 

atlantik_SOURCES = atlantik.cpp board.cpp configdlg.cpp display_widget.cpp \
	estateview.cpp main.cpp monopigator.cpp portfolioestate.cpp \
	portfolioview.cpp selectconfiguration_widget.cpp \
	selectgame_widget.cpp selectserver_widget.cpp token.cpp \
	trade_widget.cpp editor.cpp designer.cpp

KDE_ICON = atlantik

METASOURCES = AUTO 

data_DATA = designerui.rc
datadir = $(kde_datadir)/atlantik

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/atlantik.pot
