#include <iostream>
#include <string>
#include <fmt/core.h>
#include <getopt.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

static int rpi_cam_flag = 0;
static int use_hw_encoder = 0;
std::string video_height = "480";

int main(int argc, char *argv[]) {
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;
    GSocket *socket;
    GSource *source;
    GError *err = NULL;

    // pass no options to GStreamer so we can have our own
    gst_init (NULL, NULL);
    loop = g_main_loop_new(NULL, false);

    server = gst_rtsp_server_new();
    mounts = gst_rtsp_server_get_mount_points(server);
    factory = gst_rtsp_media_factory_new();
    // parse options
    static struct option long_options[] = {
        {"rpi_cam", no_argument, &rpi_cam_flag, 1},
        {"no_rpi_cam", no_argument, &rpi_cam_flag, 0},
        {"hardware_accel", no_argument, &use_hw_encoder, 1},
        {"height", required_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    while (1) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "", long_options, &option_index);
        if (c == -1) {
            break;
        } else if (c == 'h') {
            video_height = optarg;
        }
    }
    std::string pipeline;
    if (rpi_cam_flag == 1) {
        pipeline = "rpicamsrc ! video/x-h264 ! h264parse ! rtph264pay name=pay0";
    } else {
        pipeline = "v4l2src ! video/x-raw,format=YUY2,height={} ! videoconvert ! video/x-raw,format=I420 ! {} ! rtph264pay name=pay0";
        if (use_hw_encoder == 0) {
            // don't hardware accelerate
            pipeline = fmt::format(pipeline, video_height, "x264enc tune=zerolatency");
        } else {
            pipeline = fmt::format(pipeline, video_height, "omxh264enc ! video/x-h264,profile=baseline");
        }
    }
    std::cout << "Starting pipeline: " + pipeline << std::endl;
    gst_rtsp_media_factory_set_launch(factory, pipeline.c_str());
    // g_signal_connect (factory, "media-configure", (GCallback) media_configure, NULL);
    // add stream at `/test`
    gst_rtsp_mount_points_add_factory(mounts, "/test", factory);
    // free thing we're no longer using
    g_object_unref(mounts);
    // start rtsp server using info in server obj, ignoring errors
    gst_rtsp_server_attach(server, NULL);
    std::cout << "Starting loop..." << std::endl;
    g_main_loop_run(loop);
    return 0;
}
