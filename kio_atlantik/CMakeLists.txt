include_directories( ${CMAKE_SOURCE_DIR}/libatlantikclient  )

########### next target ###############

set(kio_atlantik_PART_SRCS kio_atlantik.cpp )
add_library(kio_atlantik MODULE ${kio_atlantik_PART_SRCS})
target_link_libraries(kio_atlantik Qt5::Core KF5::KIOCore)
set_target_properties(kio_atlantik PROPERTIES OUTPUT_NAME "atlantik")
install(TARGETS kio_atlantik DESTINATION ${PLUGIN_INSTALL_DIR}/kf5/kio)

########### install files ###############

install( FILES atlantik.protocol  DESTINATION  ${SERVICES_INSTALL_DIR} )
