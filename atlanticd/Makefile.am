KDE_OPTIONS = qtonly

bin_PROGRAMS = atlanticd

INCLUDES = -I$(top_srcdir)/atlantik/libatlantic $(QT_INCLUDES)
LDFLAGS = -L$(top_builddir)/atlantik/libatlantic $(X_LDFLAGS) $(QT_LDFLAGS) -R $(qt_libraries)
LDADD = -latlantic $(LIB_QT)

SUBDIRS = 

atlanticd_SOURCES = atlanticdaemon.cpp main.cpp serversocket.cpp socket.cpp

METASOURCES = AUTO 
