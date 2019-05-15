#include <stdlib.h>
#include <string>

#include "optional.h"

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#define DEFAULT_RTSP_PORT 1181
#define DEFAULT_RESOLUTION 480
#define DEFAULT_WIDTH 640
#define DEFAULT_FPS 30
#define DEFAULT_MOUNT "/stream"

bool rpi_cam_flag = false;
bool shared_mem = false;
bool judge = false;
// Raspberry-specific options
int rot = 0;
bool prev = false;
// V4L2 options
bool hw_encoder = false;
bool cam_encoder = false;
char *video_dev = (char *) "/dev/video0";
// common options
int height = DEFAULT_RESOLUTION;
int width = DEFAULT_WIDTH;
int fps = DEFAULT_FPS;
// network
char *addr = (char *) "0.0.0.0";
int port_ = DEFAULT_RTSP_PORT;
const char *url = nullptr;

GOptionEntry entries[] = {
        {"rpi_cam",       'r', G_OPTION_FLAG_NONE,   G_OPTION_ARG_NONE, &rpi_cam_flag, "Use Raspberry Pi Camera module (default: false)",                                           nullptr},
        {"shared_memory", 's', G_OPTION_FLAG_NONE,   G_OPTION_ARG_NONE, &shared_mem,   "Read frames from shared memory (default: false)"},
        {"fps",           'f', G_OPTION_FLAG_NONE,   G_OPTION_ARG_INT,  &fps,          "Framerate in FPS (default: " STRINGIFY(
                DEFAULT_FPS) ")",                                                                                                                                                   "FPS"},
        {"height",        'h', G_OPTION_FLAG_NONE,   G_OPTION_ARG_INT,  &height,       "Video height. Should be a standard resolution (in [240, 360, 480, 720] for most cameras.)", "HEIGHT"},
        {"width",         'w', G_OPTION_FLAG_NONE,   G_OPTION_ARG_INT,  &width,        "Video width. Only needs to be specified if you're using shared memory.",                    "WIDTH"},
        {"judge",         '9', G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &judge,        "",                                                                                          nullptr},
        {nullptr}
};

static GOptionEntry netEntries[]{
        {"address", 'a', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &addr,  "Network address to bind to (default: 0.0.0.0)",       "ADDRESS"},
        {"port",    'p', G_OPTION_FLAG_NONE, G_OPTION_ARG_INT,    &port_, "Port to listen on (default: " STRINGIFY(
                DEFAULT_RTSP_PORT) ")",                                                                                          "PORT"},
        {"url",     'u', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &url,   "URL to stream video at (default: " DEFAULT_MOUNT ")", "URL"},
        {nullptr}
};
GOptionGroup *netOpts = g_option_group_new("net", "Networking options", "Show networking options", nullptr, nullptr);

static GOptionEntry v4l2Entries[]{
        {"use_omx",     'o', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE,   &hw_encoder,  "Use OpenMAX hardware acceleration (default: false)",                                          nullptr},
        {"camera_h264", 'c', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE,   &cam_encoder, "Use camera-provided h.264 feed for higher-end cameras (e.g. Logitech C920) (default: false)", nullptr},
        {"device",      'd', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &video_dev,   "Video4Linux2 device or shared memory socket to use (default: /dev/video0)",                   "DEVICE"},
        {nullptr}
};
GOptionGroup *v4l2Opts = g_option_group_new("videnc", "Video encoder options", "Show video encoding options", nullptr,
                                            nullptr);

static GOptionEntry rpiCamEntries[]{
        {"preview",  'p', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &prev, "Display preview window overlay (default: false)", nullptr},
        {"rotation", 0,   G_OPTION_FLAG_NONE, G_OPTION_ARG_INT,  &rot,  "Video rotation in degrees (default: 0)",          "DEGREES"},
        {nullptr}
};
GOptionGroup *rpiCOpts = g_option_group_new("rpic", "Raspberry Pi camera module options",
                                            "Show Raspberry Pi camera options", nullptr, nullptr);

void init_options() {
    g_option_group_add_entries(netOpts, netEntries);
    g_option_group_add_entries(rpiCOpts, rpiCamEntries);
    g_option_group_add_entries(v4l2Opts, v4l2Entries);
}

int *rotation() { return &rot; }

bool *preview() { return &prev; }

V4L2Encoders selected_encoder() {
    if (cam_encoder && !shared_mem) {
        // shared memory is unencoded
        return V4L2Encoders::CAMERA_H264;
    } else if (hw_encoder) {
        return V4L2Encoders::OPENMAX;
    } else {
        return V4L2Encoders::SOFTWARE;
    }
}

VideoSources input_type() {
    if (shared_mem) {
        return VideoSources::SHMEM;
    } else if (rpi_cam_flag) {
        return VideoSources::RASP;
    } else {
        return VideoSources::V4L2;
    }
}

std::string video_device() { return std::string(video_dev); }

int *video_height() { return &height; }

int *video_width() { return &width; }

int *framerate() { return &fps; }

const char *address() { return addr; }

int *port() { return &port_; }

std::string mount() {
    if (url != nullptr) {
        std::string ret = std::string(url);
        if (ret[0] == '/') {
            return ret;
        } else {
            ret.insert(0, 1, '/');
            return ret;
        }
    } else {
        return std::string(DEFAULT_MOUNT);
    }
}

GOptionEntry *get_main_opts() {
    return entries;
}

GOptionGroup *get_rpi_opts() {
    return rpiCOpts;
}

GOptionGroup *get_v4l2_opts() {
    return v4l2Opts;
}

GOptionGroup *get_net_opts() {
    return netOpts;
}

bool judgemental() { return judge; }
