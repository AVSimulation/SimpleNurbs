cmake_minimum_required(VERSION 2.8)

project(SimpleNurbs)

add_library(SimpleNurbs SHARED
SimpleNurbs.cpp
SimpleNurbs.h
)

target_include_directories(SimpleNurbs PRIVATE ${CMAKE_SOURCE_DIR}/compiled/include/nurbs++)
target_link_libraries(SimpleNurbs 
	${CMAKE_SOURCE_DIR}/compiled/lib/libnurbsd.a
	${CMAKE_SOURCE_DIR}/compiled/lib/libnurbsf.a
	${CMAKE_SOURCE_DIR}/compiled/lib/libmatrixI.a
	${CMAKE_SOURCE_DIR}/compiled/lib/libmatrixN.a
	${CMAKE_SOURCE_DIR}/compiled/lib/libmatrix.a
	)

install(TARGETS SimpleNurbs DESTINATION lib)
