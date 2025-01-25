# cef_131_mytest

[TOC]

使用 chromium/v131

## 包管理器 conan

包管理器：conan/2.11.0

```bash
# 项目根目录下
conan install . -s build_type=Debug
```

Release 模式下

```bash
conan install . -s build_type=Release
```

此时会在 `thirdparty/conan` 目录下生成 `conandeps.props` 等 VS 属性表，然后我们在 VS 的 `视图->其他窗口->属性管理器` 引入生成的 conan 属性表

![](https://raw.githubusercontent.com/SHERlocked93/pic/master/picgo/202501241837653.png)

在项目属性上右键打开<添加现有属性表>，然后选中刚刚 `thirdparty/conan` 目录的属性配置 `conandeps.props`，添加完后，如果打开 `Debug|x64` 下的 `conan_nlohmann_json_debug_x64`，就会发现 conan 已经帮我们把附加包含目录和附加库目录已经加好了，指向 conan 的缓存目录，之后我们就可以在项目里引入 conan 配置安装的第三方库的头文件和相关动态库了。

![](https://raw.githubusercontent.com/SHERlocked93/pic/master/picgo/202501241409656.png)

引入之后重启 VS，就可以从 conan 的缓存目录里直接 `#include` 相应库的头文件了。 

### 第三方包

1. [nlohmann::json](https://json.nlohmann.me/)/3.11.3
