kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/atlantik/libatlantic ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(atlantikui_LIB_SRCS 
   auction_widget.cpp 
   board.cpp 
   estatedetails.cpp 
   estateview.cpp 
   kwrappedlistviewitem.cpp 
   portfolioestate.cpp 
   portfolioview.cpp 
   token.cpp 
   trade_widget.cpp )

kde4_automoc(${atlantikui_LIB_SRCS})

kde4_add_library(atlantikui SHARED ${atlantikui_LIB_SRCS})

target_link_libraries(atlantikui  ${QT_AND_KDECORE_LIBS} atlantic kio )

set_target_properties(atlantikui PROPERTIES VERSION 1.2.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} atlantikui )


########### install files ###############

install_files( /include/atlantik/ui FILES auction_widget.h board.h estatedetails.h  	estateview.h kwrappedlistviewitem.h portfolioestate.h  	portfolioview.h token.h trade_widget.h libatlantikui_export.h )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/atlantik/libatlantic $(all_includes)
#lib_LTLIBRARIES = libatlantikui.la
#libatlantikui_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 3:0:2
#libatlantikui_la_LIBADD = ../libatlantic/libatlantic.la $(LIB_KIO)
#
#libatlantikui_la_SOURCES = auction_widget.cpp board.cpp estatedetails.cpp \
#	estateview.cpp kwrappedlistviewitem.cpp portfolioestate.cpp \
#	portfolioview.cpp token.cpp trade_widget.cpp
#
#libatlantikuiincludedir = $(includedir)/atlantik/ui
#libatlantikuiinclude_HEADERS =  auction_widget.h board.h estatedetails.h \
#	estateview.h kwrappedlistviewitem.h portfolioestate.h \
#	portfolioview.h token.h trade_widget.h libatlantikui_export.h
#
#METASOURCES = AUTO
