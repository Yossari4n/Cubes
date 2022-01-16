set(LIBRARY_NAME "${PROJECT_NAME}")

find_package(OpenGL REQUIRED)

set(LIBRARY_SRC_PATH     		"${LIBRARY_MODULE_PATH}")
set(LIBRARY_PUBLIC_INCLUDE_PATH 	"${PROJECT_SOURCE_DIR}")
set(LIBRARY_PRIVATE_INCLUDE_PATH	"${LIBRARY_MODULE_PATH}")

set(
	SOURCE_LIST
	"glext.h"
	"wglext.h"
	"main.cpp"
)

set(
	LIBS 
	opengl32
)

add_executable(
	${LIBRARY_NAME}
	WIN32
	${SOURCE_LIST}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${LIBRARY_PUBLIC_INCLUDE_PATH}>
    PRIVATE
        ${LIBRARY_MODULE_PATH}
)

target_link_libraries(${LIBRARY_NAME} ${LIBS})
target_compile_features(${LIBRARY_NAME} PRIVATE cxx_std_17)

set(LIBRARY_NAME ${LIBRARY_NAME} PARENT_SCOPE)