#project(nimCodecs)

file(GLOB ${NIMCOMMON_LIB_NAME}_sources ./*.cpp)

include_directories(${OPENNI_INCLUDE_DIR})
include_directories(${OPENNI_MODULES_DIR}/Common/)

add_library(${NIMCOMMON_LIB_NAME} SHARED ${${NIMCOMMON_LIB_NAME}_sources})
target_link_libraries(${NIMCOMMON_LIB_NAME} ${OPENNI_LIB_NAME})

# The install_name is Mac OSX specific and is used to set the RPATH on a executable
# ---------------------------------------------------------------------------------
if(OPENNI_PLATFORM_APPLE )
        set_target_properties(${NIMCOMMON_LIB_NAME} PROPERTIES 
                BUILD_WITH_INSTALL_RPATH ON
                INSTALL_NAME_DIR ${INSTALL_NAME_VALUE}
        )
endif()

install(TARGETS ${NIMCOMMON_LIB_NAME}
	LIBRARY DESTINATION ${LIB_INSTALL_PATH} 
	RUNTIME DESTINATION .
)

