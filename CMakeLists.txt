cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(atlantik VERSION 0.7.80)

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.84")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(GenerateExportHeader)
include(ECMQtDeclareLoggingCategory)
include(ECMInstallIcons)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(ECMAddAppIcon)


ecm_setup_version(PROJECT
   VARIABLE_PREFIX ATLANTIK
   VERSION_HEADER client/version.h
)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Gui
    Network
    Xml
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    Crash
    DBusAddons
    I18n
    IconThemes
    Notifications
    NotifyConfig
    XmlGui
    WidgetsAddons
)

find_package(KF6DocTools)

find_package(KDEGames6 6.0.0 REQUIRED)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DQT_STRICT_ITERATORS)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
add_definitions(-DTRANSLATION_DOMAIN="atlantik")
add_definitions(-DQT_DEPRECATED_WARNINGS)
add_definitions(-DKF_DEPRECATED_WARNINGS_SINCE=0x060000)
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
add_definitions(-DQT_NO_CAST_TO_ASCII)
add_definitions(-DQT_NO_CAST_FROM_ASCII)

set(GENERIC_LIB_SOVERSION "5")
set(GENERIC_LIB_VERSION "5.0.0")

add_subdirectory( libatlantic ) 
add_subdirectory( libatlantikui ) 
add_subdirectory( libatlantikclient ) 
add_subdirectory( client )
add_subdirectory( pics ) 
add_subdirectory( themes ) 
if(KF6DocTools_FOUND)
  add_subdirectory(doc)
  kdoctools_install(po)
endif()

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/Atlantik")
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/AtlantikConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/AtlantikConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

########### install files ###############

install(PROGRAMS org.kde.atlantik.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES atlantik.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/AtlantikConfig.cmake" DESTINATION "${CMAKECONFIG_INSTALL_DIR}" COMPONENT Devel)
install(EXPORT AtlanticTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE AtlanticTargets.cmake NAMESPACE Atlantik:: )
install(EXPORT AtlantikUiTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE AtlantikUiTargets.cmake NAMESPACE Atlantik:: )
install(FILES org.kde.atlantik.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_qt_install_logging_categories(
    EXPORT ATLANTIK
    FILE atlantik.categories
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)

ki18n_install(po)
