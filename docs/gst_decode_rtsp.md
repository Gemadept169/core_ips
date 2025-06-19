# Gstreamer plugins
| Plugin          | Description                                                    |
| --------------- | -------------------------------------------------------------- |
| `rtspsrc`       | Connects to RTSP source                                        |
| `rtph264depay`  | Extracts the H.264 video stream from RTP packets               |
| `h264parse`     | Parses H.264 stream into manageable chunks                     |
| `nvv4l2decoder` | NVIDIA hardware-accelerated H.264 decoder                      |
| `nvvidconv`     | NVIDIA converter (colorspace, format, scaling)                 |
| `nvoverlaysink` | Displays the video using NVIDIA overlay (zero-copy to display) |



# Debug level table
- Which is a number specifying the amount of desired output:

| #   | Name    | Description                                                     |
| --- | ------- | --------------------------------------------------------------- |
| 0   | none    | No debug information is output.                                 |
| 1   | ERROR   | Logs all fatal errors. These are errors that do not allow the   |
|     |         | core or elements to perform the requested action. The           |
|     |         | application can still recover if programmed to handle the       |
|     |         | conditions that triggered the error.                            |
| 2   | WARNING | Logs all warnings. Typically these are non-fatal, but           |
|     |         | user-visible problems are expected to happen.                   |
| 3   | FIXME   | Logs all "fixme" messages. Those typically that a codepath that |
|     |         | is known to be incomplete has been triggered. It may work in    |
|     |         | most cases, but may cause problems in specific instances.       |
| 4   | INFO    | Logs all informational messages. These are typically used for   |
|     |         | events in the system that only happen once, or are important    |
|     |         | and rare enough to be logged at this level.                     |
| 5   | DEBUG   | Logs all debug messages. These are general debug messages for   |
|     |         | events that happen only a limited number of times during an     |
|     |         | object's lifetime; these include setup, teardown, change of     |
|     |         | parameters, etc.                                                |
| 6   | LOG     | Logs all log messages. These are messages for events that       |
|     |         | happen repeatedly during an object's lifetime; these include    |
|     |         | streaming and steady-state conditions. This is used for log     |
|     |         | messages that happen on every buffer in an element for example. |
| 7   | TRACE   | Logs all trace messages. Those are message that happen very     |
|     |         | very often. This is for example is each time the reference      |
|     |         | count of a GstMiniObject, such as a GstBuffer or GstEvent, is   |
|     |         | modified.                                                       |
| 9   | MEMDUMP | Logs all memory dump messages. This is the heaviest logging and |
|     |         | may include dumping the content of blocks of memory.            |

# Gstreamer pipelines
```
gst-launch-1.0 -v rtspsrc location=rtsp://192.168.1.250:8555/cam latency=200 ! \
rtph264depay ! h264parse ! nvv4l2decoder ! \
nvvidconv ! nvv4l2h264enc ! h264parse ! mp4mux ! filesink location=output.mp4

gst-launch-1.0 -v rtspsrc location=rtsp://192.168.1.250:8555/cam latency=100 tcp-timeout=1 ! queue ! \
rtph264depay ! h264parse ! mp4mux ! filesink location=output.mp4

gst-launch-1.0 -v rtspsrc location=rtsp://192.168.1.250:8555/cam latency=100 tcp-timeout=1 ! appsink

gst-launch-1.0 -v rtspsrc location=rtsp://192.168.1.250:8555/cam latency=200 ! fakesink

rtspsrc location=rtsp://192.168.0.101:8555/cam latency=100 tcp-timeout=1 ! queue ! rtph264depay ! h264parse ! avdec_h264 output-corrupt=false ! autovideosink

curl -X DESCRIBE rtsp://192.168.1.250:8555/cam --max-time 5 -v

```
