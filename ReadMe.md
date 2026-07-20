D2DControlUI
D2DControlUI 是一套基于 Direct2D 和 DirectWrite 的轻量级 UI 框架，采用 CSS 样式系统 和 Flexbox 布局 的思想设计，提供丰富的控件和灵活的布局容器。它不依赖任何大型 UI 框架，仅使用 Direct2D、DirectWrite、WIC（Windows Imaging Component）以及 ATL 容器（CAtlMap、CAtlArray），适合需要高性能原生渲染的 C++ 桌面应用程序。

核心特性
CSS 风格的状态样式管理
每个控件都支持 Normal、Hover、Pressed、Disabled、Focused、Checked、Dragging、DragOver 等多种状态，可以为每个状态独立设置样式（背景、边框、圆角、文本、变换等），并支持样式层叠（类似 CSS 的优先级）。

Flexbox 布局容器
提供了 CD2DHBoxContainerUI、CD2DVBoxContainerUI、CD2DGridContainerUI 以及带滚动功能的容器。支持弹性空间分配（CD2DSizePolicy：Fixed、Auto、Fill、Expand、Percent），主轴/交叉轴对齐，间距控制，以及绝对定位。

丰富的内置控件

按钮：CD2DControlUI 可直接作为按钮使用，通过样式设置不同状态的外观。

多行文本编辑：CD2DTextEdit 支持多行文本、占位符、最大字符数、自动换行、滚动、光标和选择。

文本区域：CD2DTextArea 用于只读多行文本显示，带滚动条。

列表：CD2DListUI 提供虚拟化列表，支持大量项的高性能渲染，内置垂直滚动条、选择、悬停。

下拉选择框：CD2DSelectUI 结合显示区和弹出列表，支持选项管理、最大可见项数和样式配置。

复选框：CD2DCheckboxUI 勾选框样式完全由状态驱动，支持自定义对勾颜色。

图标：CD2DIcon 可从文件或资源加载 PNG 图像，支持多种缩放模式（拉伸、等比缩放、原始大小），并自动适配高 DPI 显示，保证清晰不失真。

资源管理器视图：CD2DExploreUI 支持列表、水平、网格、瀑布流四种视图模式，支持单选/多选和拖拽排序。

全局样式管理
StyleManager 允许注册命名样式类（如 "btn"、"btn-danger"），然后通过空格分隔的类名列表为控件批量应用样式，彻底替代控件的独立样式设置，使界面风格统一且易于维护。

高性能渲染
利用 Direct2D 的硬件加速、图层裁剪、几何体复用等技术；列表控件采用虚拟化（仅创建可见项），拖拽时只绘制半透明副本，滚动条在需要时自动显示。

完整的交互能力
支持鼠标悬停、点击、双击、拖拽排序、键盘导航（文字编辑），以及焦点管理（CD2DContainerUI 提供统一的焦点子控件管理）。

高 DPI 完美适配
所有尺寸基于 DIP（设备无关像素），文本、位图均根据系统 DPI 缩放，保证在高分辨率显示器上清晰显示。

模块化与可扩展
所有控件继承自 CD2DControlUI（控件基类），再向上继承自 CD2DControlBaseUI（状态与事件基类）。开发者可以方便地继承并扩展自己的控件。

架构概览
text
CD2DControlBaseUI        ← 状态（Normal/Hover/...）、尺寸策略、事件虚函数、基础位置/大小
  └─ CD2DControlUI       ← 样式层叠（SetStyle/GetStyle）、文本绘制、背景/边框绘制、控件自身绘制逻辑
       ├─ CD2DContainerUI ← 子控件管理、事件分发、Z-Order 排序、焦点/捕获/悬停管理
       │    ├─ CD2DHBoxContainerUI（水平弹性布局）
       │    ├─ CD2DVBoxContainerUI（垂直弹性布局）
       │    ├─ CD2DGridContainerUI（网格布局）
       │    └─ CD2DScrollContainerUI（带滚动条的容器）
       ├─ CD2DTextEdit, CD2DTextArea, CD2DCheckbox, CD2DSelectUI, CD2DListUI, CD2DIcon, CD2DExploreUI …
       └─ CD2DScrollUI（滚动条控件）
样式系统
每个控件内部维护一个 CAtlMap<ED2DStatus, CD2DStyle> 映射，通过 SetStyle 设置不同状态的样式，CalculateEffectiveStyle 按优先级（Disabled > Pressed > Hover > Focused > Normal 等）合并出最终样式。

布局流程
容器的 Layout 方法遍历子控件，根据每个子控件的 CD2DSizePolicy 决定其尺寸（Auto 调用 SizeToContent，Fixed 用指定值，Fill/Expand 按权重分配剩余空间），再根据 CD2DAlignment 控制交叉轴位置，最后通过 SetPosition 和 SetSize 设置子控件的位置和大小。


应用开发
小河便签，小河AI导航，赛博佛堂。


特别感谢: Deepseek 和 copilot。

Vibe Coding 让windows 小软件变得简单而漂亮

[小河便签](Png/XiaoHeBianQian.png)