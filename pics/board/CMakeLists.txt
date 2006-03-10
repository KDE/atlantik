kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/atlantik/pics FILES arrow.png qmark-blue.png qmark-red.png token.png train.png )

kde4_footer()



#original Makefile.am contents follow:

#boardicondir = $(kde_datadir)/atlantik/pics
#boardicon_DATA = arrow.png qmark-blue.png qmark-red.png token.png train.png
