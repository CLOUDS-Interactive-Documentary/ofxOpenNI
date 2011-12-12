#project(LibJPEG)

file(GLOB ${JPEG_LIB_NAME}_sources *.c)
include_directories(${OPENNI_INCLUDE_DIR})
add_library(${JPEG_LIB_NAME} SHARED ${${JPEG_LIB_NAME}_sources})
add_library(${JPEG_LIB_NAME}_static STATIC ${${JPEG_LIB_NAME}_sources})
target_link_libraries(${JPEG_LIB_NAME} ${OPENNI_LIB_NAME})

# The install_name is Mac OSX specific and is used to set the RPATH on a executable
# ---------------------------------------------------------------------------------
if(OPENNI_PLATFORM_APPLE )
	set_target_properties(${JPEG_LIB_NAME} PROPERTIES 
                BUILD_WITH_INSTALL_RPATH ON
                INSTALL_NAME_DIR ${INSTALL_NAME_VALUE}
        )
endif()

install(TARGETS ${JPEG_LIB_NAME}
        LIBRARY DESTINATION ${LIB_INSTALL_PATH}
        RUNTIME DESTINATION .
)
