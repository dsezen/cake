###############################################################################
# Bullet 3 SDK locator.
###############################################################################

# -----------------------------------------------------------------------------
# Setup paths and find bullet include directory.
# -----------------------------------------------------------------------------
SET(BULLET3_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw             # Fink
	/opt/local      # DarwinPorts
	/opt/csw        # Blastwave
	/opt
	${BULLET3_PATH} # Set by the importing module calling find_package(..).
)

FIND_PATH(BULLET3_INCLUDE_DIR btBulletDynamicsCommon.h
	HINTS
	$ENV{BULLET3DIR}
	PATH_SUFFIXES src
	PATHS ${BULLET3_SEARCH_PATHS}
)

# Setup base lib directory.
SET(BULLET3_LIB_DIR "${BULLET3_PATH}/lib")
MESSAGE(${BULLET3_LIB_DIR})
IF(EXISTS ${BULLET3_LIB_DIR})
    MESSAGE("Bullet3 library directory: ${BULLET3_LIB_DIR}")
ELSE()
    MESSAGE(FATAL_ERROR "Bullet3 library directory not found.")
ENDIF()

# -----------------------------------------------------------------------------
# Setup libraries
# -----------------------------------------------------------------------------

# Currently only supports 32-bit.
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE(FATAL_ERROR "Bullet Physics only supported on 32-bit builds.")
ENDIF()

# Setup libraries that need to be linked.
SET(BULLET3_LIB_NAMES
    Bullet3Collision
    Bullet3Common
    Bullet3Dynamics
    Bullet3Geometry
    LinearMath
)

# FIND RELEASE LIBRARIES
MESSAGE("Searching for release libraries..")
FOREACH(LIB ${BULLET3_LIB_NAMES})
    FIND_LIBRARY(FOUND_LIB_${LIB}_RELEASE ${LIB} PATHS "${BULLET3_LIB_DIR}/Release")
    MESSAGE(${FOUND_LIB_${LIB}_RELEASE})
    LIST(APPEND BULLET3_LIBS_RELEASE_TEMP ${FOUND_LIB_${LIB}_RELEASE})
    MESSAGE("Found release library at: ${FOUND_LIB_${LIB}}")
ENDFOREACH(LIB)

# FIND DEBUG LIBRARIES.
MESSAGE("Searching for debug libraries..")
FOREACH(LIB ${BULLET3_LIB_NAMES})
    FIND_LIBRARY(FOUND_LIB_${LIB}_DEBUG ${LIB}_Debug PATHS "${BULLET3_LIB_DIR}/Debug")
    LIST(APPEND BULLET3_LIBS_DEBUG_TEMP ${FOUND_LIB_${LIB}_DEBUG})
    MESSAGE("Found debug library at: ${FOUND_LIB_${LIB}_DEBUG}")
ENDFOREACH(LIB)

# Set this so importing modules can use it.
SET(BULLET3_LIBS_RELEASE ${BULLET3_LIBS_RELEASE_TEMP})
SET(BULLET3_LIBS_DEBUG ${BULLET3_LIBS_DEBUG_TEMP})
SET(BULLET3_LIBS ${BULLET3_LIBS_DEBUG} ${BULLET3_LIBS_RELEASE})

# Set the temp variable to INTERNAL so it is not seen in the CMake GUI
SET(BULLET3_LIBS_DEBUG_TEMP "${BULLET3_LIBS_DEBUG_TEMP}" CACHE INTERNAL "")
SET(BULLET3_LIBS_RELEASE_TEMP "${BULLET3_LIBS_RELEASE_TEMP}" CACHE INTERNAL "")

# -----------------------------------------------------------------------------
# Implementation of script.
# -----------------------------------------------------------------------------
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BULLET3 REQUIRED_VARS BULLET3_LIBS BULLET3_INCLUDE_DIR)
