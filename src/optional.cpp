#include <stdlib.h>
#include <string>

#include "optional.h"

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#define DEFAULT_RTSP_PORT 1181
#define DEFAULT_RESOLUTION 480
#define DEFAULT_FPS 30
#define DEFAULT_MOUNT "/stream"

bool rpi_cam_flag = false;
bool judge = false;
// Raspberry-specific options
int rot = 0;
bool prev = false;
// V4L2 options
bool hw_encoder = false;
// common options
int height = DEFAULT_RESOLUTION;
int fps = DEFAULT_FPS;
// network
char* addr = (char *) "0.0.0.0";
int port_ = DEFAULT_RTSP_PORT;
std::string url = DEFAULT_MOUNT;
static gboolean validateMount(const char* option_name, const char* value, gpointer data, GError **error);

GOptionEntry entries[] = {
    {"rpi_cam", 'r', 0, G_OPTION_ARG_NONE, &rpi_cam_flag, "Use Raspberry Pi Camera module (default: false)", NULL},
    {"fps", 'f', 0, G_OPTION_ARG_INT, &fps, "Framerate in FPS (default: " STRINGIFY(DEFAULT_FPS) ")", "FPS"},
    {"height", 'h', 0, G_OPTION_ARG_INT, &height, "Video height. Should be a standard resolution (in [240, 360, 480, 720] for most cameras.)", "HEIGHT"},
    {"judge", '9', G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &judge, "", NULL},
    {NULL}
};

static GOptionEntry netEntries[] {
    {"address", 'a', 0, G_OPTION_ARG_STRING, &addr, "Network address to bind to (default: 0.0.0.0)", "ADDRESS"},
    {"port", 'p', 0, G_OPTION_ARG_INT, &port_, "Port to listen on (default: " STRINGIFY(DEFAULT_RTSP_PORT) ")", "PORT"},
    {"url", 'u', G_OPTION_FLAG_NONE, G_OPTION_ARG_CALLBACK, (gpointer) validateMount, "URL to stream video at (default: " DEFAULT_MOUNT ")", "URL"},
    {NULL}
};
GOptionGroup *netOpts = g_option_group_new("net", "Networking options", "Show networking options", NULL, NULL);

static GOptionEntry v4l2Entries[] {
    {"use_omx", 'o', 0, G_OPTION_ARG_NONE, &hw_encoder, "Use OpenMAX hardware acceleration (default: false)", NULL},
    {NULL}
};
GOptionGroup *v4l2Opts = g_option_group_new("v4l2", "Video4Linux2 options", "Show Video4Linux2 options", NULL, NULL); 

static GOptionEntry rpiCamEntries[] {
    {"preview", 'p', 0, G_OPTION_ARG_NONE, &prev, "Display preview window overlay (default: false)", NULL},
    {"rotation", 0, 0, G_OPTION_ARG_INT, &rot, "Video rotation in degrees (default: 0)", "DEGREES"},
    {NULL}
};
GOptionGroup *rpiCOpts = g_option_group_new("rpic", "Raspberry Pi camera module options", "Show Raspberry Pi camera options", NULL, NULL);

void init_options() {
    g_option_group_add_entries(rpiCOpts, rpiCamEntries);
    g_option_group_add_entries(v4l2Opts, v4l2Entries);
    g_option_group_add_entries(netOpts, netEntries);
}

static gboolean validateMount(const char* option_name, const char* value, gpointer data, GError **error) {
    std::string betterValue(value);
    bool ret = (betterValue[0] == '/');
    if (!ret) {
        betterValue.insert(0, 1, '/');
    }
    // strcpy(url, betterValue.c_str());
    url = betterValue;
    return true;
}


bool use_rpi_cam() { return rpi_cam_flag; }

int *rotation() { return &rot; }
bool *preview() { return &prev; }

bool *use_hw_encoder() { return &hw_encoder; }

int *video_height() { return &height; }
int *framerate() { return &fps; }

const char* address() { return addr; }
int *port() { return &port_; }
const char* mount() { return url.c_str(); }

GOptionEntry* get_main_opts() {
    return entries;
}

GOptionGroup* get_rpi_opts() {
    return rpiCOpts;
}

GOptionGroup* get_v4l2_opts() {
    return v4l2Opts;
}

GOptionGroup* get_net_opts() {
    return netOpts;
}

bool judgemental() { return judge; }

