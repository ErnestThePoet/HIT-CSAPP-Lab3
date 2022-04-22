project(lab3)
aux_source_directory(./src SRCS)
add_executable(lab3 ${SRCS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fopenmp")