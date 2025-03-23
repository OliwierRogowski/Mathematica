cmake_minimum_required(VERSION 3.16)

project(Mathematica VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Qml LinguistTools Charts Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Qml LinguistTools Charts Quick)

set(TS_FILES Mathematica_pl_PL.ts)

set(PROJECT_SOURCES
    main.cpp
    windowmathematica.cpp
    windowmathematica.h
    MainStyle.qml
    MathematicaWindowStyle.qml
    ${TS_FILES}
)



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Mathematica
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        functionobjects.h functionobjects.cpp
        functionschartspoints.h functionschartspoints.cpp
        derivativeofafunction.h derivativeofafunction.cpp
        MainStyle.qml
        MathematicaWindowStyle.qml
        Main.qrc
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Mathematica SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Mathematica
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

# Dodanie Qt6::Qml i QtCharts do linkowania
target_link_libraries(Mathematica PRIVATE Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::Charts )

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Mathematica)
endif()

set_target_properties(Mathematica PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Mathematica
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Mathematica)
endif()
