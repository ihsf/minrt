PROJECT(MINRT)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE(FindPkgConfig)

# OpenGL
FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

# SDL
PKG_CHECK_MODULES(SDL2 REQUIRED sdl2 SDL2_net)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
LINK_DIRECTORIES(${SDL2_LIBRARY_DIRS})

# Embree setup
SET(EMBREE_DIR "" CACHE PATH "Embree source directory")
FIND_LIBRARY(EMBREE_LIB NAMES embree embree.2 PATHS ${EMBREE_DIR} ${EMBREE_DIR}/lib/x64 ${EMBREE_DIR}/build)
INCLUDE_DIRECTORIES("${EMBREE_DIR}/include")

# pthread on GNU/Linux
FIND_PACKAGE(Threads)

SET(MINRT_SOURCES
	Camera
	CVector2
	CVector2i
	CVector3
	Dither
	EmbreeStuff
	Engine
	lz4-r123/lz4
	lz4-r123/lz4hc
	MinRT
	NetworkStuff
	OpenGLstuff
	ProcessRGB
	RT_Camera
	RT_FrameBuffer
	RT_RayQuery
	RT_RayTracer
	RT_ShadingInfo
	RT_Task
	RT_TaskManager
	RT_TaskRenderTile
	SDLstuff
	StanfordBunny
	System
	TablesETC1
	TaskDispatch
	WindowsHelper
   )

# CLANG / GCC
#SET(CILK_FLAGS "-fcilkplus -lcilkrts")
#SET(OMP_FLAGS "-fopenmp ")

# ICC
#SET(CILK_FLAGS "-lcilkrts")
#SET(OMP_FLAGS "-openmp ")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${CILK_FLAGS} ${OMP_FLAGS}")

ADD_EXECUTABLE(minrt ${MINRT_SOURCES})
TARGET_LINK_LIBRARIES(minrt ${CMAKE_THREAD_LIBS_INIT} ${SDL2_LIBRARIES} ${EMBREE_LIB} ${OPENGL_LIBRARIES})

