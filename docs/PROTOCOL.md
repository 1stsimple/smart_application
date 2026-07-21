# TLV 网络协议

所有整数采用网络字节序。每条消息为 `Type(u32) + Length(u32) + Value(bytes)`，
单条 Value 默认上限 16 MiB；TCP 解码器允许半包、粘包和连续多包。

认证采用同一连接上的两阶段流程：客户端先发送用户名，服务器返回该用户随机
`setting`，客户端计算 `MD5(setting + password)` 后发送第二阶段。注册与登录分别
使用 `RegisterSection1/2` 和 `LoginSection1/2`。业务消息必须在登录成功后发送。

主要业务 Value：

| 消息 | Value |
| --- | --- |
| `GetCameras` | 空；响应为计数及定长整数+长度前缀 UTF-8 字段 |
| `GetStream` | `<camera_id> <rtsp_or_rtmp_url>` |
| `StreamMetadata` | camera/codec/尺寸/时基/extradata 的大端二进制 |
| `StreamPacket` | packet-id、分片序号/总数、长度和 AVPacket 序列化片段 |
| `PtzControl` | `<channel> <command> <speed> <unix_seconds>` |
| `StartRecording` | `<camera_id> <channel> <url>` |
| `StopRecording` | `<camera_id> <channel>` |
| `GetPlaybackUrl` | `<camera_id> <channel> <begin_ms> <end_ms>` |

媒体包完整格式为 camera-id(u32)、packet-id(u32)、PTS(i64)、DTS(i64)、flags(u32)、
data-length(u32)、data。客户端必须按 packet-id 和连续分片序号重组，乱序包应丢弃。
