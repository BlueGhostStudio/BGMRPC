cmake_minimum_required(VERSION 3.14)

include(${dep_cmake_SOURCE_DIR}/VERSION.cmake)

project(
  MissingObjectHandler
  LANGUAGES CXX
  VERSION ${BGMRPC_VERSION})

include(${dep_cmake_SOURCE_DIR}/BGMRPC.cmake)

# set(CMAKE_AUTOUIC ON) set(CMAKE_AUTOMOC ON) set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 17) set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(${BGMRPCCommon_includeDir}
                    ${BGMRPCObjectInterface_includeDir})

add_library(
  MissingObjectHandler SHARED MissingObjectHandler_global.h
                              missingobjecthandler.cpp missingobjecthandler.h)

set_target_properties(
  MissingObjectHandler
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             OUTPUT_NAME MissingObjectHandler
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(
  MissingObjectHandler
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network
          BGMRPCObjectInterface BGMRPCCommon)

install(TARGETS MissingObjectHandler
        LIBRARY DESTINATION ${BGMRPC_REMOTE}/interfaces
                COMPONENT BGMRPC_remote_interface_MissingObjectHandler)

target_compile_definitions(MissingObjectHandler
                           PRIVATE MISSINGOBJECTHANDLER_LIBRARY)
