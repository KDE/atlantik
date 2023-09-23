add_library(atlantikui)
set_target_properties(atlantikui PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
    EXPORT_NAME AtlantikUi
)

target_sources(atlantikui PRIVATE
    auction_widget.cpp
    board.cpp
    estatedetails.cpp
    estatedetailsbase.cpp
    estateview.cpp
    portfolioestate.cpp
    portfolioview.cpp
    token.cpp
    tokentheme.cpp
    trade_widget.cpp
)
ki18n_wrap_ui(atlantikui auction.ui)
ecm_qt_declare_logging_category(atlantikui
    HEADER libatlantikui_debug.h
    IDENTIFIER LIBATLANTIKUI_LOG
    CATEGORY_NAME atlantik.libatlantikui
    DEFAULT_SEVERITY Warning
    DESCRIPTION "Atlantik - UI library"
    EXPORT ATLANTIK
)
generate_export_header(atlantikui BASE_NAME libatlantikui)

target_link_libraries(atlantikui
    PUBLIC
        atlantic
        Qt6::Widgets
    PRIVATE
        KF6::I18n
        KF6::IconThemes
        KF6::WidgetsAddons
)
target_include_directories(atlantikui INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/atlantik/ui>")
install(TARGETS atlantikui EXPORT AtlantikUiTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    board.h
    portfolioview.h
    tokentheme.h
    trade_widget.h
    ${CMAKE_CURRENT_BINARY_DIR}/libatlantikui_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/atlantik/ui
)
