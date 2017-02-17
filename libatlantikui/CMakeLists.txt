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
ki18n_wrap_ui(atlantikui_LIB_SRCS auction.ui)
ecm_qt_declare_logging_category(atlantikui_LIB_SRCS
  HEADER libatlantikui_debug.h
  IDENTIFIER LIBATLANTIKUI_LOG
  CATEGORY_NAME atlantik.libatlantikui
  DEFAULT_SEVERITY Warning
)
add_library(atlantikui SHARED ${atlantikui_LIB_SRCS})
generate_export_header(atlantikui BASE_NAME libatlantikui)
target_link_libraries(atlantikui
  PUBLIC Qt5::Widgets atlantic
  PRIVATE KF5::I18n KF5::IconThemes KF5::WidgetsAddons
)
set_target_properties(atlantikui PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} EXPORT_NAME AtlantikUi)
target_include_directories(atlantikui INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/atlantik/ui>")
install(TARGETS atlantikui EXPORT AtlantikUiTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES
   board.h
   portfolioview.h
   tokentheme.h
   trade_widget.h
   ${CMAKE_CURRENT_BINARY_DIR}/libatlantikui_export.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/atlantik/ui)
