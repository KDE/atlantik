bin_PROGRAMS = atlantik
INCLUDES = -I$(top_srcdir)/atlantik/libatlantic $(all_includes)
LDFLAGS = -L../libatlantic $(all_libraries) $(KDE_RPATH)
LDADD = -latlantic -lkdegames $(LIB_KSYCOCA) $(LIB_KFILE) 

SUBDIRS = 

atlantik_SOURCES = atlantik.cpp board.cpp configdlg.cpp display_widget.cpp \
	estateview.cpp main.cpp monopigator.cpp network.cpp \
	portfolioestate.cpp portfolioview.cpp \
	selectconfiguration_widget.cpp selectgame_widget.cpp \
	selectserver_widget.cpp token.cpp trade.cpp trade_widget.cpp \
	editor.cpp designer.cpp # editor

KDE_ICON = atlantik

METASOURCES = AUTO 

data_DATA = defaultcity.conf
datadir = $(kde_datadir)/atlantik

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/atlantik.pot
