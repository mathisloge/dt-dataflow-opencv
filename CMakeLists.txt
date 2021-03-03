cmake_minimum_required(VERSION 3.15)
project(OpenCVPlugin LANGUAGES CXX)

find_package(Corrade REQUIRED PluginManager)
find_package(Magnum REQUIRED GL)
find_package(imgui CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(DtDataFlow CONFIG REQUIRED)

set(SOURCES         
    src/plugin.cpp
    src/slots/mat.cpp
    src/nodes/mat_copy.cpp
    src/nodes/mat_output.cpp
    src/nodes/mat_display.cpp
    src/nodes/video_capture.cpp
)

set(DTDF_PLUGIN_INSTALL_DBG_DIR ${CMAKE_INSTALL_PREFIX}/bin/dataflow/dbg)
set(DTDF_PLUGIN_INSTALL_REL_DIR ${CMAKE_INSTALL_PREFIX}/bin/dataflow/rel)
message(STATUS "Plugin DBG Install dir: ${DTDF_PLUGIN_INSTALL_DBG_DIR}")
message(STATUS "Plugin REL Install dir: ${DTDF_PLUGIN_INSTALL_REL_DIR}")
corrade_add_plugin(OpenCVPlugin 
    "${DTDF_PLUGIN_INSTALL_DBG_DIR}"
    "${DTDF_PLUGIN_INSTALL_REL_DIR}"
    OpenCVPlugin.conf
    ${SOURCES}
)
set_property(TARGET OpenCVPlugin PROPERTY CXX_STANDARD 20)

target_include_directories(
    OpenCVPlugin PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_include_directories(OpenCVPlugin PRIVATE ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(OpenCVPlugin PRIVATE 
    Corrade::PluginManager
    imgui::imgui
    Magnum::Magnum
    Magnum::GL
    dt::imnodes
    dt::DtDataflowCore
    dt::DtDataflowPlugin
    ${OpenCV_LIBS}
)
