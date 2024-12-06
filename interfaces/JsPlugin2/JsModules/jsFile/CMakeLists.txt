project(
  jsFile
  LANGUAGES CXX
  VERSION 1.0.0)

include(${dep_cmake_SOURCE_DIR}/BGMRPC.cmake)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(../JsModuleCommon)

add_library(jsFile SHARED "")
target_sources(jsFile PRIVATE jsFile_global.h jsfile.cpp jsfile.h)

set_target_properties(
  jsFile
  PROPERTIES OUTPUT_NAME jsFile
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(jsFile PRIVATE Qt${QT_VERSION_MAJOR}::Core
                                     Qt${QT_VERSION_MAJOR}::Qml JsModuleCommon)

install(TARGETS jsFile LIBRARY DESTINATION ${BGMRPC_REMOTE}/JsModules
                               COMPONENT BGMRPC_remote_JsModules)
