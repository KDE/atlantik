SUBDIRS =  libatlantic libatlantikclient libatlantikui client \
           kio_atlantik pics themes

EXTRA_DIST = atlantik.desktop

xdg_apps_DATA = atlantik.desktop

rcdir = $(kde_datadir)/atlantik
rc_DATA = atlantikui.rc eventsrc

messages: rc.cpp
	$(XGETTEXT) `find . -name '*.cpp'` -o $(podir)/atlantik.pot

