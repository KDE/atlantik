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
kde4_add_library(atlantikui SHARED ${atlantikui_LIB_SRCS})
target_link_libraries(atlantikui ${KDE4_KDECORE_LIBS} atlantic ${KDE4_KDEUI_LIBRARY})
set_target_properties(atlantikui PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS atlantikui ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES
   board.h
   portfolioview.h
   tokentheme.h
   trade_widget.h
   libatlantikui_export.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/atlantik/ui)
