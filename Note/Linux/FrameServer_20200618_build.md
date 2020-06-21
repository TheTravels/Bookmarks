# OBD4G 测试服务器 FrameServer 编译
```
/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : FrameServer_20200618_build.md
* Author             : Merafour
* Last Modified Date : June 19, 2020 
* Description        : OBD4G 测试服务器 FrameServer 编译.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
* https://github.com/TheTravels/Bookmarks/tree/master/Note/Linux
******************************************************************************/
```

## 1.基本编译环境
这里以 xubuntu 为例，在线安装软件的命令为 apt-get，管理员权限 sudo。
首先通过虚拟机 [VirtualBox](https://www.virtualbox.org/) 或其它方式安装 xubuntu 系统，我这里使用虚拟机安装，在安装增强工具的时候可能会因为系统缺省没有安装 "gcc make perl"这几个工具从而导致增强工具包安装的不全，执行命令：
```
sudo apt-get install gcc make perl
```
安装即可。这里不推荐使用 VMware，原因有两点：一，VirtualBox是开源免费的，功能足够我们使用；二，VirtualBox消耗的资源相对更少。

## 2. 基本命令

CMake安装：
sudo apt-get install cmake

解压代码：
tar xzf FrameServer_20200618.tar.gz

## 3.编译源码
这里以源码包 FrameServer_20200618.tar.gz 为例。先要安装 cmake：
sudo apt-get install cmake
然后解压缩代码，并进入代码的根目录执行 make我们会得到如下信息：
```
CMake Error at /usr/share/cmake-3.10/Modules/FindBoost.cmake:1947 (message):
  Unable to find the requested Boost libraries.

  Unable to find the Boost header files.  Please set BOOST_ROOT to the root
  directory containing Boost or BOOST_INCLUDEDIR to the directory containing
  Boost's headers.
```
这表示我们需要安装 Boost这个库，参考 [Linux下boost库的编译、安装详解](https://www.cnblogs.com/smallredness/p/9245127.html)进行安装，编译的时候我们会发现：
```
[100%] Linking CXX executable obd_server
/usr/bin/ld: cannot find -lmysqlclient
collect2: error: ld returned 1 exit status
CMakeFiles/obd_server.dir/build.make:2439: recipe for target 'obd_server' failed
```
链接失败了，很明显缺少 mysqlclient这个库，我们需要安装。执行：
```
服务端：sudo apt-get install mysql-server -y
客户端：sudo apt-get install mysql-client libmysqlclient-dev -y
```
如果数据库不在本地不需要安装服务端。之后我们make就可以发现：
```
[ 99%] Building C object CMakeFiles/obd_server.dir/source/modules/obd/version.c.o
[100%] Linking CXX executable obd_server
make[4]: Leaving directory '/home/obd/git/FrameServer/build'
[100%] Built target obd_server
```
编译成功了，在build目录下有 obd_server可执行文件，这就是服务器端程序。

安装 Server端的时候可能没有提示设置 MySQL的root密码，这个需要重新设置，参考：
《[ubuntu mysql 设置root密码](https://www.cnblogs.com/longchang/p/12614662.html)》

## 附录
[Boost](https://www.boost.org/)

[Getting started guide: ](http://www.boost.org/more/getting_started/unix-variants.html)

[Boost.Build documentation](http://www.boost.org/build/)

[Linux安装Boost库](https://www.cnblogs.com/ich1990/p/12124406.html)

[Linux下boost库的编译、安装详解](https://www.cnblogs.com/smallredness/p/9245127.html)

[Ubuntu install mysql-server](https://www.cnblogs.com/xyyhcn/p/11928603.html)

[ubuntu mysql 设置root密码](https://www.cnblogs.com/longchang/p/12614662.html)


