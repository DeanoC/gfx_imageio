#if building standalone and not specified build tests
option(BUILD_TESTS "Whether to build tests" ON)
include(FetchContent)
FetchContent_Declare(al2o3 GIT_REPOSITORY https://github.com/DeanoC/al2o3 GIT_TAG master)
FetchContent_GetProperties(al2o3)
if (NOT al2o3_POPULATED)
	FetchContent_Populate(al2o3)
	add_subdirectory(${al2o3_SOURCE_DIR} ${al2o3_BINARY_DIR})
endif ()

# BASISU is apache 2 licensed which should be fine for most people but in case you can exclude it
set(USE_BASISU_TRANSCODER 1)

SET_MIN_VERSIONS()
set(LibName gfx_imageio)
project(${LibName})

set(Interface
		io.h
		)

set(Src
		loader.cpp
		saver.cpp
		)

set(Deps
		al2o3_platform
		al2o3_cmath
		al2o3_stb
		al2o3_vfile
		al2o3_syoyo
		tiny_imageformat
		gfx_image_interface
		utils_misccpp
		tiny_ktx
		tiny_dds
		)
ADD_LIB(${LibName} "${Interface}" "${Src}" "${Deps}")

set(Tests
		runner.cpp
		test_imageio.cpp
		)
set(TestDeps
		utils_simple_logmanager
		gfx_image_impl_basic
		al2o3_catch2)
ADD_LIB_TESTS(${LibName} "${Interface}" "${Tests}" "${TestDeps}")

if (USE_BASISU_TRANSCODER)
	target_compile_definitions(${LibName} PRIVATE USE_BASISU_TRANSCODER)
	target_sources(${LibName} PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu.h
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_file_headers.h
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_global_selector_cb.h
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_global_selector_palette.h
			#			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder.h
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder_internal.h
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder_tables_astc.inc
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder_tables_astc_0_255.inc
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder_tables_atc_55.inc
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder_tables_atc_56.inc
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder_tables_bc7_m5_alpha.inc
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder_tables_bc7_m5_color.inc
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder_tables_bc7_m6.inc
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder_tables_dxt1_5.inc
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu_transcoder/basisu_transcoder_tables_dxt1_6.inc
			${CMAKE_CURRENT_SOURCE_DIR}/src/basisu.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/include/gfx_imageio/basisu.h
			)
endif ()

