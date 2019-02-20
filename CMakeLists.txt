cmake_minimum_required(VERSION 2.8)

PROJECT(camerac)

SET(camerac_SOURCES
    src/main.cpp
    src/pipe_builder.cpp
    src/optional.cpp    
)

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES (DEPS REQUIRED fmt gio-2.0 glib-2.0 gstreamer-1.0 gstreamer-rtsp-server-1.0)
INCLUDE_DIRECTORIES(${DEPS_INCLUDE_DIRS})
LINK_DIRECTORIES(${DEPS_LIBRARY_DIRS})

ADD_EXECUTABLE(${PROJECT_NAME} ${camerac_SOURCES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${DEPS_LIBRARIES} -lm ${DEPS_LDFLAGS_OTHER})
