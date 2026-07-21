# 系统架构

```text
Qt 客户端 ──自定义 TLV/TCP──> Reactor + 有界线程池
   │                              ├─ 两阶段认证 ─> MySQL users
   │                              ├─ 设备目录 ───> MySQL cameras
   │                              ├─ HTTP/PTZ ───> 摄像头 HTTP API
   │                              ├─ FFmpeg 拉流 ─> RingBuffer ─> AVPacket 分片
   │                              └─ TS 分片 ────> NFS/SMB/NAS
   │                                                └─ MySQL recordings
   └────────限时签名 HLS/HTTP────────────────────────────┘
```

实时链路只转发编码后 AVPacket，客户端按服务器下发的 codec metadata 初始化解码器。
录像链路独立拉流，关键帧边界切 TS；断流重连后的首片标记 discontinuity。维护线程
按保留期和容量回收、核验 MD5。回放先经认证 TLV 获取短期 URL，再由 HLS HTTP 服务
动态生成 m3u8 并分块发送 TS。
