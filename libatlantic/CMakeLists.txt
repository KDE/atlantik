########### next target ###############

set(atlantic_LIB_SRCS 
   atlantic_core.cpp 
   auction.cpp 
   card.cpp
   configoption.cpp 
   estate.cpp 
   estategroup.cpp 
   game.cpp 
   player.cpp 
   trade.cpp )
add_library(atlantic SHARED ${atlantic_LIB_SRCS})
generate_export_header(atlantic BASE_NAME libatlantic)
target_link_libraries(atlantic
  PUBLIC Qt5::Core Qt5::Gui
)
set_target_properties(atlantic PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})
install(TARGETS atlantic ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES atlantic_core.h auction.h card.h configoption.h estate.h  	estategroup.h game.h player.h trade.h ${CMAKE_CURRENT_BINARY_DIR}/libatlantic_export.h  DESTINATION  ${INCLUDE_INSTALL_DIR}/atlantic )
