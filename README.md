# C Exam Kit · C99 脚手架

Routilet 维护的 C99 函数组件、版本化资源包和中文速查手册，面向 C 语言课程。主要交互入口为 [Accoding Companion](https://github.com/Routilet/accoding-companion)。Companion 将题目绑定到一个 preset 版本，本地测试和提交时按依赖装配代码。

所属组织：[Routilet](https://github.com/Routilet) · 原始设计者：Annouimo。

## 使用

- [下载 Companion 启动包](https://github.com/Routilet/accoding-companion/releases/latest)。启动器负责安装或更新 Companion 和 preset。
- 在 Companion 中按 `H` 打开工作区内对应版本的中文函数速查。
- [查看 preset 的接口说明](scaffold/presets/exam-io-c99/README.md)和 [npm 包](https://www.npmjs.com/package/@routilet/c-exam-kit)。

## 开发

组件、依赖清单和手册均在 `scaffold/presets/exam-io-c99/`。改动公开接口时按 [公开接口变更](scaffold/presets/exam-io-c99/README.md#公开接口变更) 核对速查、目录和发布说明。打包脚本从 `npm/accoding-preset/package.json` 读取版本，并生成 npm 包中的组件清单和手册。

```sh
python3 tests/test_lightweight_io.py
python3 tests/test_lightweight_algorithms.py
npm pack ./npm/accoding-preset
```

## npm 命名空间迁移

新资源发行使用 `@routilet/c-exam-kit`；原 `@annouimo/c-exam-kit-preset` 历史版本继续保留。旧课程中已保存的资源快照不会随包名变化。请使用新版 Companion 启动器安装新资源，不需要修改已有题目源码。

## 历史版本

单文件生成器和考试 PDF 已归档在 [v0.6.4 Release](https://github.com/Routilet/c-exam-kit/releases/tag/v0.6.4)。该标签保留完整的构建环境所需源码和配置。

| 资料 | 下载 |
| --- | --- |
| 单文件 C 生成器 | [c_scaffold_v0.6.4.c](https://github.com/Routilet/c-exam-kit/releases/download/v0.6.4/c_scaffold_v0.6.4.c)，下载后用 C99 编译器编译并运行 |
| 考试速查手册 | [PDF](https://github.com/Routilet/c-exam-kit/releases/download/v0.6.4/c-exam-handbook-v1.6.4.pdf) · [Markdown](https://github.com/Routilet/c-exam-kit/releases/download/v0.6.4/c-exam-handbook-v1.6.4.md) |
| 旧版函数与模板 | [全量脚手架手册](https://github.com/Routilet/c-exam-kit/releases/download/v0.6.4/scaffold-manual-v0.6.4.md) |
| 完整旧工程 | [源码 ZIP](https://github.com/Routilet/c-exam-kit/releases/download/v0.6.4/c-exam-kit-v0.6.4-source.zip) · [构建说明](https://github.com/Routilet/c-exam-kit/tree/v0.6.4#从工程重新构建) |

具体授权见 [LICENSING.md](LICENSING.md)。反馈 preset 问题时，请附 preset 版本、编译器版本和最小复现代码。
