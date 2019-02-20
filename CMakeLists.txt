cmake_minimum_required(VERSION 2.8)

PROJECT(potential-engine)

SET(potential-engine_SOURCES
    src/main.cpp
    src/networker.cpp
    src/optional.cpp    
    src/pipe_builder.cpp
)

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES (DEPS REQUIRED fmt gio-2.0 glib-2.0 gstreamer-1.0 gstreamer-rtsp-server-1.0)
INCLUDE_DIRECTORIES(${DEPS_INCLUDE_DIRS})
LINK_DIRECTORIES(${DEPS_LIBRARY_DIRS})

ADD_EXECUTABLE(${PROJECT_NAME} ${potential-engine_SOURCES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${DEPS_LIBRARIES} -lm ${DEPS_LDFLAGS_OTHER})
