project(
  jsByteArray
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

add_library(jsByteArray SHARED "")
target_sources(jsByteArray PRIVATE jsByteArray_global.h jsbytearray.h
                                   jsbytearray.cpp)

set_target_properties(
  jsByteArray
  PROPERTIES OUTPUT_NAME jsByteArray
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(
  jsByteArray PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Qml
                      JsModuleCommon)

install(TARGETS jsByteArray LIBRARY DESTINATION ${BGMRPC_REMOTE}/JsModules
                                    COMPONENT BGMRPC_remote_JsModules)
