## 简介
VKToolkit是基于[Vulkan-Hpp API](https://github.com/KhronosGroup/Vulkan-Hpp)的轻量级C++工具库，旨在帮助Vulkan开发者节约开发时间与学习成本，快速开发Vulkan应用程序。


## 开发环境
* 编译环境： Visual Studio 2017
* 依赖库：
    * HiveCommon
    * Vulkan
    * GLFW
* 特殊要求：
    * 本项目使用了新版本的HiveCommon( Hive_Binary_19.0.1及以上版本)


## 使用教程
* [创建窗口](docs/create_window.md)
* [创建Vulkan实例](docs/create_vk_instance.md)
* [启用Vulkan验证层](docs/debug_messenger.md)
* TODO


## 注意事项
* 所有Creator类只负责对应对象的创建，不负责销毁