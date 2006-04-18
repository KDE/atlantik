kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/atlantik/libatlantic ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kio_atlantik_PART_SRCS kio_atlantik.cpp )

kde4_automoc(${kio_atlantik_PART_SRCS})

kde4_add_plugin(kio_atlantik ${kio_atlantik_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kio_atlantik )

target_link_libraries(kio_atlantik  ${KDE4_KDECORE_LIBS} kio )

install_targets(${PLUGIN_INSTALL_DIR} kio_atlantik )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES atlantik.protocol )

kde4_footer()



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
