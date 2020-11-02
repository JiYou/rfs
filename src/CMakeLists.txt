cmake_minimum_required(VERSION 3.10)

set(CMAKE_MACOSX_RPATH 1)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../rocksdb/include)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(ROCKSDB_LIB rocksdb)

if(APPLE)
  find_library(IOKIT_LIBRARY IOKit)
  find_library(FOUNDATION_LIBRARY Foundation)
endif()

IF (WIN32)
    set(SYS_LIB
        glu32
        comctl32
        gdi32
        ole32
        setupapi
        ws2_32
        vfw32
        winmm
        opengl32
        glu32
        kernel32
        user32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
    )
ELSE ()
    set(SYS_LIB pthread m)
ENDIF()

set(SRCS
    include/rfs/rfs.h
    rfs/rfs.cc
)

add_library(rfs_lib ${SRCS})

if(APPLE)
  target_link_libraries(rfs_lib
        ${ROCKSDB_LIB}
        ${SYS_LIB}
        ${FOUNDATION_LIBRARY}
        ${IOKIT_LIBRARY}
    )
else()
  target_link_libraries(rfs_lib
        ${ROCKSDB_LIB}
        ${SYS_LIB}
    )
endif()

enable_testing()
function(rfs_test test_file)
  get_filename_component(test_target_name "${test_file}" NAME_WE)
  add_executable("${test_target_name}" ${test_file})
  add_dependencies("${test_target_name}" rfs_lib serial_lib algo_lib ${GTEST_LIB})
  target_link_libraries("${test_target_name}" rfs_lib serial_lib algo_lib ${GTEST_LIB})
  add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
endfunction(rfs_test)

function (rfs_main_test test_file)
  get_filename_component(test_target_name "${test_file}" NAME_WE)
  add_executable("${test_target_name}" ${test_file})
  add_dependencies("${test_target_name}" rfs_lib serial_lib algo_lib)
  target_link_libraries("${test_target_name}" rfs_lib serial_lib algo_lib)
  add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
endfunction(rfs_main_test)


