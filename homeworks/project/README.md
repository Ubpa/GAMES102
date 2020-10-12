# 作业项目

## 环境要求

- Windows 10
- DirectX 12（用虚拟机的同学特别注意这点）
- VS 2019（[下载链接](https://visualstudio.microsoft.com/zh-hans/vs/)）
- Git（[下载链接](http://www.baidu.com/link?url=w93HUZZENOzPu-A7xAN5UzU0Rnd4gDpZDm5W6hhImQVTIgn9Zfzd33QN4vAt-sWj)）
- JDK（[下载链接](https://www.oracle.com/java/technologies/javase/javase-jdk8-downloads.html)）
- CMake 3.18.4（[下载链接](https://cmake.org/download/)，[简单使用方法的视频教程](https://www.bilibili.com/video/BV1Z7411z78n)，更详细的使用教程（TODO））
- 可选：assimp（[源码链接](https://github.com/assimp/assimp)）

## 构建方法

- 使用 CMake-Gui 构建 vs 2019 项目。configure 过程中会下载许多依赖库，耗时可能较久，由 git clone 速度而定。另外我们会提供编译好的依赖包（TODO），免去下载/编译的过程，但这样无法查阅源码。（有个关键步骤，下载 assets_v0.zip 特别耗时（几kb/s下载几MB文件），这里提供[百度盘链接（提取码：m34i）](https://pan.baidu.com/s/11kuet_KtIvQ9MpbCM-0Jqw)，大家下载后把压缩包放到文件夹 `<build>/GAMES102_HW` 内，并把其中的assets 放到 project 下）
- 打开项目，在解决方案资源栏里找到 GAMES102_HW_hw1，右键菜单中选“设为启动项目”，然后右键菜单中选“生成”。在等待较长时间的编译完成后，即可在菜单栏中点击“本地 Windows调试器”运行hw1空项目。你会看到一个引擎编辑器界面，在 Game 窗口中有 canvas，这将是作业的所需完成部分。

## 作业 1

根据作业要求，通过修改 [CanvasSystem.cpp](src/hw1/Systems/CanvasSystem.cpp) 完成作业

> 所需数据存放在 [CanvasData.h](src/hw1/Components/CanvasData.h) 中，你可以调整它
>
> > 我们使用了 [USRefl](https://github.com/Ubpa/USRefl) 来实现静态反射功能，提供了一个自动生成描述文件 [CanvasData_AutoRefl.inl](src/hw1/Components/details/CanvasData_AutoRefl.inl) 的工具。
> >
> > 当你修改 CanvasData.h 后，你需要将 CanvasData_AutoRefl.inl 删除，以便使 CMake 自动生成新的描述文件。
> >
> > 该工具存在一定的问题，如
> >
> > - 不能有类型声明，如 `class MyClass;` 
> > - 不能嵌套类定义
> > - 不支持部分 C++17 语法
> > - 不支持模板的连续 `>>` 符号，如 `std::vector<std::vector<int>>` 
> >
> > 你可以通过手写描述的方式避免这些问题。
>
> 如果你需要添加文件，你可以在 src 的合理位置放置代码文件，然后重新在 CMake-Gui 里 configure + generate，vs2019 会刷新项目从而包含新文件

目前该系统中有画线段功能（在canvas左键拖拽即可），你需要修改这部分逻辑，如通过鼠标点击获取点集，对这些点集的 x 坐标进行排序，计算相应的曲线，对曲线采样并利用 `ImDrawList::AddLine` 来绘制 polyline（更多的绘制方法看 `ImDrawList` 定义）。

作业题涉及多种曲线生成方法，因此你还应该用 `ImGui::RadioButton` 或者其他交互方式修改曲线类型。

对于不想使用鼠标交互方式作为输入的同学，我们将提供 **plot** 方式的 UI 减轻大家的负担（TODO）

你需要了解

- Utopia：引擎框架。参考资料：[Utopia](https://github.com/Ubpa/Utopia)（内含文档，但并不丰富）
- ECS：以便理解框架的逻辑执行方式，以及代码编写方式。参考资料：[UECS 文档](https://github.com/Ubpa/UECS)，[Unity ECS 文档](https://docs.unity3d.com/Packages/com.unity.entities@0.14/index.html) 
- ImGui：简单易用的 UI 库。参考资料：[imgui](https://github.com/ocornut/imgui)。（一般直接从官方提供的 [example](https://github.com/ocornut/imgui#demo) 和 [demo](https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp) 源码中学习使用方式。

## 问题

- 如果启动后，弹出窗口过段时间后程序出错，可尝试
  - 把 [Irradiance.hlsl](assets/shaders/Irradiance.hlsl) 第 99 行的 `1024u` 改小试试，如 `256u` 
  - 把 [PreFilter.hlsl](assets/shaders/PreFilter.hlsl) 第 108 行的 `256u` 改小试试，如 `64u` 
- 框架还处于快速开发中，鲁棒性有限，不良操作可能直接导致程序崩溃，有疑惑可联系助教

