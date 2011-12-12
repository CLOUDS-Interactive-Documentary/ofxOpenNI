#project(nimCodecs)

file(GLOB ${NIMCODECS_LIB_NAME}_sources ./*.cpp)

include_directories(${JPEG_INCLUDE_DIR})
include_directories(${OPENNI_INCLUDE_DIR})
include_directories(${OPENNI_MODULES_DIR}/Common/)

add_library(${NIMCODECS_LIB_NAME} SHARED ${${NIMCODECS_LIB_NAME}_sources})
target_link_libraries(${NIMCODECS_LIB_NAME} ${JPEG_LIB_NAME} ${OPENNI_LIB_NAME})

# The install_name is Mac OSX specific and is used to set the RPATH on a executable
# ----------------------------------------------------------------------------------
if(OPENNI_PLATFORM_APPLE)
	set_target_properties(${NIMCODECS_LIB_NAME} PROPERTIES 
		BUILD_WITH_INSTALL_RPATH ON
		INSTALL_NAME_DIR ${INSTALL_NAME_VALUE}
	)
endif()

install(TARGETS ${NIMCODECS_LIB_NAME}
	LIBRARY DESTINATION ${LIB_INSTALL_PATH}
	RUNTIME DESTINATION .
)
