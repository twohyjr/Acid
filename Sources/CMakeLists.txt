add_library(Acid)
add_library(Acid::Acid ALIAS Acid)

# This file is quite big, so for readability we include instead...
# This uses target_sources(Acid) to define its headers & source files
include(CMakeSources.cmake)

# Used to track if we're using ONLY system libs
# Prevents errors with EXPORT
set(_ACID_ALL_SYSTEM_LIBS true)

# Allows usage of configure time content
include(FetchContent)

# Looks for a appropriate threads package for this platform
find_package(Threads REQUIRED)
# Finds Vulkan, env "VULKAN_SDK" must be set
find_package(Vulkan REQUIRED)

# OpenAL must be installed on the system, env "OPENALDIR" must be set
find_package(OpenAL)
find_package(OpenALSoft)
if(OPENAL_FOUND)
	if(OPENAL_FOUND AND NOT TARGET OpenAL::OpenAL)
		add_library(OpenAL::OpenAL UNKNOWN IMPORTED)
		set_target_properties(OpenAL::OpenAL PROPERTIES
				IMPORTED_LOCATION "${OPENAL_LIBRARY}"
				INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIR}"
				)
	endif()
elseif(OPENALSOFT_FOUND)
	if(OPENALSOFT_FOUND AND NOT TARGET OpenAL::OpenAL)
		add_library(OpenAL::OpenAL UNKNOWN IMPORTED)
		set_target_properties(OpenAL::OpenAL PROPERTIES
				IMPORTED_LOCATION "${OPENALSOFT_LIBRARY}"
				INTERFACE_INCLUDE_DIRECTORIES "${OPENALSOFT_INCLUDE_DIR}"
				)
	endif()
else()
	message(FATAL_ERROR "Could not find OpenAL or OpenAL-Soft")
endif()

find_package(glfw3 3.3 QUIET)
if(NOT TARGET glfw)
	set(_ACID_ALL_SYSTEM_LIBS false)
	FetchContent_Declare(glfw3
			URL https://github.com/glfw/glfw/archive/3.3.1.tar.gz
			URL_MD5 ffa7c0d8547f56b0897276c59efdd641
			)
	FetchContent_GetProperties(glfw3)
	if(NOT glfw3_POPULATED)
		foreach(_glfw3_option "GLFW_BUILD_TESTS" "GLFW_BUILD_EXAMPLES" "GLFW_BUILD_DOCS")
			set(${_glfw3_option} OFF CACHE INTERNAL "")
		endforeach()
		FetchContent_Populate(glfw3)
		add_subdirectory(${glfw3_SOURCE_DIR} ${glfw3_BINARY_DIR})
	endif()
endif()

find_package(glslang QUIET)
if(NOT glslang_FOUND)
	set(_ACID_ALL_SYSTEM_LIBS false)
	FetchContent_Declare(glslang
			URL https://github.com/KhronosGroup/glslang/archive/7.13.3496.tar.gz
			URL_MD5 9bfbff6115e490654e149fd0558dba0c
			)
	FetchContent_GetProperties(glslang)
	if(NOT glslang_POPULATED)
		foreach(_glslang_option "BUILD_TESTING" "ENABLE_GLSLANG_BINARIES" "ENABLE_SPVREMAPPER" "ENABLE_HLSL" 
				"ENABLE_AMD_EXTENSIONS" "ENABLE_NV_EXTENSIONS"
				)
			set(${_glslang_option} OFF CACHE INTERNAL "")
		endforeach()
		foreach(_glslang_option "SKIP_GLSLANG_INSTALL" "ENABLE_OPT")
			set(${_glslang_option} ON CACHE INTERNAL "")
		endforeach()
		FetchContent_Populate(glslang)
		add_subdirectory(${glslang_SOURCE_DIR} ${glslang_BINARY_DIR})
	endif()

	# Used in target_link_libraries()
	set(GLSLANG_LIBRARIES "SPIRV")
