



########### next target ###############

set(atlantic_LIB_SRCS 
   atlantic_core.cpp 
   auction.cpp 
   configoption.cpp 
   estate.cpp 
   estategroup.cpp 
   game.cpp 
   player.cpp 
   trade.cpp )

kde4_automoc(${atlantic_LIB_SRCS})

kde4_add_library(atlantic SHARED ${atlantic_LIB_SRCS})

target_link_libraries(atlantic  ${KDE4_KDECORE_LIBS} ${QT_QT3SUPPORT_LIBRARY} )

set_target_properties(atlantic PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})
install(TARGETS atlantic  DESTINATION ${LIB_INSTALL_DIR} )


########### install files ###############

install( FILES atlantic_core.h auction.h configoption.h estate.h  	estategroup.h game.h player.h trade.h libatlantic_export.h  DESTINATION  ${INCLUDE_INSTALL_DIR}/atlantic )




#original Makefile.am contents follow:

#KDE_OPTIONS = qtonly
#
#INCLUDES = $(all_includes) 
#lib_LTLIBRARIES = libatlantic.la
#libatlantic_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 3:0:2
#libatlantic_la_LIBADD = $(LIB_QT) -lQt3Support_debug
#
#libatlantic_la_SOURCES = atlantic_core.cpp auction.cpp configoption.cpp estate.cpp \
#	estategroup.cpp game.cpp player.cpp trade.cpp
#
#libatlanticincludedir = $(includedir)/atlantic
#libatlanticinclude_HEADERS = atlantic_core.h auction.h configoption.h estate.h \
#	estategroup.h game.h player.h trade.h libatlantic_export.h
#
#METASOURCES = AUTO
