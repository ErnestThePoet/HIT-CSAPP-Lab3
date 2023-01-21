# 代码说明
- `bitmap.h bitmap.cpp` 位图类的定义，代表一个`bmp`格式的位图；
- `bmp_helper.h bmp_helper.cpp` 位图工具类，提供读取位图文件、打印位图信息和保存位图文件的功能；
- `blur_impls.h blur_impls.cpp` 提供图像平滑的各种实现，不包含OpenCL实现；
- `blur_opencl.h blur_opencl.cpp` 提供图像平滑的OpenCL实现；
- `cl_device_helper.h cl_device_helper.cpp` OpenCL工作流程工具类（详见本人[仓库](https://github.com/ErnestThePoet/OpenCL-Device-Helper)）；
- `code_timer.h code_timer.cpp` 代码计时器工具类；
- `macros.h` 编译选项控制宏头文件，可控制运行哪些优化实现、是否是华为泰山服务器平台（代表ARM处理器，Linux系统，不编译OpenCL）、是否编译OpenCL优化实现等；
- `utilities.h` 工具宏头文件，定义了设置打印颜色等工具宏。  

`lab3`目录下的源文件用于在泰山服务器上使用CMake和gcc编译，其中的`macros.h`已设置好，且不包含OpenCL实现。  

如果您没有OpenCL环境，请在项目设置->Linker->Input中移除附加依赖项`opencl.lib`，在`blur_opencl.h blur_opencl.cpp cl_device_helper.h cl_device_helper.cpp`这四个文件上右键选择"Exclude from project"，然后在`macros.h`中注释掉`OPENCL`这个宏，此后清理解决方案并重新编译即可。  

如果您对OpenCL实现版本有兴趣，请您自行完成Visual Studio中的环境设置，编译好kernel代码，并修改`ImageBlur.cpp`中的kernel程序文件路径宏。为了简便起见，编译kernel代码可以使用本人封装的[此编译工具](https://github.com/ErnestThePoet/CL-Compiler)。