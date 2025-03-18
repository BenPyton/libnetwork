cmake_policy(SET CMP0140 NEW)

function(setup_win32_variables PROJECT)
	if(WIN32)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(COMPILER_ARCHITECTURE x64)
		else()
			set(COMPILER_ARCHITECTURE Win32)
		endif()

		if(DEFINED CMAKE_CXX_COMPILER_VERSION)
			set(COMPILER_FULLNAME ${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION})
		else()
			set(COMPILER_FULLNAME ${CMAKE_CXX_COMPILER_ID})
		endif()

		set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/${COMPILER_FULLNAME}/${PROJECT}/${COMPILER_ARCHITECTURE}/${CMAKE_BUILD_TYPE}")
	endif()
	return(PROPAGATE COMPILER_ARCHITECTURE COMPILER_FULLNAME OUTPUT_DIRECTORY)
endfunction()

# You can set the PACKAGE option to zip the output directory for a Release build
function(setup_win32_postbuild PROJECT)
	set(options PACKAGING)
	cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

	if(WIN32)
		setup_win32_variables(${PROJECT})

		set_target_properties(${PROJECT} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
		)

		# Remove console window for release builds
		if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
			#target_link_options(${PROJECT} PRIVATE "-mwindows")
		endif()

		get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)

		if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
			set(COMPILER_DLLS_TO_COPY
				\"${COMPILER_DIR}/libstdc++-6.dll\"
				\"${COMPILER_DIR}/libgcc_s_seh-1.dll\"
				\"${COMPILER_DIR}/libwinpthread-1.dll\"
			)
		endif()

		add_custom_command(
			TARGET ${PROJECT}
			POST_BUILD
			# Multifile copy requires CMake >= 3.5
			COMMAND ${CMAKE_COMMAND} -E copy $<$<BOOL:${BUILD_SHARED_LIBS}>:$<TARGET_FILE:libnetwork>> ${COMPILER_DLLS_TO_COPY} $<TARGET_FILE_DIR:${PROJECT}>
			COMMENT "POST BUILD: Copying dlls in the output directory."
		)
		
		if(${ARG_PACKAGING} AND (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
			add_custom_command(
				TARGET ${PROJECT}
				POST_BUILD
				WORKING_DIRECTORY "${OUTPUT_DIRECTORY}"
				COMMAND ${CMAKE_COMMAND} -E make_directory \"${CMAKE_SOURCE_DIR}/packaged\"
				COMMAND ${CMAKE_COMMAND} -E tar \"cfv\" \"${CMAKE_SOURCE_DIR}/packaged/${PROJECT}-${PROJECT_VERSION}-${COMPILER_ARCHITECTURE}.zip\" --format=zip -- "."
				COMMENT "POST BUILD: Packaging the release."
			)
		endif()
	endif()
endfunction()
