cmake_minimum_required (VERSION 2.8.1)

######## Project settings ########
PROJECT(bl_host)
SET(LICENSE "BSD")

######## Build and include settings ########
include_directories(
	inc
	mcp
)

link_directories(
	${LINK_DIRECTORIES}
)


file(GLOB SOURCES
	"src/*.cpp"
	"src/*.c"
	"mcp/*.c"
	"mcp/*.cpp"
)

add_executable(
	bl_host

	${SOURCES}
)

TARGET_LINK_LIBRARIES(
	bl_host

	usb-1.0
	serialport
	pthread
)

######## Install targets ########
INSTALL(TARGETS bl_host
	RUNTIME DESTINATION usr/bin
)
