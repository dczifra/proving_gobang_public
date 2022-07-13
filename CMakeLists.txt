# === BUILD WITH ANALYZE ===
# cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address  -fsanitize=leak -g" -DCMAKE_C_FLAGS="-fsanitize=address  -fsanitize=leak -g" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address  -fsanitize=leak" -DCMAKE_MODULE_LINKER_FLAGS="-fsanitize=address  -fsanitize=leak"

cmake_minimum_required(VERSION 2.9)
project(AMOBA)

set(DEBUG false)

if (${DEBUG})
  set(CMAKE_CXX_FLAGS "-g")
else()
  set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_FLAGS "-O3 -ffast-math -march=native")
# -ffast-math speeds up power operations, -march=native speeds up bitcount on some systems
# -ltcmalloc gives around 10% speedup due to more efficient memory allocation, needs `google-perftools`

# === COLLECT source files === 
file(GLOB AMOBA_FILES src/*.cpp src/*.hpp)
include_directories(include)
# === Create library ===
add_library(amoba ${AMOBA_FILES})

add_executable(AMOBA test/main.cpp)

add_executable(DEV test/dev.cpp)

add_executable(AMOBA_DEBUG test/main.cpp)
#set_target_properties(AMOBA_DEBUG PROPERTIES COMPILE_FLAGS "-Wall -Wno-long-long -pedantic")
#set_target_properties(AMOBA_DEBUG PROPERTIES COMPILE_FLAGS "-g")

add_executable(TEST test/test.cpp)

file(GLOB NAUTY_FILES external/nauty27r1/nauty.c external/nauty27r1/nautil.c external/nauty27r1/naugraph.c external/nauty27r1/schreier.c external/nauty27r1/naurng.c)
add_library(nauty ${NAUTY_FILES})
add_executable(NAUTY test/nauty.cpp)

file(COPY ${CMAKE_BINARY_DIR}/../test/measure.sh  ${CMAKE_BINARY_DIR}/../test/PLAY 
        ${CMAKE_BINARY_DIR}/../test/measure_both.sh ${CMAKE_BINARY_DIR}/../test/PARALLEL.py
        ${CMAKE_BINARY_DIR}/../test/common_search.sh
  DESTINATION ${CMAKE_BINARY_DIR}
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
  GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

target_link_libraries(AMOBA amoba)
target_link_libraries(DEV amoba)
target_link_libraries(AMOBA_DEBUG amoba)
target_link_libraries(TEST amoba)
target_link_libraries(NAUTY amoba nauty)
