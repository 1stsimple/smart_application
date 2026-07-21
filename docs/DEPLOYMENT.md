# Ubuntu 22.04 部署

## Docker Compose

先修改 `docker-compose.yml` 与 `deploy/server.docker.conf` 中三个示例密码/密钥及
`hls_public_url`，再执行：

```bash
docker compose up --build -d
docker compose logs -f server
```

MySQL 首次启动自动执行 `database/001_users.sql`、`002_recordings.sql` 和
`003_cameras.sql`。已存在的数据卷不会重复初始化，升级时应手工执行新增迁移。

使用 NFS 存储柜时设置 `NFS_SERVER`、`NFS_EXPORT`，并叠加覆盖文件：

```bash
docker compose -f docker-compose.yml -f deploy/docker-compose.nfs.yml up -d
```

## 裸机/systemd

安装构建依赖后执行 CMake 构建，把服务器放到 `/opt/smart-home/bin`，配置放到
`/etc/smart-home/server.conf`。将 NFS/SMB 挂载到配置的 `video_path`，并通过
`/etc/fstab` 的 `_netdev` 选项确保网络就绪后挂载。创建无登录的 `smart-home`
用户，授予录像与日志目录写权限，再安装并启用 `deploy/smart-home-monitor.service`。

生产检查项：替换所有示例密钥；数据库只开放给服务器；TCP/HLS 端口置于防火墙或
反向代理后；NTP 保持服务器和客户端时间一致；监控日志中的 reconnect、corrupt、
maintenance failed；关键录像用 NAS 快照或 rsync 做异地备份。
