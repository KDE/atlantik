include_directories( ${CMAKE_SOURCE_DIR}/libatlantic  )

########### next target ###############

set(atlantikclient_LIB_SRCS atlantik_network.cpp monopdprotocol.cpp )
add_library(atlantikclient SHARED ${atlantikclient_LIB_SRCS})
target_link_libraries(atlantikclient Qt5::Core Qt5::Gui Qt5::Xml Qt5::Network KF5::I18n KF5::KDELibs4Support atlantic)
set_target_properties(atlantikclient PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS atlantikclient ${INSTALL_TARGETS_DEFAULT_ARGS})
