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
    KF${KF_MAJOR_VERSION}::ConfigCore
    KF${KF_MAJOR_VERSION}::ConfigWidgets
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::IconThemes
    KF${KF_MAJOR_VERSION}::Notifications
    KF${KF_MAJOR_VERSION}::NotifyConfig
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::WidgetsAddons
)
if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(atlantik
        KDEGames6
        KF6::IconWidgets
        Qt6::Core5Compat
    )
else()
    target_link_libraries(atlantik KF5KDEGames)
endif()

install(TARGETS atlantik ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
