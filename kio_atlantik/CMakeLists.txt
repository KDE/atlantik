include_directories( ${CMAKE_SOURCE_DIR}/libatlantikclient  )

########### next target ###############

set(kio_atlantik_PART_SRCS kio_atlantik.cpp )
kde4_add_plugin(kio_atlantik ${kio_atlantik_PART_SRCS})
target_link_libraries(kio_atlantik  ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} )
install(TARGETS kio_atlantik  DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES atlantik.protocol  DESTINATION  ${SERVICES_INSTALL_DIR} )
