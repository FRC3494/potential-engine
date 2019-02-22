#include <glib.h>

bool use_rpi_cam();

int* rotation();
bool* preview();

bool* use_hw_encoder();

int* video_height();
int* framerate();

char* address();
int* port();
char* mount();

void init_options();

GOptionEntry* get_main_opts();
GOptionGroup* get_rpi_opts();
GOptionGroup* get_v4l2_opts();
GOptionGroup* get_net_opts();
