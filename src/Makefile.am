bin_PROGRAMS = kmonop
INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KSYCOCA) $(LIB_KFILE)

SUBDIRS =

kmonop_SOURCES = board.cpp main.cpp portfolioview.cpp estateview.cpp newgamedlg.cpp kmonop.cpp portfolioestate.cpp

KDE_ICON = kmonop

METASOURCES = AUTO 

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kmonop.pot
