# V3 客户端功能与验证说明

## 本期实现

- 新增独立的“添加设备”窗口，支持设备名称、类型、通道数、地址、端口、设备账号、设备密码、RTSP/RTMP、主码流和备用码流，并可在保存前执行 TCP 网络测试。
- 实时预览改为多画面工作区。每个通道拥有独立的 TLV/TCP 登录连接和 FFmpeg 解码线程，最多同时打开 9 路，自动切换单画面、双画面、四画面、六画面和九宫格布局。
- 设备树支持双击预览、右键停止、重新连接、查看信息和删除设备；正在预览的通道会显示状态。
- 当前活动画面支持截图、全屏、开始/停止录像及球机云台控制。云台齿轮支持鼠标拖动、八方向、距离映射速度、命令节流和松手自动停止。
- 录像中心使用日期时间控件查询服务端真实录像索引，支持播放精确分片、断点安全的临时文件下载、进度、速度、取消、重试、文件大小和 MD5 校验。
- 客户端连接、账号、主题、窗口布局、推流参数和音效设置继续通过本地 INI 保存。运行时配置文件不进入 Git。

## 协议扩展

原有任务编号保持不变，新任务追加在枚举末尾：

| 任务 | Value |
| --- | --- |
| `QueryRecordingSegments` | `<camera_id> <channel> <begin_ms> <end_ms>` |
| `RecordingSegments` | 大端二进制录像分片列表，含 ID、设备、通道、起止时间、大小、断流标记、文件名、下载 URL 和 MD5 |
| `RecordingOperationResult` | `<camera_id> <channel> <active> <success> <message>` |
| `DeleteCamera` | `<camera_id>` |
| `CameraDeleted` | `<camera_id>` |

## 运行限制

- 当前服务端实时媒体协议只发送视频 codec metadata 和视频 AVPacket，不包含音频流。因此画面内音量和静音控件会明确禁用，不能伪装成可用功能。
- 设备表目前每台设备保存一个 RTSP 和一个 RTMP 地址。多通道设备可在 URL 中使用 `{channel}` 或 `%CHANNEL%` 占位符，预览时由客户端替换。
- 删除设备只删除设备目录项，不自动删除历史录像，避免误删录像证据。

## 验证

1. 在 Ubuntu 虚拟机更新代码并重新构建服务端镜像：

   ```bash
   cd ~/docker-projects/smart_application
   git pull
   docker compose up --build -d
   docker compose ps
   docker compose logs --tail=100 server
   ```

2. Windows 运行 V3 客户端，连接虚拟机地址和端口 `8000`，完成登录。
3. 添加一个 RTSP/RTMP 设备，先执行网络测试，再保存并刷新设备。
4. 双击多个通道，检查自动布局、活动边框、截图、全屏与停止单路预览。
5. 球机设备测试齿轮拖动和松手停止；枪机应禁用云台。
6. 开始录像，等待服务端成功响应后再观察录像状态；停止后进入录像中心按时间查询、播放和下载。
