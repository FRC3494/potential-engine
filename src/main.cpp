#include <iostream>
#include <string>
#include <fmt/core.h>
#include <getopt.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

static int rpi_cam_flag = 0;
static int use_hw_encoder = 0;
std::string video_height = "480";
std::string framerate = "30";

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
        {"fps", required_argument, 0, 'f'},
        {0, 0, 0, 0}
    };
    while (1) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "h:f:", long_options, &option_index);
        if (c == -1) {
            break;
        } else {
            switch(c) {
                case 0:
                    if (long_options[option_index].flag != 0) {
                        break;
                    }
                case 'h':
                    video_height = optarg;
                    break;
                case 'f':
                    framerate = optarg;
                    break;
            }
        }
    }
    std::string pipeline;
    if (rpi_cam_flag == 1) {
        std::map<std::string, std::string> resolutions;
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
        std::string encoder;
        if (use_hw_encoder == 0) {
            // don't hardware accelerate
            encoder = "x264enc tune=zerolatency";
        } else {
            encoder = "omxh264enc ! video/x-h264,profile=baseline";
        }
        pipeline = fmt::format(pipeline, fmt::arg("h", video_height), fmt::arg("f", framerate), fmt::arg("e", encoder));
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
