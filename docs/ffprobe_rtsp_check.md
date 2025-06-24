# Ffprobe to check whether a rtsp video streaming is available
```
ffprobe -v error -select_streams v:0 -show_entries stream=codec_name,width,height,avg_frame_rate -of json rtsp://<username>:<password>@<ip_address>:<port>/path
```

> **Explaination**:
>> **-v error** suppress all output except errors. \
>> **-select_streams v:0** selects the first video stream only. \
>> **-show_entries stream=...** limits output to specific fields (you can add more fields as needed). \
>> **-of json** outputs in JSON format. \
>> **rtsp://...** replace with your actual RTSP URL. \
>> **-show_streams -show_format** want complete stream information (video, audio, etc)

