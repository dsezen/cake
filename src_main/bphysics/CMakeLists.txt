project(bphysics)

#
# Libraries
# 

# bullet physics
if(NOT USE_SYSTEM_BULLET3)
	SET(BULLET3_PATH ../libs/bullet3)
endif()
find_package(BULLET3 REQUIRED)
include_directories(${BULLET3_INCLUDE_DIR})

#
# flags
#

# Need double precision since that's what the libraries are
# compiled with.
add_definitions(-DBT_USE_DOUBLE_PRECISION)

#
# bphysics module
#

# shared code
set(SHARED_INCLUDES
	../shared/game.h
	../shared/q_shared.h
	../shared/qfiles.h
	)
set(SHARED_SOURCES
	../shared/m_flash.c
	../shared/q_shared.c
	)
source_group("shared" FILES ${SHARED_INCLUDES})
source_group("shared" FILES ${SHARED_SOURCES})

# shared physics code
file(GLOB_RECURSE BPHYSICS_SHARED_INCLUDES 
    LIST_DIRECTORIES false 
    ../shared/bphysics/*.h
)
source_group("shared/bphysics" FILES ${BPHYSICS_SHARED_INCLUDES})
MESSAGE(${BPHYSICS_SHARED_INCLUDES})

# bullet code
file(GLOB BULLET3_INCLUDES ${BULLET3_INCLUDE_DIR}/*.h)
source_group("Bullet Physics" FILES ${BULLET3_INCLUDES})

# physics dll code
file(GLOB BPHYSICS_INCLUDES *.h)
file(GLOB BPHYSICS_SOURCES *.cpp)
source_group("bphysics" FILES ${BPHYSICS_INCLUDES})
source_group("bphysics" FILES ${BPHYSICS_SOURCES})

# set include directories
include_directories(
    .
    ../shared
    ${BULLET3_INCLUDE_DIR}
)

# collect physics dll files
add_library(bphysics SHARED 
    ${SHARED_INCLUDES} 
    ${SHARED_SOURCES} 
    ${BPHYSICS_INCLUDES} 
    ${BPHYSICS_SOURCES}
)

# collect physics libraries
FOREACH(_LIB ${BULLET3_LIBS_DEBUG})
    target_link_libraries(bphysics PRIVATE debug ${_LIB})
ENDFOREACH()

FOREACH(_LIB ${BULLET3_LIBS_RELEASE})
    target_link_libraries(bphysics PRIVATE optimized ${_LIB})
ENDFOREACH()

q_set_output_dir(bphysics ${Q_GAME_DIR})
install(TARGETS bphysics RUNTIME DESTINATION ${Q_GAME_DIR} LIBRARY DESTINATION ${Q_GAME_DIR})

# 