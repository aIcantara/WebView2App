cmake_minimum_required(VERSION 3.15)

project(WebView2App)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(VCPKG_ROOT "C:/vcpkg")

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_PREFIX_PATH "${VCPKG_ROOT}/installed/x86-windows-static")

include(cmake/CMakeRC.cmake)
include(cmake/WebView2.cmake)

cmrc_add_resource_library(WebResources)
file(GLOB_RECURSE WEB_RESOURCES "resources/*")
cmrc_add_resources(WebResources WHENCE "resources" ${WEB_RESOURCES})

setupWebView2()

find_package(cpprestsdk CONFIG REQUIRED)

add_executable(${PROJECT_NAME} WIN32
    resource.rc
    
    src/main.cpp
    src/ui/window.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${WEBVIEW2_INCLUDE_DIRS})

target_link_directories(${PROJECT_NAME} PRIVATE ${WEBVIEW2_LIB_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    WebResources
    WebView2LoaderStatic
    cpprestsdk::cpprest
    cpprestsdk::cpprestsdk_zlib_internal
    cpprestsdk::cpprestsdk_brotli_internal
)