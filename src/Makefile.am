bin_PROGRAMS = atlantik
INCLUDES = $(all_includes) -I$(srcdir)/../libatlantic
LDFLAGS = $(all_libraries) $(KDE_RPATH) -L../libatlantic
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

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/atlantik.pot
