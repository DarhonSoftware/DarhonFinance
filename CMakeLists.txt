cmake_minimum_required(VERSION 3.16)

project(drfinance VERSION 1.5.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_VERSION_MAJOR 5)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Sql PrintSupport LinguistTools REQUIRED)

set(TS_FILES 
        darhonfinance_en.ts
        darhonfinance_es.ts
)

set(PROJECT_SOURCES
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

        drfinance.qrc
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(drfinance
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET drfinance APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(drfinance SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(drfinance
          ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_compile_definitions(drfinance
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(drfinance
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::PrintSupport)

set_target_properties(drfinance PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(drfinance)
    qt_finalize_executable(drfinance)
endif()

install(TARGETS drfinance)
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