else()
	set(GLSLANG_INCLUDE_DIRS "${GLSLANG_INCLUDE_DIR}" "${SPIRV_INCLUDE_DIR}")
	set(GLSLANG_LIBRARIES "glslang::glslang" "glslang::SPIRV")
endif()

if(WIN32)
	# TODO: Fix on Unix systems.
	find_package(Freetype 2.9.1 QUIET)
endif()
if(NOT FREETYPE_FOUND)
	set(_ACID_ALL_SYSTEM_LIBS false)
	FetchContent_Declare(freetype
			URL https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
			URL_MD5 c50a3c9e5e62bdc938a6e1598a782947
			)
	FetchContent_GetProperties(freetype)
	if(NOT freetype_POPULATED)
		foreach(_freetype_option "CMAKE_DISABLE_FIND_PACKAGE_PNG" "CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz"
				"CMAKE_DISABLE_FIND_PACKAGE_BZip2" "CMAKE_DISABLE_FIND_PACKAGE_ZLIB" "CMAKE_DISABLE_FIND_PACKAGE_BROTLI"
				)
			set(${_freetype_option} TRUE CACHE INTERNAL "")
		endforeach()
		FetchContent_Populate(freetype)
		add_subdirectory(${freetype_SOURCE_DIR} ${freetype_BINARY_DIR})
	endif()
	set_target_properties(freetype PROPERTIES FOLDER freetype)

	# Used in target_link_libraries()
	set(FREETYPE_LIBRARIES "freetype")
endif()

if(WIN32 AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
	set(CMAKE_DISABLE_FIND_PACKAGE_Bullet TRUE CACHE INTERNAL "")
endif()
find_package(Bullet 2.89 QUIET)
if(NOT BULLET_FOUND)
	set(_ACID_ALL_SYSTEM_LIBS false)
	FetchContent_Declare(bullet3
			URL https://github.com/bulletphysics/bullet3/archive/2.89.tar.gz
			URL_MD5 d239b4800ec30513879834be6fcdc376
			)
	FetchContent_GetProperties(bullet3)
	if(NOT bullet3_POPULATED)
		# On MSVC shared mode must be disabled with bullet currently
		set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
		foreach(_bullet3_option "BUILD_BULLET3" "BUILD_PYBULLET" "BUILD_BULLET2_DEMOS" "BUILD_OPENGL3_DEMOS" 
				"BUILD_CPU_DEMOS" "BUILD_EXTRAS" "BUILD_UNIT_TESTS" "USE_GRAPHICAL_BENCHMARK" "USE_GLUT"
				)
			set(${_bullet3_option} OFF CACHE INTERNAL "")
		endforeach()
		if(MSVC)
			set(USE_MSVC_INCREMENTAL_LINKING ON CACHE INTERNAL "")
			set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE INTERNAL "")
			set(BUILD_SHARED_LIBS OFF)
		endif()
		FetchContent_Populate(bullet3)
		add_subdirectory(${bullet3_SOURCE_DIR} ${bullet3_BINARY_DIR})
		# Reset back to value before MSVC fix
		set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")
	endif()
	set_target_properties(Bullet3Common PROPERTIES FOLDER Bullet)
	set_target_properties(BulletCollision PROPERTIES FOLDER Bullet)
	set_target_properties(BulletDynamics PROPERTIES FOLDER Bullet)
	set_target_properties(BulletInverseDynamics PROPERTIES FOLDER Bullet)
	set_target_properties(BulletSoftBody PROPERTIES FOLDER Bullet)
	set_target_properties(LinearMath PROPERTIES FOLDER Bullet)

	set(BULLET_INCLUDE_DIRS "${bullet3_SOURCE_DIR}/src")
	# Used in target_link_libraries()
	set(BULLET_LIBRARIES "BulletSoftBody" "BulletDynamics" "BulletCollision" "LinearMath")
endif()

