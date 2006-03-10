kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(atlantik_SRCS 
   atlantik.cpp 
   configdlg.cpp 
   event.cpp 
   eventlogwidget.cpp 
   main.cpp 
   monopigator.cpp 
   selectconfiguration_widget.cpp 
   selectgame_widget.cpp 
   selectserver_widget.cpp )

kde4_automoc(${atlantik_SRCS})

kde4_add_executable(atlantik ${atlantik_SRCS})

target_link_libraries(atlantik  ${QT_AND_KDECORE_LIBS} atlantikui atlantikclient kio )

install_targets(/bin atlantik )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#bin_PROGRAMS = atlantik
#INCLUDES = -I$(top_srcdir)/libkdegames -I$(srcdir)/../libatlantic -I$(srcdir)/../libatlantikclient -I$(srcdir)/../libatlantikui $(all_includes) 
#atlantik_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#atlantik_LDADD = ../libatlantikui/libatlantikui.la ../libatlantikclient/libatlantikclient.la $(LIB_KDEGAMES) $(LIB_KIO) 
#atlantik_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#
#atlantik_SOURCES = atlantik.cpp configdlg.cpp event.cpp eventlogwidget.cpp \
#	main.cpp monopigator.cpp selectconfiguration_widget.cpp \
#	selectgame_widget.cpp selectserver_widget.cpp
#
#METASOURCES = AUTO 
