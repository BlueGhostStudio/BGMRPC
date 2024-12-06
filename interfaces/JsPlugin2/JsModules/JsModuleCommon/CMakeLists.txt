project(
  JsModuleCommon
  LANGUAGES CXX
  VERSION 1.0.0)

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

add_library(JsModuleCommon STATIC "")
target_sources(
  JsModuleCommon
  PRIVATE jsobjfactory.cpp
  PUBLIC jsobjfactory.h)
set_target_properties(
  JsModuleCommon
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             OUTPUT_NAME JsModuleCommon)

target_link_libraries(JsModuleCommon PRIVATE Qt${QT_VERSION_MAJOR}::Core
                                             Qt${QT_VERSION_MAJOR}::Qml)

install(TARGETS JsModuleCommon
        LIBRARY DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}
                COMPONENT BGMRPC_remote_interface_JsPlugin2_JsModule)

install(
  TARGETS JsModuleCommon
  LIBRARY DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}
          COMPONENT BGMRPC_remote_interface_JsPlugin2_JsModule_dev
          EXCLUDE_FROM_ALL
  PUBLIC_HEADER
    DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT BGMRPC_remote_interface_JsPlugin2_JsModule_dev
    EXCLUDE_FROM_ALL)
