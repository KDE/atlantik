
include_directories(
${KDE4_INCLUDE_DIR} 
${QT_INCLUDES} 
${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/../libatlantic 
${CMAKE_CURRENT_SOURCE_DIR}/../libatlantikclient 
${CMAKE_CURRENT_SOURCE_DIR}/../libatlantikui 
)

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

########### next target ###############

set(atlantik_SRCS 
   atlantik.cpp 
   configdlg.cpp 
   event.cpp 
   eventlogwidget.cpp 
   main.cpp 
   monopigator.cpp 
   selectconfiguration_widget.cpp 
   selectgame_widget.cpp 
   selectserver_widget.cpp )

kde4_automoc(${atlantik_SRCS})

kde4_add_executable(atlantik ${atlantik_SRCS})

target_link_libraries(atlantik  ${KDE4_KDECORE_LIBS} atlantikui atlantikclient ${KDE4_KIO_LIBS} kdegames ${KDE4_KNOTIFYCONFIG_LIBS})

install(TARGETS atlantik  DESTINATION ${BIN_INSTALL_DIR} )


