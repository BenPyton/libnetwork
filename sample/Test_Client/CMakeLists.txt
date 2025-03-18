project(sample_client CXX)

include(${CMAKE_SOURCE_DIR}/cmake/win_utils.cmake)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
	PRIVATE libnetwork
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/libnetwork/include
)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:LIBNETWORK_STATIC>
)

setup_win32_postbuild(${PROJECT_NAME})