find_package(PhysFS 3.0.1 QUIET)
if(NOT PhysFS_FOUND)
	set(_ACID_ALL_SYSTEM_LIBS false)
	FetchContent_Declare(physfs
			URL https://icculus.org/physfs/downloads/physfs-3.0.2.tar.bz2
			URL_MD5 dc751294aaf59d1359bbe34e693d1d87
			)
	FetchContent_GetProperties(physfs)
	if(NOT physfs_POPULATED)
		set(PHYSFS_BUILD_STATIC NOT${BUILD_SHARED_LIBS} CACHE INTERNAL "")
		set(PHYSFS_BUILD_SHARED ${BUILD_SHARED_LIBS} CACHE INTERNAL "")
		set(PHYSFS_BUILD_TEST OFF CACHE INTERNAL "")
		set(PHYSFS_TARGETNAME_DOCS "physfs-docs" CACHE INTERNAL STRING)
		set(PHYSFS_TARGETNAME_DIST "physfs-dist" CACHE INTERNAL STRING)
		set(PHYSFS_TARGETNAME_UNINSTALL "physfs-uninstall" CACHE INTERNAL STRING)
		FetchContent_Populate(physfs)
		add_subdirectory(${physfs_SOURCE_DIR} ${physfs_BINARY_DIR})
	endif()
	set_target_properties(physfs-static PROPERTIES FOLDER physfs)
	if(TARGET physfs)
		set_target_properties(physfs PROPERTIES FOLDER physfs)
	endif()
	if(TARGET physfs-docs)
		set_target_properties(physfs-docs PROPERTIES FOLDER physfs)
	endif()
	if(TARGET physfs-uninstall)
		set_target_properties(physfs-uninstall PROPERTIES FOLDER physfs)
	endif()

	# PhysFS disables rpath on GNU, when disabled Acid will not run on Unix.
	if(CMAKE_COMPILER_IS_GNUCC)
		set(CMAKE_SKIP_RPATH OFF CACHE BOOL "Skip RPATH" FORCE)
	endif()

	set(PHYSFS_INCLUDE_DIR "${physfs_SOURCE_DIR}/src")
	# Set here so we can use it in Sources/CMakeLists.txt
	if(PHYSFS_BUILD_STATIC)
		set(PHYSFS_LIBRARY "physfs-static")
	else()
		set(PHYSFS_LIBRARY "physfs")
	endif()
endif()

target_compile_features(Acid PUBLIC c_std_11 cxx_std_17)
target_compile_definitions(Acid
		PUBLIC
		# If the CONFIG is Debug or RelWithDebInfo, define ACID_DEBUG
		# Works on both single and mutli configuration
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:DEBUG ACID_DEBUG>
		# 32-bit
		$<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:ACID_BUILD_32BIT>
		# 64-bit
		$<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:ACID_BUILD_64BIT>
		# Windows
		$<$<PLATFORM_ID:Windows>:ACID_BUILD_WINDOWS WIN32_LEAN_AND_MEAN NOMINMAX>
		# Linux
		$<$<PLATFORM_ID:Linux>:ACID_BUILD_LINUX>
		# macOS
		$<$<PLATFORM_ID:Darwin>:ACID_BUILD_MACOS>
		# MSVC
		$<$<CXX_COMPILER_ID:MSVC>:ACID_BUILD_MSVC _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS>
		# Clang/AppleClang
		$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:ACID_BUILD_CLANG>
		# GNU/GCC
		$<$<CXX_COMPILER_ID:GNU>:ACID_BUILD_GNU __USE_MINGW_ANSI_STDIO=0>
		)
target_compile_options(Acid
		PUBLIC
		# Disables symbol warnings.
		$<$<CXX_COMPILER_ID:MSVC>:/wd4251 /wd4592 /wd4018 /wd4101 /wd4244 /wd4267 /wd4756 /wd4309>
		PRIVATE
		# Enables SSE4.1, it is also possible to use SSE2 with -msse2
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-msse4.1>
		# Enabled SSE2 for MSVC for 32-bit.
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>>:/arch:SSE2>
		)
