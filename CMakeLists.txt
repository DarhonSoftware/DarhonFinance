cmake_minimum_required(VERSION 3.22)

project(drfinance VERSION 1.5.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql PrintSupport LinguistTools)

qt_standard_project_setup(REQUIRES 6.5)

set (SOURCES
  accountedit.ui
  accountinfo.ui
  accountsview.ui
  budgetedit.ui
  budgetrep.ui
  budgetview.ui
  calendar.ui
  currencyedit.ui
  dialogicons.ui
  dr_about.ui
  exchange.ui
  generallist.ui
  groupedit.ui
  help0.ui
  itemedit.ui
  iteminfo.ui
  itemview.ui
  login.ui
  mainwindow0.ui
  purgetrans.ui
  report.ui
  richtexteditor.ui
  scheduledtrans.ui
  search.ui
  settings.ui
  splittrans.ui
  transedit.ui
  transfer.ui
  transinfo.ui
  transview.ui

  accountedit.cpp
  accountinfo.cpp
  accountsview.cpp
  amounteditor.cpp
  basedelegate.cpp
  budgetedit.cpp
  budgetrep.cpp
  budgetview.cpp
  calendar.cpp
  currencyedit.cpp
  delegatebudgetedit.cpp
  delegatetransview.cpp
  dialogicons.cpp
  dr_about.cpp
  dr_crypt.cpp
  dr_sql.cpp
  exchange.cpp
  filterparam.cpp
  formbase.cpp
  formlist.cpp
  generallist.cpp
  global.cpp
  groupedit.cpp
  help0.cpp
  itemedit.cpp
  iteminfo.cpp
  itemview.cpp
  login.cpp
  main.cpp
  mainwindow0.cpp
  purgetrans.cpp
  report.cpp
  reportdrawing.cpp
  richtexteditor.cpp
  scheduledtrans.cpp
  search.cpp
  settings.cpp
  signsymbol.cpp
  splittrans.cpp
  statuswidget.cpp
  transedit.cpp
  transfer.cpp
  transinfo.cpp
  transview.cpp
)

qt_add_resources(SOURCES drfinance.qrc)

qt_add_executable(${PROJECT_NAME}
  ${SOURCES}
)

qt_add_translations(${PROJECT_NAME}
  TS_FILES darhonfinance_es.ts
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE Qt6::Core Qt6::Widgets Qt6::Sql Qt6::PrintSupport)

set_target_properties(${PROJECT_NAME} PROPERTIES
  MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS ${PROJECT_NAME})
install(PROGRAMS pkg-info/com.darhon.drfinance.desktop  DESTINATION /app/share/applications)
install(FILES pkg-info/com.darhon.drfinance.metainfo.xml DESTINATION /app/share/metainfo)
install(FILES pkg-info/com.darhon.drfinance_16.png DESTINATION /app/share/icons/hicolor/16x16/apps RENAME com.darhon.drfinance.png)
install(FILES pkg-info/com.darhon.drfinance_24.png DESTINATION /app/share/icons/hicolor/24x24/apps RENAME com.darhon.drfinance.png)
install(FILES pkg-info/com.darhon.drfinance_32.png DESTINATION /app/share/icons/hicolor/32x32/apps RENAME com.darhon.drfinance.png)
install(FILES pkg-info/com.darhon.drfinance_48.png DESTINATION /app/share/icons/hicolor/48x48/apps RENAME com.darhon.drfinance.png)
install(FILES pkg-info/com.darhon.drfinance_64.png DESTINATION /app/share/icons/hicolor/64x64/apps RENAME com.darhon.drfinance.png)
install(FILES pkg-info/com.darhon.drfinance_128.png DESTINATION /app/share/icons/hicolor/128x128/apps RENAME com.darhon.drfinance.png)
install(FILES pkg-info/com.darhon.drfinance_256.png DESTINATION /app/share/icons/hicolor/256x256/apps RENAME com.darhon.drfinance.png)
install(FILES pkg-info/com.darhon.drfinance_512.png DESTINATION /app/share/icons/hicolor/512x512/apps RENAME com.darhon.drfinance.png)
