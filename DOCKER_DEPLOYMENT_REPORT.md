# 宝可梦 MUD 游戏 —— Docker + GHCR 完整部署报告

> 项目名：team_Software_Engineering_project  
> GitHub：https://github.com/erlang-yao/team_Software_Engineering_project  
> GHCR 镜像：`ghcr.io/erlang-yao/pokemon-mud-server` / `ghcr.io/erlang-yao/pokemon-mud-client`  
> 生成日期：2026-05-17

---

## 一、本次新增 / 修改的文件清单

### 新增文件（8 个）

| 文件 | 用途 |
|---|---|
| `server/Dockerfile` | 后端 Node.js 镜像的构建文件（多阶段构建） |
| `server/.dockerignore` | 后端打包时排除 node_modules / dist 等无关文件 |
| `client/Dockerfile` | 前端 React 镜像的构建文件（多阶段构建 + nginx） |
| `client/.dockerignore` | 前端打包时排除无关文件 |
| `client/nginx.conf` | nginx 配置：提供静态文件 + 将 /api 请求代理到后端 |
| `docker-compose.yml` | 一键编排后端 + 前端两个容器（从源码构建） |
| `docker-compose.prod.yml` | 使用 GHCR 预构建镜像，无需源码即可启动 |
| `.github/workflows/docker.yml` | GitHub Actions CI：代码 push 到 main 后自动构建并推送镜像到 GHCR |

### 修改文件（2 个）

| 文件 | 改动 |
|---|---|
| `client/src/App.tsx` | 移除 StartScreen 组件上未使用的 `skipToGame` prop |
| `client/src/components/screens/StartScreen.tsx` | 移除未使用的 `Props` 接口定义 |

---

## 二、项目架构（Docker 部署视角）

```
用户浏览器 :5173
       │
       ▼
┌──────────────────────┐
│  frontend 容器        │  ← nginx (端口 80 映射到宿主机 5173)
│  /         → React   │
│  /api/*    → 代理    │
└──────────┬───────────┘
           │ proxy_pass http://backend:4010
           ▼
┌──────────────────────┐
│  backend 容器         │  ← Node.js Express (端口 4010)
│  14 个 REST API       │
│  会话管理 (内存)       │
└──────────────────────┘
```

---

## 三、各文件内容详解

### 3.1 `server/Dockerfile`

```dockerfile
# ---- Stage 1: Build ----
FROM node:20-alpine AS builder
WORKDIR /app
COPY package.json package-lock.json ./
RUN npm ci
COPY tsconfig.json ./
COPY src/ ./src/
RUN npm run build

# ---- Stage 2: Production runtime ----
FROM node:20-alpine AS runner
WORKDIR /app
COPY package.json package-lock.json ./
RUN npm ci --omit=dev
COPY --from=builder /app/dist ./dist
ENV PORT=4010
EXPOSE 4010
CMD ["node", "dist/index.js"]
```

**设计要点：**
- **多阶段构建**：阶段1安装全部依赖 + TypeScript 编译；阶段2只装生产依赖 + 复制编译产物。最终镜像体积约 150MB。
- 基础镜像用 `node:20-alpine`（约 50MB），而不是 `node:20`（约 350MB）。

### 3.2 `client/Dockerfile`

```dockerfile
# ---- Stage 1: Build React app ----
FROM node:20-alpine AS builder
WORKDIR /app
COPY package.json package-lock.json ./
RUN npm ci
COPY tsconfig.json vite.config.ts index.html ./
COPY src/ ./src/
COPY public/ ./public/
RUN npm run build

# ---- Stage 2: Serve with nginx ----
FROM nginx:alpine
COPY nginx.conf /etc/nginx/conf.d/default.conf
COPY --from=builder /app/dist /usr/share/nginx/html
EXPOSE 80
CMD ["nginx", "-g", "daemon off;"]
```

### 3.3 `client/nginx.conf`

```nginx
server {
    listen 80;
    location / {
        root /usr/share/nginx/html;
        index index.html;
        try_files $uri /index.html;    # SPA 路由回退
    }
    location /api/ {
        proxy_pass http://backend:4010; # 代理到后端容器
        proxy_http_version 1.1;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

### 3.4 `docker-compose.yml`

```yaml
services:
  backend:
    build: ./server
    ports:
      - "4010:4010"
  frontend:
    build: ./client
    ports:
      - "5173:80"
    depends_on:
      - backend
```

### 3.5 `.github/workflows/docker.yml`

```yaml
name: Docker Build and Push
on:
  push:
    branches: [ main ]
  workflow_dispatch:          # 允许手动触发

