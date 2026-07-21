# Qt 5.14 客户端

客户端实现两阶段注册/登录、摄像头目录、实时流 TLV 分片重组、FFmpeg 解码、
八方向云台、录像启停和 HLS 回放。Windows 使用需求文档指定的 Qt 5.14.2 与
FFmpeg 4.4 SDK。

Qt Creator 打开 `SmartHomeMonitorClient.pro`，在 qmake 环境中设置 `FFMPEG_ROOT`
为 FFmpeg SDK 根目录。运行时把 SDK 的 `avcodec`、`avutil`、`swscale` DLL 放在
可执行文件目录或加入 `PATH`。Ubuntu 可安装：

```bash
sudo apt install qtbase5-dev qtmultimedia5-dev libavcodec-dev libavutil-dev libswscale-dev
cmake -S . -B build
cmake --build build --target SmartHomeMonitorClient
```
