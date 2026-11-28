# C Exam Kit · C99 函数资源库

Routilet 的离线 C99 函数组件与中文速查手册。通过 [Routilet](https://github.com/Routilet/routilet) 安装，在 [Companion](https://github.com/Routilet/companion) 中按 Ctrl/Cmd+H 打开手册；题目绑定资源快照，测试和提交按依赖装配代码。

## 开发结构

- `src/`：C 实现与相邻文档注释（公开符号、依赖、标签、说明、示例）。
- `kit.json`：语言、系统头和资源身份；`GUIDE.md`：全局使用约定。
- `scripts/build.mjs`：生成 `dist/package/preset/manifest.json`（v2 装配协议）及 `reference.json`（v2 扁平条目），同时生成可发布 npm 包根。
- `package.json`：唯一发行版本来源；`tests/`：C99 编译、运行和示例回归。

```sh
npm run build
python3 tests/test_lightweight_io.py
python3 tests/test_lightweight_algorithms.py
npm pack ./dist/package
```

注释规范见 [REFERENCE.md](REFERENCE.md)。源码实现与装配协议保持稳定；新版资源仅发行扁平手册 v2，请先更新 Routilet、Companion。历史快照不覆盖、不迁移。

## 发行

PR 构建与测试，不发布；main 上的新版本在自动测试通过后经 npm Trusted Publishing (OIDC) 发布，正式版本使用 latest，测试版本使用 next。版本已存在则跳过，registry 查询异常时停止。数据包仍使用 preset/manifest.json 和 preset/reference.json；不维护旧版目录投影。授权见 [LICENSING.md](LICENSING.md)。

历史单文件生成器、考试 PDF 与早期手册见 [v0.6.4](https://github.com/Routilet/c-exam-kit/releases/tag/v0.6.4)。
