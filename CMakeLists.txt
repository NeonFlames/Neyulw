cmake_minimum_required(VERSION 3.12)
project(Neyulw VERSION 0.2.0)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
message("Found py3 at ${Python3_EXECUTABLE}")

foreach(file IN ITEMS assets/configs/ue4.yaml assets/configs/ue5.0.yaml assets/configs/ue5.5.yaml assets/IosevkaFixed/IosevkaFixed-Regular.ttf assets/IosevkaFixed/IosevkaFixed-Bold.ttf)
  get_filename_component(file_name ${file} NAME)
  message("Embedding ${file} {${CMAKE_BINARY_DIR}/${file_name}.c - ${CMAKE_BINARY_DIR}/${file_name}.h}")
  list(APPEND NYW_EMBED ${CMAKE_BINARY_DIR}/${file_name}.c)
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/${file_name}.c ${CMAKE_BINARY_DIR}/${file_name}.h
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/embed.py ${CMAKE_SOURCE_DIR}/${file} ${CMAKE_BINARY_DIR}
    DEPENDS ${CMAKE_SOURCE_DIR}/${file}
    VERBATIM
  )
endforeach()

cmake_policy(SET CMP0069 NEW)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

option(NEYULW_VENDOR_SDL "Vendor SDL3" ON)

include(CheckIPOSupported)
check_ipo_supported()

set(tfd extern/tfd/tinyfiledialogs.c)
set(imgui extern/imgui/imgui.cpp extern/imgui/imgui_widgets.cpp extern/imgui/imgui_tables.cpp extern/imgui/imgui_draw.cpp extern/imgui/misc/cpp/imgui_stdlib.cpp extern/imgui/backends/imgui_impl_sdl3.cpp extern/imgui/backends/imgui_impl_sdlrenderer3.cpp)

add_executable(Neyulw ui/main.cc ui/theme.cc ui/winit.cc ui/configure.cc ui/io.cc ${tfd} ${imgui} ${NYW_EMBED})
target_include_directories(Neyulw PRIVATE extern/imgui extern/imgui/misc/cpp extern/tfd ${CMAKE_BINARY_DIR})
set_property(TARGET Neyulw PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

set(YAML_BUILD_SHARED_LIBS OFF)
set(YAML_CPP_BUILD_TOOLS OFF)
add_subdirectory(extern/yaml-cpp)
target_link_libraries(Neyulw yaml-cpp)

if(${NEYULW_VENDOR_SDL})
  set(SDL_HAPTIC OFF)
  set(SDL_CAMERA OFF)
  set(SDL_SHARED OFF)
  set(SDL_STATIC ON)
  add_subdirectory(extern/SDL)
  target_link_libraries(Neyulw SDL3-static)
else()
  target_link_libraries(Neyulw SDL3)
endif()

