cmake_minimum_required(VERSION 3.9)
project(sec21.starter
        VERSION 0.0.1
        DESCRIPTION "Portal to sec21"
        )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Boost 1.70 REQUIRED)

set(SOURCES
        src/main.cpp
        src/window.cpp
        src/window.h
        src/generic_event.h
)
qt_add_resources(SOURCES src/resource.qrc)
qt_wrap_ui(SOURCES window.ui)

add_executable(${PROJECT_NAME} ${SOURCES})

if (EXISTS "${PROJECT_SOURCE_DIR}/test-token.stk")
    configure_file(${PROJECT_SOURCE_DIR}/test-token.stk ${PROJECT_BINARY_DIR}/test-token.stk @ONLY)
endif ()

configure_file(${PROJECT_SOURCE_DIR}/src/config.h.in ${PROJECT_BINARY_DIR}/config.h @ONLY)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_NAME})

include(cmake/StaticAnalyzers.cmake)

## https://blog.kitware.com/static-checks-with-cmake-cdash-iwyu-clang-tidy-lwyu-cpplint-and-cppcheck/
if (CMAKE_LINK_WHAT_YOU_USE)
    message(STATUS "*link what you use* is enabled")
endif ()

include(CTest)
if (BUILD_TESTING) # option is created by CTest
    enable_testing()
    add_subdirectory(tests)
endif ()

## installation
##
# include(GNUInstallDirs)
## todo