target_include_directories(Acid
		PUBLIC
		# Helps the includes find what they need at build-time
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		# Helps the includes find what they need at runtime
		# Although this also allows people to not prefix "Acid" before includes as well
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
		PRIVATE
		# Since building locally from the submodules won't always create these vars.
		# We have to do a simple check if they exist or they will cause errors
		$<$<BOOL:${GLSLANG_INCLUDE_DIRS}>:${GLSLANG_INCLUDE_DIRS}>
		$<$<BOOL:${FREETYPE_INCLUDE_DIRS}>:${FREETYPE_INCLUDE_DIRS}>
		$<$<BOOL:${BULLET_INCLUDE_DIRS}>:${BULLET_INCLUDE_DIRS}>
		$<$<BOOL:${PHYSFS_INCLUDE_DIR}>:${PHYSFS_INCLUDE_DIR}>
		)
target_link_libraries(Acid
		PUBLIC
		# All IMPORTED targets, which automatically handles includes
		Threads::Threads
		Vulkan::Vulkan
		# Filesystem
		$<$<CXX_COMPILER_ID:GNU>:stdc++fs>
		# Unix
		${CMAKE_DL_LIBS}
		# Windows
		$<$<PLATFORM_ID:Windows>:ws2_32>
		$<$<PLATFORM_ID:Windows>:dbghelp>
		# Android
		$<$<PLATFORM_ID:Android>:log>
		PRIVATE
		# More IMPORTED
		OpenAL::OpenAL
		glfw
		${GLSLANG_LIBRARIES}
		${FREETYPE_LIBRARIES}
		${BULLET_LIBRARIES}
		${PHYSFS_LIBRARY}
		)

set_target_properties(Acid PROPERTIES
		#INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
		FOLDER "Acid"
		)

# Installs all headers, preserving their file structure
foreach(_acid_header IN LISTS _temp_acid_generated_headers)
    get_filename_component(_acid_header_dir ${_acid_header} DIRECTORY)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_acid_header}
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/${_acid_header_dir}"
			)
endforeach()
foreach(_acid_header IN LISTS _temp_acid_headers)
    get_filename_component(_acid_header_dir ${_acid_header} DIRECTORY)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${_acid_header}
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/${_acid_header_dir}"
			)
endforeach()
# If strictly using system libs, we can generate an export & install it
if(_ACID_ALL_SYSTEM_LIBS)
	install(TARGETS Acid
			# Auto-generates an export to install
			EXPORT
			AcidTargets
			LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			)
	# Install the auto-generated export
	install(EXPORT AcidTargets
			# Makes the end-user imported library Acid::Acid instead of just Acid
			NAMESPACE Acid::
			DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
			)
	# Defines write_basic_package_version_file
	include(CMakePackageConfigHelpers)
	# Dynamically writes the path to the resources dir (if installed) into the AcidConfig.cmake file
	configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/AcidConfig.cmake.in"
			"${PROJECT_BINARY_DIR}/AcidConfig.cmake"
			INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
			PATH_VARS CMAKE_INSTALL_DATADIR
			NO_CHECK_REQUIRED_COMPONENTS_MACRO
			)
	# Auto-generates a version file based on the current version specified in project()
	write_basic_package_version_file("${PROJECT_BINARY_DIR}/AcidConfigVersion.cmake"
			COMPATIBILITY SameMajorVersion
			)
	# Install the auto-generated version file and AcidConfig
	install(FILES
			"${PROJECT_BINARY_DIR}/AcidConfig.cmake"
			"${PROJECT_BINARY_DIR}/AcidConfigVersion.cmake"
			DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
			)
else()
	# Install without an export since we're using 1 (or more) non-system libs
	install(TARGETS Acid
			LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
			)
endif()
