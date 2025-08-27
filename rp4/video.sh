#!/bin/bash
gst-launch-1.0 v4l2src device=/dev/video0 ! \
video/x-raw,width=640,height=480,framerate=30/1 ! \
videoconvert ! x264enc tune=zerolatency speed-preset=ultrafast bitrate=1000 ! \
rtph264pay config-interval=1 pt=96 ! \
udpsink host=192.168.1.109 port=5000
