bin_PROGRAMS = kmonop
INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes) 
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(top_builddir)/libkdegames/libkdegames.la $(LIB_KSYCOCA) $(LIB_KFILE) 

SUBDIRS =

kmonop_SOURCES = main.cpp kmonop.cpp board.cpp configdlg.cpp estateview.cpp \
monopigator.cpp network.cpp newgamedlg.cpp portfolioestate.cpp \
portfolioview.cpp token.cpp

KDE_ICON = kmonop

METASOURCES = AUTO 

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kmonop.pot
