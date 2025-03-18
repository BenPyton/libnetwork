cmake_minimum_required(VERSION 3.23)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

project(libnetwork-project)

add_subdirectory(libnetwork)
add_subdirectory(sample/Test_Client)
add_subdirectory(sample/Test_Server)
