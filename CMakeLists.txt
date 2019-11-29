cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

PROJECT(potential-engine
    VERSION 1.3.2
    DESCRIPTION "Simple RTSP server designed for the FIRST Robotics Competition"
    HOMEPAGE_URL "https://github.com/BHSSFRC/potential-engine")

AUX_SOURCE_DIRECTORY(src potential-engine_SOURCES)

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(DEPS REQUIRED fmt gio-2.0 glib-2.0 gstreamer-1.0 gstreamer-rtsp-server-1.0)
INCLUDE_DIRECTORIES(${DEPS_INCLUDE_DIRS})
LINK_DIRECTORIES(${DEPS_LIBRARY_DIRS})

ADD_EXECUTABLE(${PROJECT_NAME} ${potential-engine_SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${DEPS_LIBRARIES} -lm ${DEPS_LDFLAGS_OTHER})

INSTALL(TARGETS potential-engine DESTINATION bin)
# CPack nonsense
SET(CPACK_GENERATOR DEB TGZ ZIP)
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "fmt (>= 6.0.1), libgstrtspserver-1.0-dev (1.14.4-1), gstreamer1.0-omx-rpi (>= 1.14.4-1+rpt1), gstreamer1.0-plugins-ugly (1.14.4-1), gstreamer1.0-plugins-good (1.14.4-1), gstreamer1.0-plugins-bad (1.14.4-1)")
SET(CPACK_PACKAGE_CONTACT "The Quadrangles FRC 3494 <frc3494@gmail.com>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE /dev/null)
INCLUDE(CPack)
