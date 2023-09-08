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
    PUBLIC
        Qt${QT_MAJOR_VERSION}::Core
        Qt${QT_MAJOR_VERSION}::Gui
        atlantic
    PRIVATE
        Qt${QT_MAJOR_VERSION}::Xml
        Qt${QT_MAJOR_VERSION}::Network
        KF${KF_MAJOR_VERSION}::I18n
)
if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(atlantikclient PRIVATE Qt6::Core5Compat)
endif()

set_target_properties(atlantikclient PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS atlantikclient ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
