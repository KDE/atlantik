include_directories( ${CMAKE_SOURCE_DIR}/libatlantic  )

########### next target ###############

set(atlantikclient_LIB_SRCS atlantik_network.cpp monopdprotocol.cpp )
kde4_add_library(atlantikclient SHARED ${atlantikclient_LIB_SRCS})
target_link_libraries(atlantikclient ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTNETWORK_LIBRARY} atlantic)
set_target_properties(atlantikclient PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS atlantikclient ${INSTALL_TARGETS_DEFAULT_ARGS})
