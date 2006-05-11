kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/atlantik/libatlantic  )


########### next target ###############

set(atlanticd_SRCS 
   atlanticclient.cpp 
   atlanticdaemon.cpp 
   main.cpp 
   serversocket.cpp )

kde4_automoc(${atlanticd_SRCS})

kde4_add_executable(atlanticd ${atlanticd_SRCS})

target_link_libraries(atlanticd  ${KDE4_KDECORE_LIBS} )

install_targets(/bin atlanticd )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#KDE_OPTIONS = qtonly
#
#bin_PROGRAMS = atlanticd
#
#INCLUDES = -I$(top_srcdir)/atlantik/libatlantic $(QT_INCLUDES)
#LDADD = ../libatlantic/libatlantic.la 
#
#atlanticd_SOURCES = atlanticclient.cpp atlanticdaemon.cpp main.cpp serversocket.cpp
#
#METASOURCES = AUTO 
