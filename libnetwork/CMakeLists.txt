project(libnetwork VERSION 0.1.0 LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/win_utils.cmake)

find_package(Threads REQUIRED)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		src/Client.cpp
		src/Event.cpp
		src/Message.cpp
		src/MsgSystem.cpp
		src/Serializer.cpp
		src/Server.cpp
		src/Socket.cpp
		src/SocketException.cpp
	PUBLIC FILE_SET HEADERS # FILE_SET requires cmake >=3.23
	BASE_DIRS ${PROJECT_SOURCE_DIR}/include
	FILES # Here list all header files that should be installed with the library
		include/Exports.h
		include/Network.h
		include/Utils.h
		include/Network/Client.h
		include/Network/Event.h
		include/Network/Message.h
		include/Network/MsgSystem.h
		include/Network/Serializer.h
		include/Network/Server.h
		include/Network/Socket.h
		include/Network/SocketException.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	PREFIX "" # Removes the first "lib" in output name, it is already in the project name.
	OUTPUT_NAME ${PROJECT_NAME}$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:-s>$<$<CONFIG:Debug,RelWithDebInfo>:-d>
	POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

target_compile_definitions(${PROJECT_NAME} 
	PRIVATE $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,LIBNETWORK_EXPORTS,LIBNETWORK_STATIC>
	PUBLIC $<$<CONFIG:Debug,RelWithDebInfo>:_DEBUG>
)

target_include_directories(${PROJECT_NAME} PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE Threads::Threads
)

if(WIN32)
	# Windows-specific socket library
	target_link_libraries(${PROJECT_NAME}
		PRIVATE Ws2_32
	)

	setup_win32_variables(${PROJECT_NAME})
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE ${OUTPUT_DIRECTORY})
	endif()
endif()

# Create all the necessary to install properly the library
# as well as making it available with find_package

set(PROJECT_TARGETS ${PROJECT_NAME}_targets)
set(PROJECT_CONFIG ${PROJECT_NAME}Config)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} 
	EXPORT ${PROJECT_TARGETS}
	FILE_SET HEADERS
	ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT  ${PROJECT_TARGETS}
	FILE ${PROJECT_TARGETS}.cmake 
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

include (CMakePackageConfigHelpers)

write_basic_package_version_file("${PROJECT_CONFIG}Version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

install(FILES "${PROJECT_CONFIG}.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG}Version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
