#include "pipe_builder.h"

#include <map>
#include <fmt/core.h>

static std::map<int, int> resolutions = {
        {144, 256},
        {240, 426},
        {360, 640},
        {480, 640},
        {720, 1280}
};

std::string raspberry_pipe(const int *height, const int *framerate, const int *rotation, const bool *preview) {
    int used_width = resolutions[*height];
    std::string pipeline = fmt::format("rpicamsrc preview={} rotation={}", *preview, *rotation);
    // Add cap filter
    pipeline = pipeline +
               fmt::format(" ! video/x-h264,height={},width={},framerate={}/1", *height, used_width, *framerate);
    // payload
    pipeline = pipeline + " ! h264parse ! rtph264pay name=pay0";
    return pipeline;
}

std::string v4l2_pipe(const int *height, const int *framerate, const bool *openmax, std::string device) {
    std::string encoder;
    if (*openmax) {
        encoder = "omxh264enc ! video/x-h264,profile=baseline";
    } else {
        encoder = "x264enc tune=zerolatency";
    }
    return fmt::format("v4l2src device={d} ! "
                       "video/x-raw,height={h},framerate={f}/1,name=yuy ! "
                       "videoconvert ! "
                       "video/x-raw,format=I420 ! "
                       "{e} ! rtph264pay name=pay0",
                       fmt::arg("h", *height), fmt::arg("f", *framerate), fmt::arg("e", encoder), fmt::arg("d", device)
    );
}