jobs:
  server:
    runs-on: ubuntu-latest
    permissions:
      contents: read
      packages: write          # 允许往 GHCR 推送
    steps:
      - uses: actions/checkout@v4
      - name: Log in to GHCR
        uses: docker/login-action@v3
        with:
          registry: ghcr.io
          username: ${{ github.repository_owner }}
          password: ${{ secrets.GITHUB_TOKEN }}
      - name: Build and push server image
        uses: docker/build-push-action@v5
        with:
          context: ./server
          push: true
          tags: |
            ghcr.io/${{ github.repository_owner }}/pokemon-mud-server:latest
            ghcr.io/${{ github.repository_owner }}/pokemon-mud-server:${{ github.sha }}

  client:
    runs-on: ubuntu-latest
    permissions:
      contents: read
      packages: write
    steps:
      - uses: actions/checkout@v4
      - name: Log in to GHCR
        uses: docker/login-action@v3
        with:
          registry: ghcr.io
          username: ${{ github.repository_owner }}
          password: ${{ secrets.GITHUB_TOKEN }}
      - name: Build and push client image
        uses: docker/build-push-action@v5
        with:
          context: ./client
          push: true
          tags: |
            ghcr.io/${{ github.repository_owner }}/pokemon-mud-client:latest
            ghcr.io/${{ github.repository_owner }}/pokemon-mud-client:${{ github.sha }}
```

**标签说明**：同时打两个 tag —— `latest`（始终指向最新）和 `${{ github.sha }}`（commit SHA 版本固定，方便回滚）。

---

## 四、推送到 GitHub 并触发镜像构建（完整步骤）

> 以下步骤，任何人在任何电脑上都可以按顺序操作。

### 第 1 步：确保代码已提交并推送到 GitHub

打开终端（PowerShell / CMD / Git Bash），进入项目目录，依次执行：

```bash
# 进入项目目录
cd d:\vscode_c\C++\soft4\team_Software_Engineering_project

# 添加所有新文件和修改
git add server/Dockerfile server/.dockerignore
git add client/Dockerfile client/.dockerignore client/nginx.conf
git add docker-compose.yml
git add .github/workflows/docker.yml
git add client/src/App.tsx client/src/components/screens/StartScreen.tsx

# 提交
git commit -m "Add Docker deployment configs and GHCR CI workflow"

# 推送到 GitHub
git push origin main
```

### 第 2 步：GitHub Actions 自动构建

推送完成后：
1. 打开浏览器，访问 https://github.com/erlang-yao/team_Software_Engineering_project
2. 点击顶部 **Actions** 标签页
3. 你会看到一个正在运行的 workflow（名字：**Docker Build and Push**）
4. 等待约 3-5 分钟，直到圆圈变成绿色 ✓
5. 如果出现红色 ✗，点击该 workflow → 展开失败的 job → 查看日志找原因

### 第 3 步：验证镜像已推送成功

打开浏览器，访问：
- https://github.com/erlang-yao/team_Software_Engineering_project/pkgs/container/pokemon-mud-server
- https://github.com/erlang-yao/team_Software_Engineering_project/pkgs/container/pokemon-mud-client

你应该能看到带 `latest` 标签的镜像包。

> **注意**：GHCR 镜像默认是 **private**（私有的）。如果你希望任何人都能 `docker pull`，需要手动设置为 public：
> 1. 打开上述 Packages 链接
> 2. 点击 **Package settings**
> 3. 拉到 **Danger Zone** → 点击 **Change visibility** → 选择 **Public**

---

## 五、任何人如何拉取并运行（零基础指南）

### 方式 A：从 GHCR 直接拉取，一条 docker compose 命令启动（推荐，不需要源代码）

适用场景：你只想玩游戏，不想编译代码，也不想理解 Docker 网络。

**第 1 步**：下载 `docker-compose.prod.yml` 文件

在终端中执行：
```bash
curl -O https://raw.githubusercontent.com/erlang-yao/team_Software_Engineering_project/main/docker-compose.prod.yml
```

如果 curl 不可用，也可以直接浏览器打开：
https://raw.githubusercontent.com/erlang-yao/team_Software_Engineering_project/main/docker-compose.prod.yml
右键 → 另存为 → 保存到任意文件夹，文件名改为 `docker-compose.yml`

**第 2 步**：一键拉取并启动
```bash
docker compose up -d
```

**第 3 步**：浏览器打开 **http://localhost:5173** 即可游戏。

> 这个 compose 文件会自动下载 GHCR 镜像、创建共享网络、启动两个容器。不会出现 `host not found` 的问题。

---

### 方式 B：纯 docker run 命令（理解原理用）

如果你想知道背后发生了什么，或者只能使用 `docker run`：

```bash
# 1. 拉取镜像
docker pull ghcr.io/erlang-yao/pokemon-mud-server:latest
docker pull ghcr.io/erlang-yao/pokemon-mud-client:latest

