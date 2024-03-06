add_executable(atlantik)

target_sources(atlantik PRIVATE
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
   selectserver_widget.cpp

   atlantik.qrc
)
kconfig_add_kcfg_files(atlantik settings.kcfgc)
ki18n_wrap_ui(atlantik
    configboard.ui
    configgeneral.ui
    configmetaserver.ui
    configplayer.ui
    selectgame.ui
    selectserver.ui
)
ecm_qt_declare_logging_category(atlantik
    HEADER atlantik_debug.h
    IDENTIFIER ATLANTIK_LOG
    CATEGORY_NAME atlantik.client
    DEFAULT_SEVERITY Warning
    DESCRIPTION "Atlantik - App"
    EXPORT ATLANTIK
)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/*-apps-atlantik.png")
ecm_add_app_icon(atlantik ICONS ${ICONS_SRCS})

target_link_libraries(atlantik
    atlantic
    atlantikui
    atlantikclient
    KDEGames6
    KF6::ConfigCore
    KF6::ConfigWidgets
    KF6::CoreAddons
    KF6::Crash
    KF6::DBusAddons
    KF6::I18n
    KF6::IconThemes
    KF6::Notifications
    KF6::NotifyConfig
    KF6::XmlGui
    KF6::WidgetsAddons
    KF6::IconWidgets
)

install(TARGETS atlantik ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
