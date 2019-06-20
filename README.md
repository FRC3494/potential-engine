# `potential-engine`

:steam_locomotive:

A pretty simple RTSP (**R**eal **T**ime **S**treaming **P**rotocol) server.

## Dependencies
### Building
* `cmake` version 3.1 or greater
* [`fmt`](http://fmtlib.net/latest/index.html) version 5.3.0 (or greater?)
  * Note that the version of `fmt` included in Debian Stable (and Raspbian Stable) is **not** up-to-date. If you are using either of these systems you **must** install `fmt` from source. 
* `gio-2.0` and `glib-2.0`
* `gstreamer-1.0`
* `gstreamer-rtsp-server-1.0`

`cmake` finds all build dependencies with `pkg-config`. **Your compiler must support C++11.**

### Runtime
* [`gst-rpicamsrc`](https://github.com/thaytan/gst-rpicamsrc): for Raspberry Pi Camera users. At time of writing this must be installed from source. Their [README file](https://github.com/thaytan/gst-rpicamsrc/blob/master/README) details both building and installation.
* `gstreamer1.0-omx` (should be available in your distro's packages): for hardware-accelerated video encoding.
  * Raspberry Pi users should also install `gstreamer1.0-omx-rpi`
* `gstreamer1.0-plugins-ugly`: for software encoding with `x264enc`
* `gstreamer1.0-plugins-bad`: for shared memory video source (useful for integrating with other applications such as Open CV vision processing)

On Raspbian, you can install everything you need to build and run this repository **except `fmt`** with the following mouthful of a command:
```bash
sudo apt update && sudo apt upgrade && sudo apt install git cmake pkg-config gstreamer1.0-rtsp gstreamer1.0-plugins-bad gstreamer1.0-omx-rpi gstreamer1.0-omx libgstreamer1.0-dev libgstrtspserver-1.0-dev
```
If you need to install `fmt` from source, run the following:
```bash
cd # go home
git clone https://github.com/fmtlib/fmt.git # download the source
cd fmt && mkdir build && cd build # create build files directory
cmake .. # create build files
sudo make install # build and install {fmt}
```

## Building, Running and User's Manual
```bash
cd # go home
git clone https://github.com/BHSSFRC/potential-engine.git
cd potential-engine && mkdir build && cd build
cmake ..
make
```
`make` will produce an executable named `potential-engine` in the `build` folder. (If you'd like to install the project to use it from any directory, run `sudo make install` instead of `make`.) To see what options are available, use `./potential-engine --help`.  
Running with no options is functionally identical to `./potential-engine --address 0.0.0.0 --port 1181 --url /stream`. The server does not use the Raspberry Pi Camera Module or OpenMAX hardware acceleration unless explicity told to with `--rpi_cam` or `--use_omx` respectively.  
If you have a very nice camera (such as the [Logitech C920](https://www.logitech.com/en-us/product/hd-pro-webcam-c920s)) which outputs h.264 video natively, add the `--camera_h264` flag at runtime instead of `--use_omx`.
