#include <glib.h>
#include <string>

#include "pipe_builder.h"

VideoSources input_type();

int *rotation();

bool *preview();

V4L2Encoders selected_encoder();

std::string video_device();

int *video_height();

int *video_width();

int *framerate();

const char *address();

int *port();

std::string mount();

void init_options();

GOptionEntry *get_main_opts();

GOptionGroup *get_rpi_opts();

GOptionGroup *get_v4l2_opts();

GOptionGroup *get_net_opts();

bool judgemental();
