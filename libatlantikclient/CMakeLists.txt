########### next target ###############

set(atlantikclient_LIB_SRCS atlantik_network.cpp monopdprotocol.cpp )
ecm_qt_declare_logging_category(atlantikclient_LIB_SRCS
  HEADER libatlantikclient_debug.h
  IDENTIFIER LIBATLANTIKCLIENT_LOG
  CATEGORY_NAME atlantik.libatlantikclient
  DEFAULT_SEVERITY Warning
)
add_library(atlantikclient SHARED ${atlantikclient_LIB_SRCS})
generate_export_header(atlantikclient BASE_NAME libatlantikclient)
target_link_libraries(atlantikclient
  PUBLIC Qt5::Core Qt5::Gui atlantic
  PRIVATE Qt5::Xml Qt5::Network KF5::I18n
)
set_target_properties(atlantikclient PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS atlantikclient ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
