SUBDIRS =  libatlantic libatlantikclient libatlantikui client \
           kio_atlantik pics

EXTRA_DIST = atlantik.desktop

KDE_ICON = atlantik

appsdir = $(kde_appsdir)/Games/Board
apps_DATA = atlantik.desktop

rcdir = $(kde_datadir)/atlantik
rc_DATA = atlantikui.rc

messages: rc.cpp
	$(XGETTEXT) `find . -name '*.cpp'` -o $(podir)/atlantik.pot

