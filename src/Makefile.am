bin_PROGRAMS = atlantik
INCLUDES = $(all_includes) 
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KSYCOCA) $(LIB_KFILE) 

SUBDIRS =

atlantik_SOURCES = main.cpp atlantik.cpp board.cpp configdlg.cpp estateview.cpp \
monopigator.cpp network.cpp newgamedlg.cpp portfolioestate.cpp \
portfolioview.cpp token.cpp

KDE_ICON = atlantik

METASOURCES = AUTO 

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/atlantik.pot
