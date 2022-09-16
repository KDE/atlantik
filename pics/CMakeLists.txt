add_subdirectory( board ) 
add_subdirectory( misc ) 
add_subdirectory( toolbar ) 

########### install files ###############

ecm_install_icons(ICONS
  16-apps-atlantik.png
  32-apps-atlantik.png
  48-apps-atlantik.png
  DESTINATION ${KDE_INSTALL_ICONDIR}
  THEME hicolor
)
