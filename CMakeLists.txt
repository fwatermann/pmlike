cmake_minimum_required(VERSION 3.26)
project(PM_Like)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(GLFW_INCLUDE_NONE)
add_compile_options(-lpsapi)

### TARGETS ###
if(WIN32 AND NOT UNIX)
    add_executable(PM_Like)
else()
    add_executable(PM_Like)
endif()
add_library(PM_Like_lib STATIC)

### INCLUDES ###
target_include_directories(PM_Like_lib SYSTEM PUBLIC lib/include)

target_include_directories(PM_Like SYSTEM PUBLIC lib/include)
target_include_directories(PM_Like PUBLIC src include)

### SOURCE FILES ###

# -> Executeable (PM_Like)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)
list(REMOVE_DUPLICATES SOURCE_FILES)
target_sources(PM_Like PUBLIC ${SOURCE_FILES})
message(NOTICE "Source files: ${SOURCE_FILES}")

# -> Library (PM_Like_lib)
file(GLOB_RECURSE LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/*.c)
list(REMOVE_DUPLICATES LIB_SOURCES)
target_sources(PM_Like_lib PUBLIC ${LIB_SOURCES})
message(NOTICE "Library source files: ${LIB_SOURCES}")

### LIBRARIES ###
set(LIB_BINARY_DIR lib/bin/win32)
if (WIN32)
    set(LIB_BINARY_DIR lib/bin/win32)
elseif (UNIX)
    set(LIB_BINARY_DIR lib/bin/unix)
endif ()
find_library(LIBGLFW glfw3 PATHS ${LIB_BINARY_DIR} REQUIRED)
find_library(LIBFASTNOISE2 FastNoise PATHS ${LIB_BINARY_DIR} REQUIRED)

### LINKING ###
target_link_libraries(PM_Like ${LIBGLFW} ${LIBFASTNOISE2} PM_Like_lib)

### ASSETS & SHADERS ###
list(APPEND COPY_DIRS assets shader)
foreach (DIR ${COPY_DIRS})
    file(GLOB_RECURSE FILES ${DIR}/*)
    foreach (FILE ${FILES})
        file(RELATIVE_PATH rel ${CMAKE_SOURCE_DIR} ${FILE})
        get_filename_component(dir rel DIRECTORY)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dir})
        configure_file(${FILE} ${CMAKE_CURRENT_BINARY_DIR}/${rel} COPYONLY)
        install(FILES ${FILE} DESTINATION ./${rel})
    endforeach ()
endforeach ()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

