bin_PROGRAMS = kmonop
INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KSYCOCA) $(LIB_KFILE)

SUBDIRS =

kmonop_SOURCES = main.cpp kmonop.cpp board.cpp estateview.cpp network.cpp \
newgamedlg.cpp portfolioestate.cpp portfolioview.cpp token.cpp

KDE_ICON = kmonop

METASOURCES = AUTO 

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kmonop.pot
