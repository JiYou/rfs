cmake_minimum_required(VERSION 3.5)
include(ExternalProject)

project (rfs)

# Add GoogleTest
ExternalProject_Add(
    google_gtest
    URL https://github.com/google/googletest/archive/release-1.8.0.zip
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/third/gtest
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_SOURCE_DIR}/build -DBUILD_SHARED_LIBS=OFF
)
add_dependencies(third google_gtest)

# Add RocksDB
ExternalProject_Add(
    rocksdb
    GIT_REPOSITORY https://github.com/facebook/rocksdb.git
    GIT_TAG v6.14.6
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/third/rocksdb
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_SOURCE_DIR}/build -DBUILD_SHARED_LIBS=OFF
)
add_dependencies(third rocksdb)

add_custom_target(third)

set(GTEST_LIB gtest_main gtest)
set(SYS_LIB pthread m)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/lib)

add_executable(unit_tests main_test.cpp)
add_dependencies(unit_tests third google_gtest)
target_link_libraries(unit_tests
    ${GTEST_LIB}
    ${SYS_LIB}
)
