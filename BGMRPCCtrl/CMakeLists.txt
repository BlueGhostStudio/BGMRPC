cmake_minimum_required(VERSION 3.18)

project(
  BGMRPCCtrl
  LANGUAGES CXX
  VERSION 1.0.0)

include(../cmake/BGMRPC.cmake)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network
  REQUIRED)

if(QT_VERSION_MAJOR EQUAL 5)
  set(RUNTIME_OUTPUT_NAME "BGMRPCCtrl-qt5")
else()
  set(RUNTIME_OUTPUT_NAME "BGMRPCCtrl")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_executable(BGMRPCCtrl main.cpp)

set_target_properties(
  BGMRPCCtrl
  PROPERTIES OUTPUT_NAME ${RUNTIME_OUTPUT_NAME}
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

include_directories(${BGMRPCCommon_includeDir})

target_link_libraries(
  BGMRPCCtrl PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network
                     BGMRPCCommon)

install(
  TARGETS BGMRPCCtrl
  RUNTIME
    DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_BINDIR}
    PERMISSIONS
      SETUID
      OWNER_READ
      OWNER_WRITE
      OWNER_EXECUTE
      GROUP_READ
      GROUP_EXECUTE
      WORLD_READ
      WORLD_EXECUTE
    COMPONENT BGMRPC_remote)
