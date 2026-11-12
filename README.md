# C语言考试工具包 · C Exam Kit

面向 C 语言课程考试的精简速查手册与离线代码脚手架。

设计者：**Annouimo** · 手册 **v1.4** · 脚手架 **v0.4.0**

- [考试手册](docs/C语言考试手册.md)：数据类型、标准库、输入输出注意事项和精简组件目录。
- [下载发布版](https://github.com/Annouimo/c-exam-kit/releases)：手册、单文件源码、工程包和各平台程序。
- [全量脚手架手册](docs/全量脚手架手册.md)：详细用法与可复制的代码。
- [脚手架附录](docs/脚手架附录.md)：方便手动替换进考试手册的后半部分。

**优先下载Release中的对应平台编译成品，解压后运行。若有运行或兼容问题，再下载单文件源代码自行编译，步骤见下文。**

Release中的 `c-exam-handbook` 是考试手册，`scaffold-appendix` 是可替换附录，`scaffold-manual` 是详细手册；附件名中的版本号便于区分更新。

## 脚手架怎么用

选输入输出骨架 → 选函数组件和可选main片段 → 配置类型、容量和分隔符 → 复制或保存代码，再自行完成题意。

提供43个函数组件、14种输入输出方式和8个main片段。只生成选中的组件，共用定义自动去重；可只生成函数、不生成main。默认复制纯源码，控制台随后显示本次说明，输入 `c` 看源码、`s` 另存、回车退出。

程序使用内置固定模板，运行时无需联网、Python或外部配置文件。模板不会分析题意、自动调用函数或完成整道题；部分自定义读入骨架保留待补语法，需要修改后编译。

## 直接编译单文件

macOS / Linux：

```sh
cd dist
cc -std=c99 -O2 -Wall -Wextra c_template_builder.c -o c_template_builder
./c_template_builder
```

Windows（GCC / MinGW，或双击 `dist/build_windows.bat`）：

```sh
gcc -std=c99 -O2 -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8 dist/c_template_builder.c -o c_template_builder.exe -luser32
```

`--plain` 禁用清屏和颜色，适合IDE输出窗口；`--version` 显示版本；`--manual manual.md` 导出详细手册，已有文件不覆盖。

Windows和macOS支持自动复制；Linux当前提供保存/显示源码的回退方式。包含数学函数的生成代码在GCC/Clang下通常还需要链接 `-lm`。

## 从工程重新构建

需要Python 3.9+；无第三方Python依赖。生成文件不需要C编译器，运行检查需要GCC或Clang。

```sh
python3 tools/build.py
python3 tools/build.py --check
python3 tests/test_project.py
python3 tools/build.py --package
```

最后一个命令在 `release/` 生成工程ZIP。所有路径相对工程本身，命令可从其他工作目录执行。

| 目录 | 用途 |
| --- | --- |
| `catalog/catalog.json` | 分类、顺序、名称、说明、依赖、参数及模板路径 |
| `templates/` | 可直接阅读、修改的C函数、骨架和main片段 |
| `src/` | 菜单、装配、文件和剪贴板等核心流程 |
| `tools/build.py` | 校验配置、生成单文件与脚手架相关文档、打包 |
| `tests/` | 生成组合编译、算法边界、输出格式、配置和交互检查 |
| `docs/` | 考试手册、独立附录、详细手册和维护说明 |
| `dist/` | 可独立编译的单文件源码与编译脚本 |

**只修改工程输入，再重新生成。** 不手工改 `dist/c_template_builder.c` 或两份自动生成的脚手架文档。考试手册主体独立维护，构建脚本不会覆盖它；更新附录时手动替换。详见[维护说明](docs/开发与发布.md)。

## 发布

推送到main后，GitHub Actions运行检查并构建Linux、macOS和Windows程序。全部成功后，根据配置版本创建同名tag和Release，附上手册、源码、工程包及平台程序；已有该版本Release时跳过发布。后续发布先修改版本号、更新手册中的对应版本，再提交。

macOS程序取决于构建runner的CPU架构，发布文件名会注明实际架构；Windows为x86_64。桌面剪贴板需在真实桌面环境验证，CI编译和模拟后端测试不代表桌面端到端验证。
