INCLUDES = -I$(top_srcdir)/atlantik/libatlantic $(all_includes)
lib_LTLIBRARIES = libatlantikclient.la
libatlantikclient_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 3:0:2
libatlantikclient_la_LIBADD = ../libatlantic/libatlantic.la $(LIB_KIO)

libatlantikclient_la_SOURCES = atlantik_network.cpp monopdprotocol.cpp

noinst_HEADERS = atlantik_network.h monopdprotocol.h

METASOURCES = AUTO
