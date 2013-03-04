project(qbat)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(INSTALL_SHARE_PREFIX ${CMAKE_INSTALL_PREFIX}/share)
set(qbat_INSTALL_SHARE_PREFIX ${INSTALL_SHARE_PREFIX}/qbat)
set(qbat_FORMS settingsdialog.ui)
set(qbat_SOURCES_H constants.h batteryicon.h common.h powermanager.h settings.h qtimermessagebox.h)
set(qbat_TRANSLATIONS qbat_de.ts)
set(qbat_SOURCES_CPP main.cpp batteryicon.cpp powermanager.cpp common.cpp settings.cpp qtimermessagebox.cpp)

qt4_wrap_ui(qbat_FORMS_H ${qbat_FORMS})
qt4_wrap_cpp(qbat_SOURCES_MOC ${qbat_SOURCES_H})
qt4_add_translation(qbat_TRANSLATIONS_QM ${qbat_TRANSLATIONS})

add_executable(qbat ${qbat_SOURCES_CPP} ${qbat_SOURCES_MOC} ${qbat_SOURCES_H} ${qbat_FORMS_H} ${qbat_TRANSLATIONS_QM})
target_link_libraries(qbat ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

file(GLOB qbat_RES_ICONS res/*.png)

install(TARGETS qbat DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${qbat_TRANSLATIONS_QM} DESTINATION ${qbat_INSTALL_SHARE_PREFIX}/lang)
install(FILES ${qbat_RES_ICONS} DESTINATION ${qbat_INSTALL_SHARE_PREFIX}/icons)
install(FILES qbat.desktop DESTINATION ${INSTALL_SHARE_PREFIX}/applications)
