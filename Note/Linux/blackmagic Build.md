# blackmagic Build
```
/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : blackmagic Build.md
* Author             : Merafour
* Last Modified Date : September 10, 2020
* Description        : blackmagic 源码编译.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
* https://github.com/TheTravels/Bookmarks/tree/master/Note/Linux
******************************************************************************/
```

## 1.编译环境
这里使用的硬件平台为 raspberry4B，在线安装软件的命令为 apt-get,pip,pip3，管理员权限 sudo。
##### 1.1 下载源码
	git clone https://github.com/blacksphere/blackmagic.git
##### 1.2 编译命令
```
$ git clone https://github.com/blacksphere/blackmagic
$ cd blackmagic
$ git submodule init
$ git submodule update
$ make
$ cd src
$ make clean
$ make PROBE_HOST=native
$ cd ../scripts
```




## 附录：

[blackmagic源码](https://github.com/blacksphere/blackmagic)
[WiKi](https://github.com/blacksphere/blackmagic/wiki)
[Debugger Hardware](https://github.com/blacksphere/blackmagic/wiki/Debugger-Hardware)
[Debug Probes](http://wiki.paparazziuav.org/wiki/Debug_Probes) : 参考编译文档
[STLink](http://wiki.paparazziuav.org/wiki/STLink#Update_the_ST-Link_to_blackmagic_probe) : STLink固件

