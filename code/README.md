# 图像滤镜效果实现
 
吴家行 2020213991

从以下滤镜效果中自选两种实现：
- 人像美肤
- LOMO滤镜
- 晶格化效果
- 扩散高光效果

本作业由个人完成，采用c++进行滤镜效果的实现，使用CMake进行构建工程，使用Lenna图进行效果评价。

工具库：
- [opencv-4.4.0](https://opencv.org/releases/)

## 1 人像美肤

美颜主要分为两个部分：`美白和磨皮`

### 编译安装opencv

```shell
mkdir code/face_beauty_filter/3rdparty/opencv-4.4.0/build
cd code/face_beauty_filter/3rdparty/opencv-4.4.0/build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=../../../lib/opencv ..
make -j8 
make install
```

### 构建

```shell
cd code/face_beauty_filter
mkdir build
cd build
cmake ..
make
```

### 运行

```shell
./Demo
```

相关链接：
1. python实现人像美颜 https://blog.csdn.net/weixin_41010198/article/details/87858867
2. Applying Bilateral Filter http://opencvexamples.blogspot.com/2013/10/applying-bilateral-filter.html
3. Bilateral Smoothing https://www.opencv-srf.com/2018/03/bilateral-smoothing.html
4. 双边滤波（Bilateral Filter）原理解析及代码实现 https://blog.csdn.net/molihong28/article/details/89194757
5. OpenCV双边滤波详解及实代码实现 https://blog.csdn.net/qq_36359022/article/details/80198890

## 2 晶格化效果
