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
   trade_widget.cpp )
kde4_add_library(atlantikui SHARED ${atlantikui_LIB_SRCS})
target_link_libraries(atlantikui  ${KDE4_KDECORE_LIBS} atlantic ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS} )
set_target_properties(atlantikui PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS atlantikui  DESTINATION ${LIB_INSTALL_DIR} )

########### install files ###############

install(FILES
   board.h
   portfolioview.h
   trade_widget.h
   libatlantikui_export.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/atlantik/ui)
