include_directories(
${KDE4_INCLUDE_DIR} 
${QT_INCLUDES} 
${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/../libatlantic 
${CMAKE_CURRENT_SOURCE_DIR}/../libatlantikclient 
${CMAKE_CURRENT_SOURCE_DIR}/../libatlantikui 
)

########### next target ###############

set(atlantik_SRCS 
   atlantik.cpp 
   configdlg.cpp 
   connectioncookie.cpp
   customlocationiconbutton.cpp
   event.cpp 
   eventlogwidget.cpp 
   main.cpp 
   metatlantic.cpp
   selectconfiguration_widget.cpp 
   selectgame_widget.cpp 
   selectserver_widget.cpp )
kde4_add_kcfg_files(atlantik_SRCS settings.kcfgc)
kde4_add_ui_files(atlantik_SRCS configboard.ui configgeneral.ui configmetaserver.ui configplayer.ui)
kde4_add_executable(atlantik ${atlantik_SRCS})
target_link_libraries(atlantik  ${KDE4_KDECORE_LIBS} atlantic atlantikui atlantikclient ${KDE4_KIO_LIBS} kdegames ${KDE4_KNOTIFYCONFIG_LIBS})
install(TARGETS atlantik ${INSTALL_TARGETS_DEFAULT_ARGS})
