bin_PROGRAMS = atlantik
INCLUDES = -I$(top_srcdir)/libkdegames -I$(srcdir)/../libatlantic -I$(srcdir)/../libatlantikclient -I$(srcdir)/../libatlantikui $(all_includes) 
atlantik_LDFLAGS = $(all_libraries) $(KDE_RPATH)
atlantik_LDADD = ../libatlantikui/libatlantikui.la ../libatlantikclient/libatlantikclient.la $(LIB_KDEGAMES) $(LIB_KIO) 
atlantik_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

atlantik_SOURCES = atlantik.cpp configdlg.cpp event.cpp eventlogwidget.cpp \
	main.cpp monopigator.cpp selectconfiguration_widget.cpp \
	selectgame_widget.cpp selectserver_widget.cpp

METASOURCES = AUTO 
