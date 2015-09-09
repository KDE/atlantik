project(atlantik)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
find_package(KDEGames 4.9.0 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories(${KDEGAMES_INCLUDE_DIRS} ${KDE4_INCLUDES})

add_subdirectory( libatlantic ) 
add_subdirectory( libatlantikui ) 
add_subdirectory( kio_atlantik )
add_subdirectory( libatlantikclient ) 
add_subdirectory( client )
add_subdirectory( pics ) 
add_subdirectory( themes ) 
add_subdirectory( doc )

########### install files ###############

install( FILES atlantik.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES atlantikui.rc atlantik.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/atlantik )
