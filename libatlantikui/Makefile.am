INCLUDES = -I$(top_srcdir)/atlantik/libatlantic $(all_includes)
lib_LTLIBRARIES = libatlantikui.la
libatlantikui_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 3:0:2
libatlantikui_la_LIBADD = ../libatlantic/libatlantic.la $(LIB_KIO)

libatlantikui_la_SOURCES = auction_widget.cpp board.cpp estatedetails.cpp \
	estateview.cpp kwrappedlistviewitem.cpp portfolioestate.cpp \
	portfolioview.cpp token.cpp trade_widget.cpp

libatlantikuiincludedir = $(includedir)/atlantik/ui
libatlantikuiinclude_HEADERS =  auction_widget.h board.h estatedetails.h \
	estateview.h kwrappedlistviewitem.h portfolioestate.h \
	portfolioview.h token.h trade_widget.h libatlantikui_export.h

METASOURCES = AUTO
