KDE_OPTIONS = qtonly

INCLUDES = $(all_includes) 
lib_LTLIBRARIES = libatlantic.la
libatlantic_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 3:0:2
libatlantic_la_LIBADD = $(LIB_QT)

libatlantic_la_SOURCES = atlantic_core.cpp auction.cpp configoption.cpp estate.cpp \
	estategroup.cpp game.cpp player.cpp trade.cpp

libatlanticincludedir = $(includedir)/atlantic
libatlanticinclude_HEADERS = atlantic_core.h auction.h configoption.h estate.h \
	estategroup.h game.h player.h trade.h libatlantic_export.h

METASOURCES = AUTO
