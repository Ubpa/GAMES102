# 作业项目

项目代码变更记录：[changelog.md](changelog.md) 

## 环境要求

- Windows 10

- DirectX 12

- VS 2019 16.7.5+（[下载链接](https://visualstudio.microsoft.com/zh-hans/vs/)）

  > 注意版本，有测试过 16.4.5 版本就不行

- Git（[下载链接](http://www.baidu.com/link?url=w93HUZZENOzPu-A7xAN5UzU0Rnd4gDpZDm5W6hhImQVTIgn9Zfzd33QN4vAt-sWj)）

  > 在 CMake 的过程中需要调用 Git，注意把 git 目录下的文件夹 bin 路径添加到系统环境变量 Path 中，参考[教程](https://www.cnblogs.com/-mrl/p/11246666.html) 

- Github Desktop（[下载链接](https://desktop.github.com/)）

  > 希望同学尽量用 GitHub desktop 来克隆库，由于项目代码经常更新，用 GitHub desktop 方便拉取更新。更新可能导致冲突问题，详细解决方法请自行查询相关资料。

- CMake 3.18.4+（[下载链接](https://cmake.org/download/)，[简单使用方法的视频教程](https://www.bilibili.com/video/BV1Z7411z78n)，[更详细的使用教程](../../softwares/CMake.md)）

- 可选：assimp（[源码链接](https://github.com/assimp/assimp)）

## 常见问题

- 编译时缺失 `atl*` 的话，可以在 vs2019 里选 工具->获取工具和功能->单个组件->搜索 "x64 14.27" 选生成工具，再搜 "atl"，选最下边的“适用于最新 v142 生成工具的 C++ ATL(x86和x64)"
- 如果启动后，弹出窗口过段时间后程序出错，可尝试（依显卡性能而定）
  - 将独显设为默认显卡
  - 把 [Irradiance.hlsl](assets/shaders/Irradiance.hlsl) 第 99 行的 `1024u` 改小试试，如 `256u`，甚至 `1u`，`0u`
  - 把 [PreFilter.hlsl](assets/shaders/PreFilter.hlsl) 第 108 行的 `256u` 改小试试，如 `64u`，甚至 `1u`，`0u` 
- 框架还处于快速开发中，鲁棒性有限，不良操作可能直接导致程序崩溃，有疑惑可联系助教

## 构建方法

### 方法一

此方法完全从源码构建，如果不行请看[方法二](#方法二) 

- 使用 Github Desktop 克隆此库 [GAMES 102](https://github.com/Ubpa/GAMES102)，注意路径应放到**较浅**的位置，如盘符根目录

- 使用 CMake-Gui 构建 vs 2019 项目

  - 注意 where is the source code 是<project 路径>，下边的路径是 <project 路径>/build
  - 首次点击 configure 默认就是 vs2019 和 x64，这两者都是必要的

  > configure 过程中会下载许多依赖库，耗时可能较久，由 git clone 速度而定。另外我们会提供编译好的依赖包（即方法二），免去下载/编译的过程，但这样无法查阅源码。
  >
  
- 打开项目，在解决方案资源栏里找到 GAMES102_HW_hw1，右键菜单中选“设为启动项目”，然后右键菜单中选“生成”。在等待较长时间的编译完成后，即可在菜单栏中点击“本地 Windows调试器”运行 hw1 空项目。你会看到一个引擎编辑器界面，在 Game 窗口中有 canvas，这将是作业1的所需完成部分。

### 方法 2

此方法使用方便，缺点是不含源码

> 所需下载的资源在课程群文件内的“框架依赖资源”里可以找到

- 使用 Github Desktop 克隆此库 [GAMES 102](https://github.com/Ubpa/GAMES102)，注意路径应放到**较浅**的位置，如盘符根目录
- 下载编译好的依赖库 [Ubpa_v2.zip](https://pan.baidu.com/s/1byH_y3rXcrrs-H7UN9mueA)（百度网盘提取码：qmp4）放到合适的位置，然后将其内的文件夹 bin 的路径加到环境变量 Path 中（参考[教程](https://www.cnblogs.com/-mrl/p/11246666.html)）
- 使用 CMake-Gui 构建 vs 2019 项目（此时会非常顺利，最终构建的项目内**仅含作业项目**，日后有需要在进行源码构建）
  - 注意 where is the source code 是<project 路径>，下边的路径是 <project 路径>/build
  - 首次点击 configure 默认就是 vs2019 和 x64，这两者都是必要的

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
> > - 不支持部分 C++17 语法
> > - 不支持模板的连续 `>>` 符号，如 `std::vector<std::vector<int>>`，可改为 `std::vector<std::vector<int>> >` 
> >
> > 你可以通过手写描述的方式避免这些问题。
>
> 如果你需要添加文件，你可以在 src 的合理位置放置代码文件，然后重新在 CMake-GUI 里 configure + generate，vs2019 会刷新项目从而包含新文件

目前该系统中有画线段功能（在canvas左键拖拽即可），你需要修改这部分逻辑，如通过鼠标点击获取点集，计算相应的曲线，对曲线采样并利用 `ImDrawList::AddLine` 来绘制 polyline（更多的绘制方法看 `ImDrawList` 定义）。

作业题涉及多种曲线生成方法，因此你还应该用 `ImGui::RadioButton` 或者其他交互方式修改曲线类型。

你可能需要用到矩阵计算，推荐使用纯头文件的 Eigen 库（[官网](http://eigen.tuxfamily.org/index.php?title=Main_Page)），添加进项目里的方式有两个

- 简单：将其源码（Eigen 文件夹）放到 `src/hw1`，接着删掉其中的 `CMakeLists.txt`，接着 cmake configure + generate 后即可将库添加到项目中，后续用 `#include "<relative-path>/Eigen/<component>"` 来包含库。
- :star:科学：将源码（Eigen 文件夹）放到文件夹 `include/_deps/` 内（自行创建），接着在 [src/hw1/CMakeLists.txt](src/hw1/CMakeLists.txt) 的 Add_Target 内添加一行 `INC "${PROJECT_SOURCE_DIR}/include/_deps"`，后续用 `#include <Eigen/<component>>"` 来包含库。

对于不想使用鼠标交互方式作为输入的同学，我们将提供 **plot** 方式的 UI 减轻大家的负担（TODO）

你需要了解

- Utopia：引擎框架。参考资料：[Utopia](https://github.com/Ubpa/Utopia)（内含文档，但并不丰富）
- ECS：以便理解框架的逻辑执行方式，以及代码编写方式。参考资料：[UECS 文档](https://github.com/Ubpa/UECS)，[Unity ECS 文档](https://docs.unity3d.com/Packages/com.unity.entities@0.14/index.html) 
- ImGui：简单易用的 UI 库。参考资料：[imgui](https://github.com/ocornut/imgui)。（一般直接从官方提供的 [example](https://github.com/ocornut/imgui#demo) 和 [demo](https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp) 源码中学习使用方式）

## 作业 3

根据作业要求，通过修改 [CanvasSystem.cpp](src/hw3/Systems/CanvasSystem.cpp) 完成作业

> 所需数据存放在 [CanvasData.h](src/hw3/Components/CanvasData.h) 中，你可以调整它

作业 3 和作业 1 是类似的

## 作业 4

根据作业要求，通过修改 [CanvasSystem.cpp](src/hw4/Systems/CanvasSystem.cpp) 完成作业

> 所需数据存放在 [CanvasData.h](src/hw4/Components/CanvasData.h) 中，你可以调整它

## 作业 5

根据作业要求，通过修改 [CanvasSystem.cpp](src/hw5/Systems/CanvasSystem.cpp) 完成作业

> 所需数据存放在 [CanvasData.h](src/hw5/Components/CanvasData.h) 中，你可以调整它

## 作业 6

根据作业要求，通过修改 [DenoiseSystem.cpp](src/hw6/Systems/DenoiseSystem.cpp) 完成作业

> 所需数据存放在 [DenoiseData.h](src/hw6/Components/DenoiseData.h) 中，你可以调整它

本次主要用到了 [UHEMesh 库](https://github.com/Ubpa/UHEMesh)，课程视频中有其简单介绍，详细请查看源码中各类型（TVertex, TEdge, TPolygon, THalfEdge, HEMesh）接口

### 网格

点击下边的下载链接，把它们放到 [assets/models/](assets/models/) 下，重启框架即可找到新的 obj 

- [Balls.obj](https://cdn.jsdelivr.net/gh/Ubpa/USTC_CG_Data@master/Homeworks/04_MinSurfMeshPara/mesh/Balls.obj) 
- [Bunny_head.obj](https://cdn.jsdelivr.net/gh/Ubpa/USTC_CG_Data@master/Homeworks/04_MinSurfMeshPara/mesh/Bunny_head.obj) 
- [Cat_head.obj](https://cdn.jsdelivr.net/gh/Ubpa/USTC_CG_Data@master/Homeworks/04_MinSurfMeshPara/mesh/Cat_head.obj) 
- [David328.obj](https://cdn.jsdelivr.net/gh/Ubpa/USTC_CG_Data@master/Homeworks/04_MinSurfMeshPara/mesh/David328.obj) 
- [Nefertiti_face.obj](https://cdn.jsdelivr.net/gh/Ubpa/USTC_CG_Data@master/Homeworks/04_MinSurfMeshPara/mesh/Nefertiti_face.obj) 

## 作业 7

根据作业要求，通过修改 [DenoiseSystem.cpp](src/hw6/Systems/DenoiseSystem.cpp) 完成作业

> 所需数据存放在 [DenoiseData.h](src/hw6/Components/DenoiseData.h) 中，你可以调整它

框架涉及内容同于[作业 6](#作业 6)

