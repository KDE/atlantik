KDE_OPTIONS = qtonly

bin_PROGRAMS = atlanticd

INCLUDES = -I$(top_srcdir)/atlantik/libatlantic $(QT_INCLUDES)
LDADD = ../libatlantic/libatlantic.la 

atlanticd_SOURCES = atlanticclient.cpp atlanticdaemon.cpp main.cpp serversocket.cpp

METASOURCES = AUTO 
