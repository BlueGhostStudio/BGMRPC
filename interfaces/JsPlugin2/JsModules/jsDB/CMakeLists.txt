project(
  jsDB
  LANGUAGES CXX
  VERSION 1.0.0)

include(${dep_cmake_SOURCE_DIR}/BGMRPC.cmake)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Sql Qml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Sql Qml
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(../JsModuleCommon)

add_library(jsDB SHARED "")
target_sources(jsDB PRIVATE jsdb_global.h jsdb.h jsdb.cpp)

set_target_properties(
  jsDB
  PROPERTIES OUTPUT_NAME jsDB
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(
  jsDB PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql
               Qt${QT_VERSION_MAJOR}::Qml JsModuleCommon)

install(TARGETS jsDB LIBRARY DESTINATION ${BGMRPC_REMOTE}/JsModules
                             COMPONENT BGMRPC_remote_JsModules)
