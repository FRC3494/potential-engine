#include "pipe_builder.h"

#include <map>
#include <fmt/core.h>

static std::map<std::string, std::string> resolutions = {
    {"144", "256"},
    {"240", "426"},
    {"360", "640"},
    {"480", "640"},
    {"720", "1280"}
};

std::string raspberry_pipe(const std::string height, const std::string width, const std::string framerate, const bool preview) {
    std::string used_width = width;
    if (width == "") {
        used_width = resolutions[height];
    }
    std::string pipeline = fmt::format("rpicamsrc preview={}", preview);
    // Add cap filter
    pipeline = pipeline + fmt::format(" ! video/x-h264,height={},width={},framerate={}/1", height, used_width, framerate);
    // payload
    pipeline = pipeline + " ! h264parse ! rtph264pay name=pay0";
    return pipeline;
}

std::string v4l2_pipe(const std::string height, const std::string framerate, const bool openmax) {
    std::string encoder = openmax ? "omxh264enc ! video/x-h264,profile=baseline" : "x264enc tune=zerolatency";
    return fmt::format("v4l2src ! "
        "video/x-raw,format=YUY2,height={h},framerate={f}/1 ! "
        "videoconvert ! "
        "video/x-raw,format=I420 ! "
        "{e} ! rtph264pay name=pay0",
        fmt::arg("h", height), fmt::arg("f", framerate), fmt::arg("e", encoder)
    );
}
