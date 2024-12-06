cmake_minimum_required(VERSION 3.18)

include(${dep_cmake_SOURCE_DIR}/VERSION.cmake)

project(BGMRPCObjectLoader LANGUAGES CXX)

include(${dep_cmake_SOURCE_DIR}/BGMRPC.cmake)

add_definitions(-DVERSION="${BGMRPC_VERSION}")

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network
  REQUIRED)

if(QT_VERSION_MAJOR EQUAL 5)
  set(RUNTIME_OUTPUT_NAME "BGMRPCObjectLoader-qt5")
else()
  set(RUNTIME_OUTPUT_NAME "BGMRPCObjectLoader")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_EXECUTABLE_SUFFIX "")

add_executable(BGMRPCObjectLoader main.cpp)

set_target_properties(
  BGMRPCObjectLoader
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             OUTPUT_NAME ${RUNTIME_OUTPUT_NAME}
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

include_directories(${BGMRPCCommon_includeDir}
                    ${BGMRPCObjectInterface_includeDir})

target_link_libraries(
  BGMRPCObjectLoader
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network
          BGMRPCCommon BGMRPCObjectInterface)

install(TARGETS BGMRPCObjectLoader
        RUNTIME DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_BINDIR}
                COMPONENT BGMRPC_remote)
