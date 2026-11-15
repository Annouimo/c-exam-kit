# C语言考试工具包 · C Exam Kit

面向 C 语言课程考试的精简速查手册与离线代码脚手架。

设计者：**Annouimo** · 手册 **v1.6.2** · 脚手架 **v0.6.2**

## 从这里开始

| 你想做什么 | 下载或操作 |
| --- | --- |
| 阅读考试速查资料 | [下载 PDF 手册](https://github.com/Annouimo/c-exam-kit/releases/latest/download/c-exam-handbook-v1.6.2.pdf) |
| 使用离线脚手架 | [下载单文件 C 源码](https://github.com/Annouimo/c-exam-kit/releases/latest/download/c_scaffold_v0.6.2.c)，**先编译运行，再按菜单选择** |
| 像传统板子一样搜索、复制函数 | [下载全量脚手架手册](https://github.com/Annouimo/c-exam-kit/releases/latest/download/scaffold-manual-v0.6.2.md) |

- [考试手册模板](docs/templates/exam_handbook.md)：按做题主题组织，说明文字可直接修改。
- [下载发布版](https://github.com/Annouimo/c-exam-kit/releases)：PDF与Markdown手册、带版本号的单文件源码和工程包。
- [手册维护说明](docs/开发与发布.md)：模板、表格数据、组件目录和PDF配置。

**阅读资料优先下载PDF；使用生成器优先下载 `c_scaffold_v0.6.2.c`，编译运行后按菜单选择。新版Release不再分发编译好的二进制。**

这个 `.c` 是交互式脚手架生成程序，不是传统的代码片段集合。需要直接搜索、复制函数时，请看《全量脚手架手册》。文件开头也有英文编译运行提示，旧IDE能直接阅读。

Release中的 `c-exam-handbook` 是考试手册（PDF和Markdown），`scaffold-appendix` 是可替换附录，`scaffold-manual` 是详细手册；`c_scaffold_v版本号.c` 是推荐下载的生成器源码。

## 脚手架怎么用

选程序流程及必要读法 → 选函数组件 → 按需配置、确认 → 复制或保存代码，再自行完成题意。处理/输出代码在确认页输入 `o` 可选补充，不再单独询问main内片段。

提供45个函数组件、11种程序流程（包含17种具体骨架）和8份可选代码模板。只生成选中的组件，共用定义自动去重；可只生成函数、不生成main。默认复制带精简注释的源码（包含署名），控制台随后显示本次说明，输入 `c` 看源码、`s` 另存、回车退出。确认页回车复制精简注释代码，1复制无注释代码，2复制完整注释代码；s保存时再直接选择注释模式，r返回修改；所有交付渠道使用同一份代码，头文件按依赖合并。

程序使用内置固定模板，运行时无需联网、Python或外部配置文件。装配在内存中完成，不依赖临时目录的写入权限。模板不会分析题意、自动调用函数或完成整道题；部分自定义读入骨架保留待补语法，需要修改后编译。

例如，只想向已有程序加入最大公约数函数：选择“仅组件函数” → 选择“最大公约数与最小公倍数” → 回车复制 → 粘贴到编辑器。组件会带入依赖；你仍需在自己的程序中写调用，例如 `printf("%lld\n", gcd(12, 18));`，输出为 `6`。

**考试使用前请向课程组确认。** 本工具是离线固定模板装配程序，不分析题意，也不在运行时调用 AI。允许携带本地资料或代码，不必然等于允许运行装配工具；请事先核实考试规则，并理解、检查自己使用的代码。项目不代表课程组，不保证考试许可或题目通过。

## 直接编译单文件

macOS / Linux：

```sh
cd dist
cc -std=c99 -O2 -Wall -Wextra c_scaffold_v0.6.2.c -o c_scaffold
./c_scaffold
```

Windows（GCC / MinGW，或双击 `dist/build_windows.bat`）：

```sh
gcc -std=c99 -O2 -Wall -Wextra dist/c_scaffold_v0.6.2.c -o c_scaffold.exe -luser32
```

`--plain` 禁用清屏和颜色，适合IDE输出窗口；`--stdio-encoding=gbk` 适配按GBK解释输入输出的旧Windows IDE；`--version` 显示版本；`--manual manual.md` 导出详细手册，已有文件不覆盖。

单文件源码仅含ASCII字符，中文以UTF-8字节转义保存，不依赖IDE的源码编码设置。Windows真正的控制台使用ReadConsoleW/WriteConsoleW，不修改系统代码页；重定向默认UTF-8，只有显式选择GBK时转换界面输入输出。该选项不改变保存文件和剪贴板的编码。

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
| `docs/templates/` | 人工维护的说明段落、示例及自动内容占位符 |
| `catalog/handbook/` | 标准库表格、主题归属、版本元数据和PDF样式 |
| `docs/` | 维护说明与文档模板 |
| `dist/` | 可独立编译的单文件源码与编译脚本 |

**只修改模板、数据和工程输入，再重新生成。** 仓库不保留展开后的全量手册；Markdown、PDF和详细手册输出到 `release/`，作为Release附件分发。说明段落在模板中编辑，重复表格和目录由配置生成；构建不修改模板。只预览文档可执行 `python3 tools/build.py --docs-only`。详见[维护说明](docs/开发与发布.md)。

## PDF手册

普通源码构建不需要PDF依赖。PDF构建在Linux / GitHub Actions中运行，需额外安装Pandoc、Noto CJK / DejaVu字体及PDF构建依赖：

```sh
python3 -m pip install -r tools/pdf-requirements.txt
python3 tools/build_pdf.py
```

输出为 `release/c-exam-handbook-v1.6.2.pdf`，支持中文字体、可点击目录/链接、可搜索文本、C代码高亮（含表格签名）、代码换行和跨页表头。PDF与Markdown共用同一份模板和数据；字体、页边距、表格列宽及页眉页脚在 `catalog/handbook/pdf.json` 中配置。执行时加 `--preview` 可标记为未发布预览。

## 发布

功能分支只生成检查结果和文档预览，不发布Release。推送main后，GitHub Actions验证Linux、macOS、Windows以及GCC 9编译兼容性，并生成PDF。检查全部成功后创建Release，仅附带版本号的单文件 `.c`、源码工程包、PDF和Markdown文档，不上传可执行程序。已有该版本Release时跳过发布，后续发布需提升版本号。

CI中的编译成品仅用于测试。旧版本Release作为历史记录保留；v0.5.0起不再分发二进制。真实Dev-C++环境仍建议实际试运行；现代MinGW上的Unicode控制台测试与旧IDE窗口的GBK兼容测试分别进行。

## 授权与反馈

工程代码采用 [MIT](LICENSE)，可复用模板、生成代码及手册代码示例采用 [MIT-0](LICENSES/MIT-0.txt)，手册与说明文档采用 [CC BY 4.0](LICENSES/CC-BY-4.0.txt)。**生成代码可以删除工具署名和全部注释；转载手册正文请署名并注明来源及修改情况。** 具体范围见[授权说明](LICENSING.md)。

遇到问题请[提交 Issue](https://github.com/Annouimo/c-exam-kit/issues/new/choose)，附版本、系统、IDE/编译器、操作步骤和完整报错；手册问题注明页码或段落。旧 Windows IDE 中文乱码可先尝试 `--plain --stdio-encoding=gbk`，或在独立控制台中运行。欢迎反馈错漏和提交改进，贡献内容请遵循对应授权并注明第三方来源。
