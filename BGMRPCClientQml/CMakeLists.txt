cmake_minimum_required(VERSION 3.18)

project(BGMRPCClientQml VERSION 1.0.0)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Qml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Qml
  REQUIRED)

set(CMAKE_AUTOMOC ON)

include(../cmake/BGSQml.cmake)

message(${BGS_QML_IMPORT_PATH})

qt_add_qml_module(
  BGMRPCClientQml
  URI
  BGStudio.BGMRPC
  VERSION
  1.0
  OUTPUT_DIRECTORY
  BGStudio/BGMRPC
  QML_FILES
  BGMRPC.qml
  NO_PLUGIN
  DESIGNER_SUPPORTED)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/BGStudio
        DESTINATION ${BGS_QML_IMPORT_PATH})
