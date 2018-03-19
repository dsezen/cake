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
# set(BULLET3_LIBRARY ${BULLET3_LIBRARY})

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

# physics dll code
file(GLOB BPHYSICS_INCLUDES *.h)
file(GLOB BPHYSICS_SOURCES *.cpp)
source_group("bphysics" FILES ${BPHYSICS_INCLUDES})
source_group("bphysics" FILES ${BPHYSICS_SOURCES})

# set include directories
include_directories(
    .
    ../shared
)

# build the bphysics dll
add_library(bphysics SHARED 
    ${SHARED_INCLUDES} 
    ${SHARED_SOURCES} 
    ${BPHYSICS_INCLUDES} 
    ${BPHYSICS_SOURCES}
)

target_link_libraries(bphysics PRIVATE 
    release ${BULLET3_LIBS_RELEASE}
    debug ${BULLET3_LIBS_DEBUG}
)

q_set_output_dir(bphysics ${Q_GAME_DIR})
install(TARGETS bphysics RUNTIME DESTINATION ${Q_GAME_DIR} LIBRARY DESTINATION ${Q_GAME_DIR})
