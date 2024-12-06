cmake_minimum_required(VERSION 3.18)

include(${dep_cmake_SOURCE_DIR}/VERSION.cmake)

project(BGMRPCd LANGUAGES CXX)

include(${dep_cmake_SOURCE_DIR}/BGMRPC.cmake)

add_definitions(-DVERSION="${BGMRPC_VERSION}")

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core WebSockets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core WebSockets
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_EXECUTABLE_SUFFIX "")

add_executable(
  BGMRPCd
  bgmrpc.h
  bgmrpc.cpp
  client.h
  client.cpp
  main.cpp
  objectctrl.h
  objectctrl.cpp
  serverctrl.h
  serverctrl.cpp
  ctrlbase.h
  ctrlbase.cpp)

set_target_properties(
  BGMRPCd
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             OUTPUT_NAME BGMRPCd
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

include_directories(${BGMRPCCommon_includeDir})

target_link_libraries(
  BGMRPCd PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::WebSockets
                  BGMRPCCommon)

install(TARGETS BGMRPCd
        RUNTIME DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_BINDIR}
                COMPONENT BGMRPC_remote)
