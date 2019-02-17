#include <map>
#include <string>
#include <fmt/core.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#define DEFAULT_RTSP_PORT "8554"
#define DEFAULT_RESOLUTION "480"
#define DEFAULT_FPS "30"
#define DEFAULT_MOUNT "/stream"

static char *port = (char *) DEFAULT_RTSP_PORT;

static bool rpi_cam_flag = false;
static bool use_hw_encoder = false;
static std::string video_height = (char *) DEFAULT_RESOLUTION;
static std::string framerate = (char *) DEFAULT_FPS;
static std::string mount = (char *) DEFAULT_MOUNT;

static GOptionEntry entries[] = {
    {"rpi_cam", 'r', 0, G_OPTION_ARG_NONE, &rpi_cam_flag, "Use Raspberry Pi Camera module (default: false)", NULL},
    {"fps", 'f', 0, G_OPTION_ARG_STRING, &framerate, "Framerate in FPS (default: " DEFAULT_FPS ")", "FPS"},
    {"use_omx", 'o', 0, G_OPTION_ARG_NONE, &use_hw_encoder, "Use OpenMAX hardware acceleration (default: false). Ignored if the Raspberry Pi Camera module is used.", NULL},
    {"height", 'h', 0, G_OPTION_ARG_STRING, &video_height, "Video height. Should be a standard resolution (in [240, 360, 480, 720] for most cameras.)", "HEIGHT"},
    {"url", 'u', 0, G_OPTION_ARG_STRING, &mount, "URL to stream video at. Must start with \"/\" (default: " DEFAULT_MOUNT ")", "URL"},
    {"port", 'p', 0, G_OPTION_ARG_STRING, &port, "Port to listen on (default: " DEFAULT_RTSP_PORT ")", "PORT"},
    {NULL}
};

int main(int argc, char *argv[]) {
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;
    GOptionContext *optctx;
    GError *error = NULL;

    optctx = g_option_context_new(" - Potential Engine RTSP Server\n"
                "\n"
                "Copyright (C) 2019 The Quadrangles FRC Team 3494\n"
                "This program comes with ABSOLUTELY NO WARRANTY, to the extent permitted by " 
                "applicable law. (You have been warned!)\n"
                "This is free software, and you are welcome to redistribute it "
                "under certain conditions; see LICENSE for details."
    );
    g_option_context_add_main_entries(optctx, entries, NULL);
    g_option_context_add_group(optctx, gst_init_get_option_group());
    if (!g_option_context_parse (optctx, &argc, &argv, &error)) {
        g_printerr ("Error parsing options: %s\n", error->message);
        g_option_context_free (optctx);
        g_clear_error (&error);
        return -1;
    }
    g_option_context_free (optctx);

    loop = g_main_loop_new(NULL, false);

    server = gst_rtsp_server_new();
    g_object_set(server, "service", port, NULL);
    mounts = gst_rtsp_server_get_mount_points(server);
    factory = gst_rtsp_media_factory_new();
    
    std::string pipeline;
    if (rpi_cam_flag) {
        std::map<std::string, std::string> resolutions;
        resolutions["144"] = "256";
        resolutions["240"] = "426";
        resolutions["360"] = "640";
        resolutions["480"] = "640";
        resolutions["720"] = "1280";
        pipeline = fmt::format(
            "rpicamsrc ! "
            "video/x-h264,height={},width={},framerate={}/1 ! "
            "h264parse ! "
            "rtph264pay name=pay0",
            video_height,
            resolutions[video_height],
            framerate
        );
    } else {
        pipeline = "v4l2src ! "
            "video/x-raw,format=YUY2,height={h},framerate={f}/1 ! "
            "videoconvert ! "
            "video/x-raw,format=I420 ! "
            "{e} ! rtph264pay name=pay0";
        std::string encoder = "x264enc tune=zerolatency";
        if (use_hw_encoder) {
            // hardware accelerate
            encoder = "omxh264enc ! video/x-h264,profile=baseline";
        }
        pipeline = fmt::format(pipeline, fmt::arg("h", video_height), fmt::arg("f", framerate), fmt::arg("e", encoder));
    }
    
    g_print("Starting pipline : %s", pipeline.c_str());
    gst_rtsp_media_factory_set_launch(factory, pipeline.c_str());
    gst_rtsp_media_factory_set_shared(factory, true);
    gst_rtsp_mount_points_add_factory(mounts, mount.c_str(), factory);
    // free thing we're no longer using
    g_object_unref(mounts);
    // start rtsp server using info in server obj, ignoring errors
    gst_rtsp_server_attach(server, NULL);
    g_print("stream ready at rtsp://127.0.0.1:%s%s\n", port, mount.c_str());
    g_main_loop_run(loop);
    
    return 0;
}
