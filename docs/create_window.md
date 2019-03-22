## 接口
GLFWwindow* create(int vWidth, int vHeight, int vPosX = 0, int vPosY = 0, const std::string& vTitle = "", bool vFullScreen = false, bool vResizable = false);

GLFWwindow* create(const SWindowCreateInfo& vCreateInfo);

<br><br>

## 代码示例
```cpp
CWindowCreator WindowCreator;
GLFWwindow* pGLFWWindow = WindowCreator.create(1600, 900, 100, 100, "Create Window", false, true);
```

或者

```cpp
CWindowCreator WindowCreator;

SWindowCreateInfo CreateInfo;
CreateInfo.WindowWidth = 1600;             //默认为0
CreateInfo.WindowHeight = 900;             //默认为0
CreateInfo.WindowPosX = 100;               //默认为0
CreateInfo.WindowPosY = 100;               //默认为0
CreateInfo.WindowTitle = "Create Window";  //默认为""
CreateInfo.IsWindowFullScreen = false;     //默认为false
CreateInfo.IsWindowResizable = true;       //默认为false

GLFWwindow* pGLFWWindow = WindowCreator.create(CreateInfo);
```

<br><br>

## 注意事项
* 当FullScreen为true时，窗口大小即为屏幕大小，此时WindowWidth与WindowHeight参数无效。