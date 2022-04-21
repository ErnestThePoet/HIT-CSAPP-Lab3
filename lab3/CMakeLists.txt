project(blur)
aux_source_directory(./ SRCS)
add_executable(blur ${SRCS})
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fopenmp)