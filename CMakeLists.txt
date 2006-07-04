project(atlantik)

add_subdirectory( libatlantic ) 
add_subdirectory( libatlantikui ) 
add_subdirectory( kio_atlantik )
add_subdirectory( libatlantikclient ) 
add_subdirectory( client )
add_subdirectory( pics ) 
add_subdirectory( themes ) 




########### install files ###############

install( FILES atlantik.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES atlantikui.rc eventsrc  DESTINATION  ${DATA_INSTALL_DIR}/atlantik )




#original Makefile.am contents follow:

#SUBDIRS =  libatlantic libatlantikui \
#           kio_atlantik pics themes
#
## TODO thiago 
## SUBDIRS += libatlantikclient client 
#
#EXTRA_DIST = atlantik.desktop
#
#xdg_apps_DATA = atlantik.desktop
#
#rcdir = $(kde_datadir)/atlantik
#rc_DATA = atlantikui.rc eventsrc
#
#messages: rc.cpp
#	$(XGETTEXT) `find . -name '*.cpp'` -o $(podir)/atlantik.pot
#
