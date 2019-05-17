#include <string>

#ifndef V4ENCS
#define V4ENCS
enum class V4L2Encoders {
    CAMERA_H264, OPENMAX, SOFTWARE
};
#endif
#ifndef VIDEO_SOURCES
#define VIDEO_SOURCES
enum class VideoSources {
    V4L2, RASP, SHMEM
};
#endif

std::string raspberry_pipe(const int *height,
                           const int *framerate,
                           const int *rotation,
                           const bool *preview
);

std::string v4l2_pipe(const int *height, const int *framerate, V4L2Encoders encoder, const std::string &device);

std::string shm_pipe(const int *height, const int *width, const int *framerate, V4L2Encoders encoder,
                     const std::string &device);