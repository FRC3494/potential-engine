#include <string>

std::string raspberry_pipe(const int *height,
                           const int *framerate,
                           const int *rotation,
                           const bool *preview
);

std::string v4l2_pipe(const int *height,
                      const int *framerate,
                      const bool openmax,
                      std::string device
);
