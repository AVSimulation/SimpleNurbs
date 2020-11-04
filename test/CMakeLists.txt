cmake_minimum_required(VERSION 2.8)

project(SimpleNurbsTest)

add_executable(SimpleNurbsTest
main.cpp
)

target_link_libraries(SimpleNurbsTest
	dl
	)
