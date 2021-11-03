PROJECT(LIBCRPC)
include_directories(.)
set(CMAKE_VERBOSE_MAKEFILE 1)

add_definitions(
                "-W"
                "-Wall"
                #"-Werror"
                # "-Wshadow"
)
add_definitions("-g")
set(CMAKE_CXX_STANDARD 11)
file(GLOB_RECURSE native_srcs ./*.cpp ./*.cc)
ADD_LIBRARY(crpc SHARED ${native_srcs})