cmake_minimum_required(VERSION 3.5)

project(GraphMaster VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)
set(PROJECT_SOURCES
        main.cpp
        MainView.cpp
        MainView.h
        GraphGraphicsView.h
        GraphGraphicsView.cpp
        TabWidget.h
        TabWidget.cpp
        TermLineEdit.h
        TermLineEdit.cpp
        FileHelper.h
        FileHelper.cpp
        PathAlgorithm.h
        PathAlgorithm.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GraphMaster
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
target_link_libraries(GraphMaster PRIVATE Qt6::Concurrent)

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GraphMaster APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GraphMaster SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GraphMaster
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(GraphMaster PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(GraphMaster PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS GraphMaster
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GraphMaster)

endif()

