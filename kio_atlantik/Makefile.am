INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kio_atlantik.la

kio_atlantik_la_SOURCES = kio_atlantik.cpp
kio_atlantik_la_LIBADD = $(LIB_KIO)
kio_atlantik_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
noinst_HEADERS = kio_atlantik.h

kdelnk_DATA = atlantik.protocol
kdelnkdir = $(kde_servicesdir)

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kio_atlantik.pot
