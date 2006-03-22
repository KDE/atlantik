kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/atlantik/libatlantic ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(atlantikclient_LIB_SRCS atlantik_network.cpp monopdprotocol.cpp )

kde4_automoc(${atlantikclient_LIB_SRCS})

kde4_add_library(atlantikclient SHARED ${atlantikclient_LIB_SRCS})

target_link_libraries(atlantikclient  ${KDE4_KDECORE_LIBS} atlantic kio )

set_target_properties(atlantikclient PROPERTIES VERSION 1.2.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} atlantikclient )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/atlantik/libatlantic $(all_includes)
#lib_LTLIBRARIES = libatlantikclient.la
#libatlantikclient_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 3:0:2
#libatlantikclient_la_LIBADD = ../libatlantic/libatlantic.la $(LIB_KIO)
#
#libatlantikclient_la_SOURCES = atlantik_network.cpp monopdprotocol.cpp
#
#noinst_HEADERS = atlantik_network.h monopdprotocol.h
#
#METASOURCES = AUTO