# 2. 创建共享网络（必须！否则前端 nginx 找不到后端）
docker network create pokemon-net

# 3. 启动后端（连到共享网络）
docker run -d -p 4010:4010 --name pokemon-backend --network pokemon-net ghcr.io/erlang-yao/pokemon-mud-server:latest

# 4. 启动前端（连到同一个共享网络）
docker run -d -p 5173:80 --name pokemon-frontend --network pokemon-net ghcr.io/erlang-yao/pokemon-mud-client:latest

# 5. 浏览器打开 http://localhost:5173
```

### 方式 C：从源码构建（需要源代码）

适用场景：你有完整的项目源代码，想自己构建镜像。

```bash
# 1. 进入项目目录
cd team_Software_Engineering_project

# 2. 一键构建并启动
docker compose up -d --build

# 3. 浏览器打开 http://localhost:5173
```

### 验证服务是否正常运行

```bash
# 检查后端健康状态
curl http://localhost:4010/api/health

# 应该返回：
# {"status":"ok","timestamp":1715932800000}
```

### 停止服务

```bash
# 方式 A / C（compose 启动的）
docker compose down

# 方式 B（手动 docker run 启动的）
docker stop pokemon-backend pokemon-frontend
docker rm pokemon-backend pokemon-frontend
docker network rm pokemon-net
```

---

## 六、推送流程总览

```
┌─────────────┐     git push      ┌──────────────────┐
│  本地代码    │ ────────────────→ │  GitHub 仓库      │
│  (你的电脑) │                   │  (github.com)     │
└─────────────┘                   └────────┬─────────┘
                                           │ 触发
                                           ▼
                                  ┌──────────────────┐
                                  │  GitHub Actions   │
                                  │  docker.yml       │
                                  │                   │
                                  │  1. docker login  │
                                  │  2. docker build  │
                                  │  3. docker push   │
                                  └────────┬─────────┘
                                           │ 推送到
                                           ▼
                                  ┌──────────────────┐
                                  │  GHCR 镜像仓库     │
                                  │  ghcr.io/         │
                                  │  erlang-yao/      │
                                  │  pokemon-mud-*    │
                                  └────────┬─────────┘
                                           │ docker pull
                                           ▼
                                  ┌──────────────────┐
                                  │  任何人的电脑     │
                                  │  docker run ...   │
                                  └──────────────────┘
```

---

## 七、常见问题速查

| 问题 | 原因 | 解决方法 |
|---|---|---|
| `docker: command not found` | 没装 Docker Desktop | 去 docker.com 下载安装 |
| `port is already allocated` | 端口被占用 | `netstat -ano \| findstr 4010` 查占用，`taskkill /PID <pid> /F` 杀进程 |
| 前端能打开但 API 报错 | 后端没启动 或 nginx 代理失败 | 确认 `docker ps` 看到两个容器都在运行 |
| `docker pull` 报 `unauthorized` | GHCR 包是私有的 | 在 GitHub → Packages → Package settings 改为 Public |
| GitHub Actions 失败 | CI 日志有具体报错 | 打开 Actions → 点击失败的 job → 查看日志 |
| 改了代码但镜像还是旧的 | 没有重新 push / 缓存 | push 新代码到 main 分支，CI 会自动重新构建 |
| `npm ci` 失败（Docker 内） | lockfile 损坏 | 删除 server/package-lock.json，运行 `npm install` 重新生成 |

---

## 八、GitHub Actions 所需权限

本项目的 CI 需要 `packages: write` 权限（已在 workflow 文件中声明）。如果你的仓库是 Fork 的，请确认：

1. 仓库 → Settings → Actions → General → Workflow permissions
2. 选择 **Read and write permissions**
3. 勾选 **Allow GitHub Actions to create and approve pull requests**

---

## 九、后续扩展建议

1. **添加数据库**：在 `docker-compose.yml` 中增加 PostgreSQL / MongoDB 服务，替换当前的内存存储
2. **添加健康检查**：在 Docker Compose 中配置 `healthcheck`，确保后端就绪后前端才接收流量
3. **版本管理**：CI 已同时打 `latest` 和 `sha` 标签，可以方便回滚到任意版本
4. **HTTPS**：在前端容器前加一层 Traefik / Caddy 反向代理，自动申请 Let's Encrypt 证书
