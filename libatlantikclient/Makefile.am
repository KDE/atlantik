INCLUDES = -I$(top_srcdir)/atlantik/libatlantic $(all_includes)
lib_LTLIBRARIES = libatlantiknetwork.la
libatlantiknetwork_la_LDFLAGS = -L$(top_builddir)/atlantik/libatlantic $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 0:0:0
libatlantiknetwork_la_LIBADD = -latlantic $(LIB_QT) $(LIB_KIO)

libatlantiknetwork_la_SOURCES = atlantik_network.cpp

noinst_HEADERS = atlantik_network.h

METASOURCES = AUTO
