add_library(atlantic)
set_target_properties(atlantic PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
    EXPORT_NAME Atlantic
)

target_sources(atlantic PRIVATE
    atlantic_core.cpp
    auction.cpp
    card.cpp
    configoption.cpp
    estate.cpp
    estategroup.cpp
    game.cpp
    player.cpp
    trade.cpp
)
generate_export_header(atlantic BASE_NAME libatlantic)
target_link_libraries(atlantic
    PUBLIC
        Qt6::Core
        Qt6::Gui
)
target_include_directories(atlantic INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/atlantic>")
install(TARGETS atlantic EXPORT AtlanticTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    atlantic_core.h
    auction.h
    card.h
    configoption.h
    estate.h
    estategroup.h
    game.h
    player.h
    trade.h
    ${CMAKE_CURRENT_BINARY_DIR}/libatlantic_export.h
    DESTINATION  ${KDE_INSTALL_INCLUDEDIR}/atlantic
)
