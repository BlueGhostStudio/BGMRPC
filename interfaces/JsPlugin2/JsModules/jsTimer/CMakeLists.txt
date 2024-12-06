project(
  jsTimer
  LANGUAGES CXX
  VERSION 1.0.0)

# include(${CMAKE_CURRENT_LIST_DIR})
include(${dep_cmake_SOURCE_DIR}/BGMRPC.cmake)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Qml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Qml
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(../JsModuleCommon)

add_library(jsTimer SHARED "")
target_sources(jsTimer PRIVATE jstimer_global.h jstimer.h jstimer.cpp)

set_target_properties(
  jsTimer
  PROPERTIES OUTPUT_NAME jsTimer
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(jsTimer PRIVATE Qt${QT_VERSION_MAJOR}::Core
                                      Qt${QT_VERSION_MAJOR}::Qml JsModuleCommon)

install(TARGETS jsTimer LIBRARY DESTINATION ${BGMRPC_REMOTE}/JsModules
                                COMPONENT BGMRPC_remote_JsModules)
