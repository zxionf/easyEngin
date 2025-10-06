cmake_minimum_required(VERSION 3.15)
project(al)

set(CMAKE_CXX_STANDARD 17)

# 设置编译器标志
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# 查找依赖包
find_package(OpenGL REQUIRED)


if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif()


if (MINGW OR MSYS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${GLFW3_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})

file(GLOB_RECURSE ENGIN CONFIGURE_DEPENDS
    LIST_DIRECTORIES false
    "engin/*.cpp" "engin/2d/*.cpp"
)
file(GLOB_RECURSE REMOVE_SOURCES "engin/3d/*.*")
list(REMOVE_ITEM ENGIN ${REMOVE_SOURCES})

# 添加可执行文件
add_executable(GameEngine 
    main2d.cpp
    ${ENGIN}
)

target_compile_definitions(GameEngine PRIVATE GLEW_STATIC)

# 链接库
target_link_libraries(GameEngine 
    ${PROJECT_SOURCE_DIR}/libs/libglfw3.a
    ${PROJECT_SOURCE_DIR}/libs/glew32s.lib
    "opengl32.lib"
)

add_executable(snake snake.cpp ${ENGIN})
target_compile_definitions(snake PRIVATE GLEW_STATIC)
target_link_libraries(snake 
    ${PROJECT_SOURCE_DIR}/libs/libglfw3.a
    ${PROJECT_SOURCE_DIR}/libs/glew32s.lib
    "opengl32.lib"
)

# 复制资源文件
configure_file(assets/shaders/basic.vert ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/basic.vert COPYONLY)
configure_file(assets/shaders/basic.frag ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/basic.frag COPYONLY)