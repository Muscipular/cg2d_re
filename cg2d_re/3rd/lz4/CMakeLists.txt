# CMake support for LZ4
#
# To the extent possible under law, the author(s) have dedicated all
# copyright and related and neighboring rights to this software to
# the public domain worldwide. This software is distributed without
# any warranty.
#
# For details, see <http://creativecommons.org/publicdomain/zero/1.0/>.


cmake_minimum_required(VERSION 3.17)



set(LZ4_TOP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")


project(LZ4 LANGUAGES C)


set(LZ4_LIB_SOURCE_DIR "${LZ4_TOP_SOURCE_DIR}/lib")

include_directories("${LZ4_LIB_SOURCE_DIR}")

file(GLOB LZ4_SOURCES
        "${LZ4_LIB_SOURCE_DIR}/*.c")

#set(STATIC_LIB_NAME lz4)
#if (MSVC AND BUILD_SHARED_LIBS)
#    set(STATIC_LIB_NAME lz4_static)
#endif ()
add_library(lz4_static STATIC ${LZ4_SOURCES})
#target_include_directories(lz4_static
#        PUBLIC $<BUILD_INTERFACE:${LZ4_LIB_SOURCE_DIR}>
#        INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

#set_target_properties(lz4_static PROPERTIES
#        OUTPUT_NAME ${STATIC_LIB_NAME}
#        POSITION_INDEPENDENT_CODE ${LZ4_POSITION_INDEPENDENT_LIB})

#target_compile_definitions(lz4_static PRIVATE XXH_NAMESPACE=LZ4_)

# Extra warning flags
if (MSVC)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /W4")
endif ()

