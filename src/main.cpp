#include <iostream>
#include <string>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

int main(int argc, char *argv[]) {
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;
    GSocket *socket;
    GSource *source;
    GError *err = NULL;

    gst_init (&argc, &argv);
    loop = g_main_loop_new(NULL, false);

    server = gst_rtsp_server_new();
    mounts = gst_rtsp_server_get_mount_points(server);
    factory = gst_rtsp_media_factory_new();
    // string = pipeline to use
    // TODO: Use rpicamsrc if available
    gst_rtsp_media_factory_set_launch(factory, "v4l2src ! video/x-raw,format=YUY2 ! videoconvert ! x264enc tune=zerolatency ! rtph264pay name=pay0");
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
