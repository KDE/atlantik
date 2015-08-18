
include_directories( ${CMAKE_SOURCE_DIR}/libatlantic  )


########### next target ###############

set(kio_atlantik_PART_SRCS kio_atlantik.cpp )

kde4_add_plugin(kio_atlantik ${kio_atlantik_PART_SRCS})



target_link_libraries(kio_atlantik  ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} )

install(TARGETS kio_atlantik  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES atlantik.protocol  DESTINATION  ${SERVICES_INSTALL_DIR} )




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/atlantik/libatlantic $(all_includes)
#METASOURCES = AUTO
#
#kde_module_LTLIBRARIES = kio_atlantik.la
#
#kio_atlantik_la_SOURCES = kio_atlantik.cpp
#kio_atlantik_la_LIBADD = $(LIB_KIO)
#kio_atlantik_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#noinst_HEADERS = kio_atlantik.h
#
#kdelnk_DATA = atlantik.protocol
#kdelnkdir = $(kde_servicesdir)
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kio_atlantik.pot
