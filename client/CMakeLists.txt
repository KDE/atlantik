include_directories(
${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/../libatlantic 
${CMAKE_CURRENT_SOURCE_DIR}/../libatlantikclient 
${CMAKE_CURRENT_SOURCE_DIR}/../libatlantikui 
)

########### next target ###############

set(atlantik_SRCS 
   atlantik.cpp 
   clickablelabel.cpp
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
kconfig_add_kcfg_files(atlantik_SRCS settings.kcfgc)
ki18n_wrap_ui(atlantik_SRCS configboard.ui configgeneral.ui configmetaserver.ui configplayer.ui selectserver.ui)
ecm_qt_declare_logging_category(atlantik_SRCS
  HEADER atlantik_debug.h
  IDENTIFIER ATLANTIK_LOG
  CATEGORY_NAME atlantik.client
  DEFAULT_SEVERITY Warning
)
add_executable(atlantik ${atlantik_SRCS})
target_link_libraries(atlantik KF5::ConfigCore KF5::ConfigWidgets KF5::I18n KF5::IconThemes KF5::Notifications KF5::NotifyConfig KF5::XmlGui KF5::WidgetsAddons KF5::KDELibs4Support KF5KDEGames atlantic atlantikui atlantikclient)
install(TARGETS atlantik ${INSTALL_TARGETS_DEFAULT_ARGS})
