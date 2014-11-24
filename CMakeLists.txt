CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

INCLUDE_DIRECTORIES(./SDL)

SET (QWRT_SLASHER_SOURCES 
	Camera
	CVector2
	CVector2i
	CVector3
	Dither
	EmbreeStuff
	Engine
	Etc1Fast
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

MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH: " ${EXECUTABLE_OUTPUT_PATH})

LINK_DIRECTORIES()

SET(SSE_FLAGS "-msse -msse2 -msse3 -mssse3")
SET (CMAKE_CXX_FLAGS "${SSE_FLAGS} -O3")

ADD_EXECUTABLE(qwrt ${QWRT_SLASHER_SOURCES})

TARGET_LINK_LIBRARIES(qwrt SDL SDL_net GL)
