KDE_OPTIONS = qtonly

INCLUDES = $(all_includes) 
lib_LTLIBRARIES = libatlantic.la
libatlantic_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 0:0:0
libatlantic_la_LIBADD = $(LIB_QT)

libatlantic_la_SOURCES = atlantic_core.cpp auction.cpp estate.cpp \
	estategroup.cpp player.cpp trade.cpp

libatlanticincludedir = $(includedir)/atlantic
libatlanticinclude_HEADERS = atlantic_core.h auction.h estate.h \
	estategroup.h player.h trade.h

METASOURCES = AUTO 
 
