INCLUDES = $(all_includes) 
lib_LTLIBRARIES = libatlantic.la
libatlantic_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -version-info 0:0:0
libatlantic_la_LIBADD = $(LIB_QT)

#SOURCES = estate.cpp player.cpp
libatlantic_la_SOURCES = estate.cpp player.cpp

noinst_HEADERS = estate.h player.h

METASOURCES = AUTO 

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/atlantik.pot
