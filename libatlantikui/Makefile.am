INCLUDES = -I$(top_srcdir)/atlantik/libatlantic -I$(top_srcdir)/atlantik/libatlantikclient $(all_includes)
lib_LTLIBRARIES = libatlantikui.la
libatlantikui_la_LDFLAGS = -L$(top_builddir)/atlantik/libatlantic -L$(top_builddir)/atlantik/libatlantikclient $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 0:0:0
libatlantikui_la_LIBADD = -latlantic -latlantiknetwork $(LIB_QT) $(LIB_KIO)

libatlantikui_la_SOURCES = auction_widget.cpp board.cpp display_widget.cpp \
	estateview.cpp portfolioestate.cpp portfolioview.cpp token.cpp \
	trade_widget.cpp

noinst_HEADERS = 

METASOURCES = AUTO
