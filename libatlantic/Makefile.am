INCLUDES = $(all_includes) 
lib_LTLIBRARIES = libatlantic.la
libatlantic_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 0:0:0
libatlantic_la_LIBADD = $(LIB_QT)

libatlantic_la_SOURCES = atlantic_core.cpp estate.cpp player.cpp trade.cpp

noinst_HEADERS = atlantic_core.h estate.h player.h trade.h

METASOURCES = AUTO 

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/libatlantic.pot
