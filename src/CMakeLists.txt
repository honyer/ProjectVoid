find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia Concurrent)


set(PROJECT_SOURCES
        res.qrc
        main.cpp
        gamewidget.cpp
        gamewidget.h
        startmenu.h
        startmenu.cpp
        qgraphicsbutton.h
        qgraphicsbutton.cpp
        weaponmod.h
        weaponmod.cpp
        shopitem.h
        shopitem.cpp
        archivemod.h
        archivemod.cpp
        gameengine.h
        gameengine.cpp
        map.h
        soundplayer.h
        globaltimer.h
        log.h
        player.h
        player.cpp
        weapon.h
        weapon.cpp
        bullet.h
        bullet.cpp
        enemy.h
        enemy.cpp
        exp.h
        exp.cpp
        heartlabel.h
        heartlabel.cpp
        upgrademod.h
        upgrademod.cpp
        gamemenu.h
        gamemenu.cpp
        gameend.h
        gameend.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ProjectVoid
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ProjectVoid APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ProjectVoid SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ProjectVoid
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ProjectVoid PRIVATE Qt${QT_VERSION_MAJOR}::Widgets 
                                       Qt${QT_VERSION_MAJOR}::Multimedia 
                                       Qt${QT_VERSION_MAJOR}::Concurrent)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ProjectVoid)
endif()
set_target_properties(ProjectVoid PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ProjectVoid
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ProjectVoid)
endif()
