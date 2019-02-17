#include <string>

#ifdef DEFAULT_FPS
std::string raspberry_pipe(const std::string height,
    const std::string width="",
    const std::string framerate=DEFAULT_FPS,
    const bool preview=false
);
std::string v4l2_pipe(const std::string height,
    const std::string framerate=DEFAULT_FPS,
    const bool openmax=false
);
#else
std::string raspberry_pipe(const std::string height,
    const std::string width="",
    const std::string framerate="30",
    const bool preview=false
);
std::string v4l2_pipe(const std::string height,
    const std::string framerate="30",
    const bool openmax=false
);
#endif
