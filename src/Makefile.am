bin_PROGRAMS = atlantik
INCLUDES = $(all_includes) 
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = -lkdegames $(LIB_KSYCOCA) $(LIB_KFILE) 

SUBDIRS = 

atlantik_SOURCES = main.cpp atlantik.cpp board.cpp configdlg.cpp \
estate.cpp estateview.cpp monopigator.cpp network.cpp newgamedlg.cpp \
player.cpp portfolioestate.cpp portfolioview.cpp selectgame_widget.cpp \
selectserver_widget.cpp token.cpp

KDE_ICON = atlantik

METASOURCES = AUTO 

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/atlantik.pot
