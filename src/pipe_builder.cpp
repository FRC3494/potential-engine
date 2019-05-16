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

std::string
encoder_string(const int *height, const int *width, const int *framerate, const V4L2Encoders encoder, const bool shmem,
               const std::string &device) {
    std::string noFmtEnc;
    if (width == nullptr) {
        switch (encoder) {
            case V4L2Encoders::CAMERA_H264:
                noFmtEnc = "video/x-h264,height={h},framerate={f}/1";
                break;
            case V4L2Encoders::OPENMAX:
                noFmtEnc = "video/x-raw,height={h},framerate={f}/1,name=yuy ! videoconvert ! video/x-raw,format=I420 ! omxh264enc ! video/x-h264,profile=baseline";
                break;
            case V4L2Encoders::SOFTWARE:
                noFmtEnc = "video/x-raw,height={h},framerate={f}/1,name=yuy ! videoconvert ! x264enc tune=zerolatency";
                break;
        }
        return fmt::format(noFmtEnc, fmt::arg("h", *height), fmt::arg("f", *framerate));
    } else if (shmem) {
        // for shared memory
        switch (encoder) {
            case V4L2Encoders::CAMERA_H264:
                noFmtEnc = "video/x-h264,height={h},width={w},framerate={f}/1";
                break;
            case V4L2Encoders::OPENMAX:
                noFmtEnc = "video/x-raw,format=I420,height={h},width={w},framerate={f}/1 !  omxh264enc ! video/x-h264,profile=baseline";
                break;
            case V4L2Encoders::SOFTWARE:
                noFmtEnc = "video/x-raw,format=I420,height={h},width={w},framerate={f}/1 ! queue ! x264enc tune=zerolatency";
                break;
        }
        return fmt::format(noFmtEnc, fmt::arg("h", *height), fmt::arg("w", *width), fmt::arg("f", *framerate));
    }
    // interestingly i am allowed to not return anything
    // so that's nice
}

std::string v4l2_pipe(const int *height, const int *framerate, const V4L2Encoders encoder, const std::string &device) {
    std::string enc = encoder_string(height, nullptr, framerate, encoder, false, device);
    return fmt::format("v4l2src device={d} ! {e} ! rtph264pay name=pay0", fmt::arg("d", device), fmt::arg("e", enc));
}

std::string shm_pipe(const int *height, const int *width, const int *framerate, const V4L2Encoders encoder,
                     const std::string &device) {
    V4L2Encoders real_encoder = encoder;
    if (encoder == V4L2Encoders::CAMERA_H264) {
        // hmm no i don't believe it
        real_encoder = V4L2Encoders::SOFTWARE;
    }
    std::string enc = encoder_string(height, width, framerate, real_encoder, true, device);
    return fmt::format("shmsrc socket-path={d} ! {e} ! rtph264pay name=pay0", fmt::arg("d", device),
                       fmt::arg("e", enc));
}
