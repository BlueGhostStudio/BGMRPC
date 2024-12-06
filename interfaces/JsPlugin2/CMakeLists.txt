cmake_minimum_required(VERSION 3.18)

include(${dep_cmake_SOURCE_DIR}/VERSION.cmake)

project(
  JsPlugin2
  LANGUAGES CXX
  VERSION ${BGMRPC_VERSION})

include(${dep_cmake_SOURCE_DIR}/BGMRPC.cmake)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Qml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Qml
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(${BGMRPCCommon_includeDir}
                    ${BGMRPCObjectInterface_includeDir})

add_library(JsPlugin2 SHARED "")
target_sources(
  JsPlugin2
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/JsPlugin2_global.h
          ${CMAKE_CURRENT_LIST_DIR}/jsengine.cpp
          ${CMAKE_CURRENT_LIST_DIR}/jsengine.h
          ${CMAKE_CURRENT_LIST_DIR}/jsobjs.cpp
          ${CMAKE_CURRENT_LIST_DIR}/jsobjs.h)
set_target_properties(
  JsPlugin2
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             OUTPUT_NAME JsPlugin2
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(
  JsPlugin2
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network
          Qt${QT_VERSION_MAJOR}::Qml BGMRPCObjectInterface BGMRPCCommon)

install(TARGETS JsPlugin2 LIBRARY DESTINATION ${BGMRPC_REMOTE}/interfaces
                                  COMPONENT BGMRPC_remote_interface_JsPlugin2)

add_subdirectory(JsModules)
