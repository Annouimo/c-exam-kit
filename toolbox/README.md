# C Toolbox · v0.5.0 预览版

为熟悉高级语言的使用者提供可装入 C99 答案的容器、字符串和数据处理操作。下载一个 Python 脚本，离线初始化题目目录；无需安装第三方 Python 包。

## 日常使用

运行发布脚本：

~~~sh
python c_toolbox_v0.5.0.py
~~~

只问一次：

~~~text
题目文件夹（留空导出根目录 Markdown 手册）：
~~~

输入如“练习/C5/A”，在脚本所在目录下创建多层目录，里面只有：

- main.c：已引入同目录 c_toolbox.h 的空模板。
- c_toolbox.h：该题固定使用的完整工具箱。
- prepare.py：该题固定使用的提交准备工具。

相对路径以入口脚本所在目录为基准，也支持绝对路径。已存在的目标目录不会覆盖，也不会自动做提交准备；若目录里已有 prepare.py，会显示它的运行命令。换一个目录名可创建新题目。更新入口脚本只影响新建题目，不改变旧题。

输入留空，只在入口脚本旁导出“工具箱速查_v0.5.0.md”；不在每道题里重复生成手册。手册正文约 10KB，按任务列常用接口、短例子和易错规则，不提供 PDF。

## 写题、运行、提交

编辑 main.c：

~~~c
#include "c_toolbox.h"

int main(void) {
    IntVector values = read_ints(5);
    IntVector_sort_asc(&values);
    print_ints(&values, " ", "\n");
    IntVector_free(&values);
    return 0;
}
~~~

提交前运行题目目录里的 prepare.py：

~~~sh
python prepare.py
~~~

扫描该目录顶层的 C 文件，只有一个就直接处理；多个时按编号选择。展开本地头文件，保留系统头文件引用，编译检查通过后复制完整源码到剪贴板。另存 .toolbox/submit.c，原文件不变，导出结果不参与下次扫描。脚本可从其他工作目录调用。

~~~sh
python prepare.py --run               # 编译运行，不复制
python prepare.py --no-copy           # 编译检查并保存
python prepare.py other.c             # 直接指定文件
python prepare.py --cc "编译器路径"    # 编译器未加入 PATH 时
~~~

支持 GCC/Clang，自动带 C99 与 -lm。也可手动编译：cc -std=c99 -O2 main.c -o main -lm。编译检查只验证可编译链接，不验证题意或答案。

Windows 使用 Unicode 剪贴板；macOS 使用 pbcopy；Linux 使用可用的 wl-copy/xclip/xsel。没有编译器或编译失败不复制；剪贴板不可用会明确提示，检查过的源码仍保存在 .toolbox/submit.c。剪贴板分支有自动化测试，实际桌面的可用性仍取决于系统会话和工具。

展开支持普通双引号本地 include、子目录、带保护宏的重复引用和续行，并保留 #line 来源定位。会去掉源文件注释，不展开注释或字符串里的伪 include。所有条件分支引用的本地文件都须存在；不支持宏形式 include 与本地循环引用。系统头文件不展开。一次处理一个翻译单元，不自动链接多个独立 .c 文件。

## 功能

- String 与拥有字符串的 StringList：增删、切片、分词、排序去重、格式化、对象间直接传参。
- 类型化 Vector：常用数值排序、查找、统计；同类型 map/filter/reduce、any/all、回调计数和查询、便捷构造、整数 range。
- Deque / Heap：队列、栈、双端队列和优先队列，支持用户类型。
- 字符串键 Map / Set / Counter：映射、集合运算、频次统计，可直接接收 String 并返回独立字符串列表。
- io：默认跳过空白，raw 精确保留空行和首尾空白，支持严格数值验证与批量读写。

切片目前仍为起点加数量，不支持 Python 式负下标或步长；map/filter/reduce 使用普通 C 回调，不提供 lambda。通用 Vector 浅复制元素，字符串序列默认用 StringList。整数键映射、矩阵、日期、大整数和旧板子的其他算法尚未全部迁入。

[精简速查源稿](docs/quick_reference.md) · [详细对象约定](docs/objects.md) · [输入输出规则](docs/io.md)

## 开发与高级装配

开发时逻辑、C 模板与手册内容分离；发布时组装成一个单文件脚本。

~~~sh
python toolbox/build.py
python toolbox/build.py --check
python toolbox/tests/test_toolbox.py
python toolbox/tests/test_workflow.py
~~~

| 路径 | 内容 |
| --- | --- |
| src/generator.py | 依赖解析、类型化 C 装配 |
| src/installer.py | 单次提问、题目目录与 Markdown 导出 |
| src/prepare.py | 独立提交准备工具，复制到每个题目 |
| catalog.json、templates/ | 配置与独立 C 实现 |
| docs/quick_reference.md | 人工维护的短手册，版本由构建替换 |
| build.py | 合并逻辑、模板、手册与许可 |
| tests/ | 算法、所有权、分发和提交流程测试 |
| dist/ | 生成物，不手动修改 |

旧参数接口保留，也可显式使用 generate/header。需要定制用户结构体容器时，可生成一份替代该题 c_toolbox.h 的完整头文件；不要同时引入两份不同配置的工具箱。

~~~sh
python c_toolbox_v0.5.0.py --list
python c_toolbox_v0.5.0.py header --vector StudentVector=Student --preamble types.h -o 练习/A/c_toolbox.h
python c_toolbox_v0.5.0.py generate --modules string counter -o functions.c
~~~

types.h 的类型定义会嵌入生成头文件，不必在 main.c 中再次定义。重新生成题目头文件是显式操作；日常初始化不会更新旧目录。

CI 验证 Linux GCC/Clang、macOS、Windows、Python 3.9，并执行地址/未定义行为/泄漏检查。预览附件只分发单文件 Python，不创建正式 Release。

工程与脚本 MIT；C 模板、生成源码和代码示例 MIT-0；说明和手册正文 CC BY 4.0。详见根目录 LICENSING.md。单文件发布脚本内保留适用许可和说明署名。沿用项目现有考试使用约定，是否允许离线装配工具须由课程组确认。
