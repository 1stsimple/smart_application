# Smart Home Monitor

基于需求文档实现的智能家居监控系统。服务器使用 C++11，完整实现自定义
TLV 协议、Reactor + ThreadPool、用户认证、摄像头 HTTP/PTZ 控制、
RTSP/RTMP 中转、录像分片和 HLS 回放；桌面客户端使用 Qt。

## 当前状态

- [x] 工程骨架、配置读取、日志基础
- [x] TLV 编解码与流式拆包
- [x] Reactor TCP 服务器与线程池
- [x] 两阶段注册/登录、随机盐与用户仓储接口
- [x] MySQL 用户仓储适配器
- [x] 摄像头 HTTP API 签名与云台请求模型
- [x] libcurl/Socket HTTP 传输与已认证 TLV 转发
- [x] AVPacket 网络序列化、分片重组与 RingBuffer
- [x] FFmpeg RTSP/RTMP 数据源与异步媒体会话
- [x] 已认证的开始/停止流 TLV 接入
- [x] 录像索引、HLS VOD 与挂载式网络存储
- [x] FFmpeg TS 分片录制与录像校验索引
- [x] HLS HTTP 播放列表与 TS 文件服务
- [x] 已认证回放 URL 的 TLV 查询
- [x] 录像任务启停、状态查询与后台分片调度
- [x] MySQL 录像索引持久化与重启后回放查询
- [x] 摄像头设备表、设备列表序列化及登录后查询协议
- [x] MySQL 摄像头设备目录持久化
- [x] Qt 客户端：认证、设备、实时解码、云台、录像与 HLS 回放
- [x] 录像断流自动重连与 HLS 不连续标记
- [x] 网络存储保留期/容量回收、MD5 巡检与失效索引清理
- [x] HLS 限时签名鉴权与 TS 大文件分块传输
- [x] Docker/NFS/systemd 部署与 Ubuntu 全依赖 CI

## 构建

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/SmartHomeMonitorServer ./conf/server.conf
```

不安装 MySQL 的本地联调可改用 `./conf/server.memory.conf`。

协议、架构和生产部署分别见 `docs/PROTOCOL.md`、`docs/ARCHITECTURE.md`、
`docs/DEPLOYMENT.md`。快速联调可在替换示例密码和密钥后执行
`docker compose up --build -d`。

Windows 多配置生成器的可执行文件通常位于 `build/Debug`。

配置文件使用 `key value` 格式，支持整行及行尾 `#` 注释；重复键、格式错误、
缺少必填项或越界数字都会让服务器拒绝启动，避免带着错误配置运行。

网络消息使用 `[Type:4B][Length:4B][Value:Length B]`，两个整数均为网络
字节序。流式解码器支持 TCP 半包、粘包、空消息和连续多消息，并限制消息体
最大长度，避免恶意长度字段耗尽内存。

TCP 服务器使用 Reactor 风格事件循环接收连接和网络数据，并将完整 TLV
消息提交给有界线程池。任务队列满时会立即返回错误，避免无限制堆积；连接
发送带互斥保护，支持业务工作线程安全回包。网络层兼容 Linux sockets 与
Windows Winsock，生产部署目标仍为 Ubuntu 22.04。

认证服务实现文档中的两阶段流程：服务器先返回每个用户独立的 crypt
`setting`，客户端计算密文后再提交。用户名经过白名单校验，密文使用常量
时间比较。`database/001_users.sql` 提供 MySQL 表结构，自动化测试通过线程
安全的内存仓储验证认证业务，生产环境可切换到 MySQL 仓储。

发现 MySQL/MariaDB 客户端开发库时，CMake 会自动编译
`MysqlUserRepository`。查询与插入均使用 prepared statement，不拼接用户
输入；单连接访问带互斥保护。部署前执行 `database/001_users.sql` 建表。
Ubuntu 默认启用该适配器；Windows 默认关闭，因为客户端库必须与所选
MSVC/MinGW 工具链匹配，可用 `-DSMART_HOME_WITH_MYSQL=ON` 显式开启。

注册和登录已接入 TCP `TaskType` 分发。Section1 成功后，会话绑定到当前
`TcpConnection`；Section2 必须从同一连接按正确操作继续，断线自动清理，
从而避免不同客户端串用认证阶段。配置 `database_driver=mysql` 使用持久化
仓储，开发测试可显式设置为 `memory`。
