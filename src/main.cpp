#include <map>
#include <string>
#include <fmt/core.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#include "networker.h"
#include "optional.h"
#include "pipe_builder.h"

int main(int argc, char *argv[]) {
    GMainLoop *loop = g_main_loop_new(NULL, false);
    GstRTSPServer *server = gst_rtsp_server_new();
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();
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
    // Add common options
    g_option_context_add_main_entries(optctx, get_main_opts(), NULL);
    // GST options
    g_option_context_add_group(optctx, gst_init_get_option_group());
    // setup options
    init_options();
    g_option_context_add_group(optctx, get_rpi_opts());
    g_option_context_add_group(optctx, get_v4l2_opts());
    g_option_context_add_group(optctx, get_net_opts());
    // parse options passed
    if (!g_option_context_parse (optctx, &argc, &argv, &error)) {
        g_printerr ("Error parsing options: %s\n", error->message);
        g_option_context_free (optctx);
        g_clear_error (&error);
        return -1;
    }
    g_option_context_free (optctx);

    // shell scope to prevent mem "leak"
    {
        const char* addr = address();
        bool b = isValidIP(addr);
        if (b) {
            gst_rtsp_server_set_address(server, addr); 
        } else {
            g_print("Invalid address %s passed, using 0.0.0.0\n", addr);
        }
    }
    // set port
    gst_rtsp_server_set_service(server, std::to_string(*port()).c_str());

    std::string pipeline;

    gboolean rpi = use_rpi_cam();
    if (rpi) {
        pipeline = raspberry_pipe(video_height(), framerate(), rotation(), preview());
    } else {
        pipeline = v4l2_pipe(video_height(), framerate(), use_hw_encoder());
    }

    g_print("Starting pipline: %s\n", pipeline.c_str());
    gst_rtsp_media_factory_set_launch(factory, pipeline.c_str());
    gst_rtsp_media_factory_set_shared(factory, true);
    std::string m = mount();
    const char* mount_cstr = m.c_str();
    g_print("Mounting to %s\n", mount_cstr);
    gst_rtsp_mount_points_add_factory(mounts, mount_cstr, factory);
    // free thing we're no longer using
    g_object_unref(mounts);
    // start rtsp server, ignoring errors
    gst_rtsp_server_attach(server, NULL);
    gchar* addr = gst_rtsp_server_get_address(server);
    g_print("stream starting at rtsp://%s:%d%s\n", addr, gst_rtsp_server_get_bound_port(server), mount_cstr);
    {
        bool b = judgemental();
        if (b) {
            g_print("        /\\\n"
                    " |9|   /==\\\n"
                    "  |    \\==/\n"
                    " (==)- /\\/\n"
                    "  ||--/ *ping*\n"
                    "  ||\n"
                    "  /\\\n"
                    "_/  \\_\n");
        }
    }
    g_free(addr);
    g_main_loop_run(loop);

    return 0;
}
