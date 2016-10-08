cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 11)
project (shade)

###############################################################################
# Compilable submodules
###############################################################################
# GLFW
	option(GLFW_BUILD_DOCS OFF)
	option(GLFW_BUILD_EXAMPLES OFF)
	option(GLFW_BUILD_TESTS OFF)
	add_subdirectory(3rdparty/glfw)

include_directories(
	include/
	coquilib/
	3rdparty/
	3rdparty/glfw/include
	3rdparty/glm
	3rdparty/imgui
)

file(GLOB PROJECT_HEADERS include/*.h)
file(GLOB PROJECT_SOURCES 
		src/*.cpp 
        src/3rdparty/*.cpp)
file(GLOB TEST_SOURCES tests/*.cpp)

###############################################################################
# IDE Source Groups
###############################################################################
source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Source" FILES ${PROJECT_SOURCES})
source_group("Test" FILES ${TEST_SOURCES})

###############################################################################
# Main Executable
###############################################################################
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})

###############################################################################
# Tests (auto-generate one exe and test per cpp in tests)
###############################################################################
enable_testing()
foreach(tf ${TEST_SOURCES})
	get_filename_component(tname ${tf} NAME_WE)
	add_executable(${tname} ${tf})
	set_target_properties(${tname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/${tname})
	add_test(NAME ${tname} COMMAND ${tname})
endforeach()	
