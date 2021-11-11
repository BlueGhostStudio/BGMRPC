project(
  BGMRPCCommon
  LANGUAGES CXX
  VERSION 2.0.0)

include(../cmake/BGMRPC.cmake)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network
  REQUIRED)
message(
  "-->Qt${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}, ${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}"
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(BGMRPCCommon SHARED "")
target_sources(
  BGMRPCCommon
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/bgmrpccommon.cpp
          ${CMAKE_CURRENT_LIST_DIR}/intbytes.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/intbytes.h ${CMAKE_CURRENT_LIST_DIR}/flags.h
         ${CMAKE_CURRENT_LIST_DIR}/bgmrpccommon.h)

set_target_properties(
  BGMRPCCommon
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             OUTPUT_NAME BGMRPCCommon
             PUBLIC_HEADER "intbytes.h;flags.h;bgmrpccommon.h"
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(BGMRPCCommon PRIVATE Qt${QT_VERSION_MAJOR}::Core
                                           Qt${QT_VERSION_MAJOR}::Network)

install(
  TARGETS BGMRPCCommon
  LIBRARY DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}
          COMPONENT BGMRPC_remote_dev
          EXCLUDE_FROM_ALL
  PUBLIC_HEADER
    DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT BGMRPC_remote_dev
    EXCLUDE_FROM_ALL)

install(TARGETS BGMRPCCommon
        LIBRARY DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}
                COMPONENT BGMRPC_remote)