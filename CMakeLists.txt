cmake_minimum_required(VERSION 3.31)
project(MineWeb)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXECUTABLE_SUFFIX ".html")

if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Emscripten toolchain not loaded!")
endif()

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.hpp")
file(GLOB_RECURSE LIB_FILES "lib/*.cpp" "lib/*.hpp" "lib/*.h")
add_executable(MineWeb ${SOURCE_FILES} ${LIB_FILES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

target_link_options(MineWeb PRIVATE
        "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/src/payload.html"
        "SHELL:-s USE_WEBGL2=1"
        "SHELL:-s MIN_WEBGL_VERSION=2"
        "SHELL:-s MAX_WEBGL_VERSION=2"
        "SHELL:-s WASM=1"
        "SHELL:-s MODULARIZE=1"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s EXPORT_ES6=1"
        "SHELL:-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        "SHELL:-s NO_EXIT_RUNTIME=1"
        "SHELL:-s ASSERTIONS=1"
        "SHELL:-s USE_GLFW=3"
        "SHELL:-s STB_IMAGE=1"
        "SHELL:-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1"
        "SHELL:-s FORCE_FILESYSTEM=1"
        "SHELL:-s ASYNCIFY"
        "SHELL:-s ENVIRONMENT=web"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/assets@/assets"
        "SHELL:-s STACK_SIZE=16777216"
        "SHELL:-s INITIAL_MEMORY=134217728"
)

target_include_directories(MineWeb PRIVATE
        ${EMSCRIPTEN_ROOT_PATH}/cache/sysroot/include
)

target_link_libraries(MineWeb PRIVATE
        "-lGL"
        "-lhtml5"
        "-lwebsocket.js"
)
