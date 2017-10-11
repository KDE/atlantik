project(atlantik)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

find_package(ECM 5.14.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(GenerateExportHeader)
include(ECMQtDeclareLoggingCategory)
include(ECMInstallIcons)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMAddAppIcon)

ecm_setup_version(0.7.80
   VARIABLE_PREFIX ATLANTIK
   VERSION_HEADER client/version.h
)

find_package(Qt5 CONFIG REQUIRED Core Gui Network Xml Widgets)

find_package(KF5 5.7.0 REQUIRED
  Config
  ConfigWidgets
  CoreAddons
  Crash
  DocTools
  I18n
  IconThemes
  KIO
  Notifications
  NotifyConfig
  XmlGui
  WidgetsAddons
)

find_package(KF5 REQUIRED KDEGames)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DQT_STRICT_ITERATORS)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
add_definitions(-DTRANSLATION_DOMAIN="atlantik")
add_definitions(-DQT_DEPRECATED_WARNINGS)
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
add_definitions(-DQT_NO_CAST_TO_ASCII)
add_definitions(-DQT_NO_CAST_FROM_ASCII)

set(GENERIC_LIB_SOVERSION "5")
set(GENERIC_LIB_VERSION "5.0.0")

add_subdirectory( libatlantic ) 
add_subdirectory( libatlantikui ) 
add_subdirectory( kio_atlantik )
add_subdirectory( libatlantikclient ) 
add_subdirectory( client )
add_subdirectory( pics ) 
add_subdirectory( themes ) 
add_subdirectory( doc )

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/Atlantik")
ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/AtlantikConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/AtlantikConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

########### install files ###############

install(PROGRAMS org.kde.atlantik.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES atlantikui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/atlantik)
install(FILES atlantik.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR})
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/AtlantikConfig.cmake" DESTINATION "${CMAKECONFIG_INSTALL_DIR}" COMPONENT Devel)
install(EXPORT AtlanticTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE AtlanticTargets.cmake NAMESPACE Atlantik:: )
install(EXPORT AtlantikUiTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE AtlantikUiTargets.cmake NAMESPACE Atlantik:: )
install(FILES atlantik.categories DESTINATION ${KDE_INSTALL_CONFDIR})
install(FILES org.kde.atlantik.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
