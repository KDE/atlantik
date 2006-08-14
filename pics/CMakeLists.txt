
add_subdirectory( board ) 
add_subdirectory( misc ) 
add_subdirectory( toolbar ) 




########### install files ###############


kde4_install_icons( ${ICON_INSTALL_DIR}   )




#original Makefile.am contents follow:

#SUBDIRS = board misc toolbar
#
#KDE_ICON = atlantik
