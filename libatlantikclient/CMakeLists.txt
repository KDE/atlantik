add_library(atlantikclient)
set_target_properties(atlantikclient PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

target_sources(atlantikclient PRIVATE
    atlantik_network.cpp
    monopdprotocol.cpp
)

ecm_qt_declare_logging_category(atlantikclient
    HEADER libatlantikclient_debug.h
    IDENTIFIER LIBATLANTIKCLIENT_LOG
    CATEGORY_NAME atlantik.libatlantikclient
    DEFAULT_SEVERITY Warning
    DESCRIPTION "Atlantik - Client library"
    EXPORT ATLANTIK
)
generate_export_header(atlantikclient BASE_NAME libatlantikclient)

target_link_libraries(atlantikclient
    PUBLIC
        atlantic
        Qt6::Core
        Qt6::Gui
    PRIVATE
        KF6::I18n
        Qt6::Xml
        Qt6::Network
)

install(TARGETS atlantikclient ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
