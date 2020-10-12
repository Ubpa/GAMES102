# CMake

CMake 适用于自动构建项目的工具

我们掌握如何使用即可，不要求能编写

## 构建项目方法

设 `<project>` 为包含 CMakeLists.txt 的路径，也是项目的根目录

在 cmake-gui 里，`Where is the source code` 内填上 `<project>`，在 `Where to build the binaries` 中填上 `<project>/build` 

接着点击 configure，此时会弹出一个窗口，用于选择 IDE（默认 vs2019），位数（win32/x64）等，保持默认即可

> 如果出错，可以查看下框中的信息，寻找错误信息，并依此寻找解决办法

如果没有错误，中框会有一些项目的配置选项，一般保持默认，也可根据文档或者变量名对应的含义，并根据需求去修改它们

接着点击 generate

> 此时也有一定可能出错

如果没有错误，点击 open project 即可打开项目

在 vs 2019 的解决方案资源管理器中，可以看到有许多项目，其中有 CMake 自动生成的项目

- ALL_BUILD：右键生成可以生成其他所有项目
- INSTALL：右键生成可以生成（编译）其他所有项目，并将 lib dll exe include 等文件安装到 `CMAKE_INSTALL_PREFIX`（默认在 C 盘，可能会因为没有权限而安装失败，可以在 cmake-gui 中框中找到该变量，并修改它到其他盘） 中。安装是为了供给其他 CMake 项目的 `find_package` 
- ZERO_CHECK：没什么用

另外还有用户自己的项目，包含 static library, shared library, exe 等，都可以右键生成，如果是exe项目，还可以右键“设为启动项目”，然后点击上方的“本地 Windows 调试器"来运行（并调试）该 exe

## 其他操作

- 刷新 cache：cmake-gui 菜单栏 file 处可以 delete cache，接着重新 configure 就起到刷新 cache 的作用
- 完全刷新：把 build 删了
- 添加文件：本项目提供的 CMake 能自动添加新文件，我们将文件放到比如 project/src/hw1 内，接着重新 configure + generate，回到 vs2019 后会提示重新加载，点击后就可以看到新文件加进来了。删除文件也是类似的操作
- 查看错误：configure 和 generate 都可能发生错误，有错误时，在下窗口中可以看到错误信息（有时内容很长，往上翻才能看到具体错误，一般是红色字体）
- 项目选项：cmake-gui 中框有许多可配置的选项，默认没有分组，可以勾选右上角的  "Group"，让那些选项分组。比较关键的选项有
  - CMAKE_INSTALL_PREFIEX（上文有提到）
  - 项目作者提到的选项
  - bool 变量（如本作业框架里的 `Ubpa_BuildTest_*`，默认勾选，即构建测试项目，取消勾选可以简化生成的项目）

