# Add search paths.
# ------------------
include_directories(${USB_INCLUDE_DIR})
include_directories(${TINYXML_INCLUDE_DIR})
include_directories(${OPENNI_INCLUDE_DIR})
include_directories("./FilesOnly")

# Get sources for OpenNI
# ----------------------
file(GLOB ${OPENNI_LIB_NAME}_sources ./*.cpp)

# Add platform specific sources.
# ------------------------------
if(OPENNI_PLATFORM_LINUX)
	file(GLOB ${OPENNI_LIB_NAME}_platform_sources ./Linux-x86/*.cpp)
elseif(OPENNI_PLATFORM_APPLE)
	file(GLOB ${OPENNI_LIB_NAME}_platform_sources ./Linux-x86/*.cpp)
	message(${${OPENNI_LIB_NAME}_platform_sources})
	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation -framework IOKit")
endif()

# Set our target
# ---------------
add_library(
	${OPENNI_LIB_NAME} 
	SHARED 
	${${OPENNI_LIB_NAME}_sources} 
	${${OPENNI_LIB_NAME}_platform_sources}
)

# The install_name is Mac OSX specific and is used to set the RPATH on a executable
# ----------------------------------------------------------------------------------
if(OPENNI_PLATFORM_APPLE)
	set_target_properties(${OPENNI_LIB_NAME} PROPERTIES 
		BUILD_WITH_INSTALL_RPATH ON
		INSTALL_NAME_DIR ${INSTALL_NAME_VALUE}
	)
endif()

# Set Link libraries.
# -------------------
find_library(LIBDL dl)
target_link_libraries(
	${OPENNI_LIB_NAME}
	${USB_LIB_NAME} 
	${LIBDL} 
	${TINYXML_LIB_NAME}
	${CMAKE_EXE_LINKER_FLAGS}
)

# Set path where to install the target, lib, runtime. 
# (to same dir where we call cmake ., as we use the "dot")
# -----------------------------------------------------
install(TARGETS ${OPENNI_LIB_NAME}
	LIBRARY DESTINATION ./${LIB_INSTALL_PATH}
	RUNTIME DESTINATION .
)
