#project(Sample-NiSimpleCreate)
set(SIMPLECREATE_EXE Sample-NiSimpleCreate)

# Add all files we have for the example.
# ---------------------------------------
file(GLOB ${SIMPLECREATE_EXE}_sources ./*.cpp)

# Include directories
# ---------------------------------------
include_directories(${OPENNI_INCLUDE_DIR})

# The target for this sample
# ---------------------------------------
add_executable(${SIMPLECREATE_EXE} ${${SIMPLECREATE_EXE}_sources})

# And make it link against these libs.
# ---------------------------------------
target_link_libraries(${SIMPLECREATE_EXE} ${OPENNI_LIB_NAME})
add_dependencies(${SIMPLECREATE_EXE} ${OPENNI_LIB_NAME})

# We install the file to the dir where cmake is executed
# ------------------------------------------------------
install(TARGETS ${SIMPLECREATE_EXE}
	RUNTIME DESTINATION ./${BIN_INSTALL_PATH}
)
