include_directories( ${CMAKE_SOURCE_DIR}/libatlantic  )

########### next target ###############

set(atlantikui_LIB_SRCS 
   auction_widget.cpp 
   board.cpp 
   estatedetails.cpp 
   estateview.cpp 
   portfolioestate.cpp 
   portfolioview.cpp 
   token.cpp 
   tokentheme.cpp
   trade_widget.cpp )
ecm_qt_declare_logging_category(atlantikui_LIB_SRCS
  HEADER libatlantikui_debug.h
  IDENTIFIER LIBATLANTIKUI_LOG
  CATEGORY_NAME atlantik.libatlantikui
  DEFAULT_SEVERITY Warning
)
add_library(atlantikui SHARED ${atlantikui_LIB_SRCS})
generate_export_header(atlantikui BASE_NAME libatlantikui)
target_link_libraries(atlantikui Qt5::Widgets KF5::I18n KF5::IconThemes KF5::WidgetsAddons KF5::KDELibs4Support atlantic)
set_target_properties(atlantikui PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS atlantikui ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES
   board.h
   portfolioview.h
   tokentheme.h
   trade_widget.h
   ${CMAKE_CURRENT_BINARY_DIR}/libatlantikui_export.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/atlantik/ui)
