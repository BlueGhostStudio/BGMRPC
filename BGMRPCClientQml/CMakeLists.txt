cmake_minimum_required(VERSION 3.16)

# include(FetchContent) FetchContent_Declare( dep_cmake GIT_REPOSITORY
# https://github.com/BlueGhostStudio/BGStudio-cmake.git GIT_TAG main )
# FetchContent_MakeAvailable(dep_cmake)

include(${dep_cmake_SOURCE_DIR}/VERSION.cmake)

project(
  BGMRPCClientQml
  VERSION ${BGMRPC_VERSION}
  LANGUAGES CXX)

include(${dep_cmake_SOURCE_DIR}/BGSQml.cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(
  Qt6 6.2
  COMPONENTS Quick WebSockets Network
  REQUIRED)

qt_add_library(BGMRPCClientQml STATIC)
qt_add_qml_module(
  BGMRPCClientQml
  URI
  BGStudio.BGMRPC
  VERSION
  1.0
  OUTPUT_DIRECTORY
  BGStudio/BGMRPC
  SOURCES
  bgmrpcclientqml.cpp
  bgmrpcclientqml.h
  calling.h
  calling.cpp
  bgmrpccommonqml.h
  bgmrpccommonqml.cpp)

set_target_properties(
  BGMRPCClientQml
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

target_compile_definitions(
  BGMRPCClientQml
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(BGMRPCClientQml PRIVATE Qt6::Quick Qt6::WebSockets)

target_include_directories(BGMRPCClientQml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(ANDROID)
  FetchContent_Declare(
    android_openssl
    DOWNLOAD_EXTRACT_TIMESTAMP true
    URL https://github.com/KDAB/android_openssl/archive/refs/heads/master.zip
    # URL_HASH MD5=c97d6ad774fab16be63b0ab40f78d945 #optional
  )
  FetchContent_MakeAvailable(android_openssl)
  include(${android_openssl_SOURCE_DIR}/android_openssl.cmake)
endif()
