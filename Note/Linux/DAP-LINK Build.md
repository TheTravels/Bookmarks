# DAP-LINK Build
```
/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : DAP-LINK Build.md
* Author             : Merafour
* Last Modified Date : September 10, 2020 
* Description        : DAP-LINK 源码编译.
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
	git clone https://github.com/ARMmbed/DAPLink
##### 1.2 Python 虚拟环境安装
	sudo apt-get install virtualenv virtualenvwrapper
    or
    sudo pip3 install virtualenv virtualenvwrapper
##### 1.3 编译参考命令
```
	pip install virtualenv
	virtualenv venv
```
```
	venv/Scripts/activate.bat
	pip install -r requirements.txt
	progen generate -t uvision
	venv/Scripts/deactivate.bat
```
注：参考命令来自参考文档，根据自己的实际需要执行！
##### 1.4 编译版本 tag:v0253
	git checkout v0253
##### 1.5 工具 project_generator
	Project generators for various embedded tools (IDE). IAR, uVision, Makefile, CoIDE, Eclipse and many more in the roadmap!
这个是英文原文描述，简单说就是这个工具可以用来生成我们常用的开发环境的工程。

##### 1.6 更新虚拟环境
	pyrsistent requires Python '>=3.5' but the running Python is 2.7.16
当创建虚拟环境之后在执行 **sudo pip install -r requirements.txt** 命令时出现改错误通过更新 Python 版本解决，需删除原虚拟环境然后执行以下命令：
```
	virtualenv -p python3 venv
```

参考：[使用virtualenv创建Python虚拟环境](https://www.jianshu.com/p/94a047301f4a)

##### 1.7 删除虚拟环境
	rmvirtualenv venv

## 2.编译源码
##### 2.1 创建虚拟环境：
	sudo apt-get install virtualenv virtualenvwrapper
    virtualenv venv
当出现如下信息便是虚拟环境创建成功：
```

DAPLink $ virtualenv venv
Running virtualenv with interpreter /usr/bin/python2
New python executable in /home/pi/github/tools/arm/DAPLink/venv/bin/python2
Also creating executable in /home/pi/github/tools/arm/DAPLink/venv/bin/python
Installing setuptools, pkg_resources, pip, wheel...done.
DAPLink $
```
以上命令需在 DAPLink 源码路径执行。
##### 2.2 更新需要的工具
    venv/Scripts/activate.bat       # 激活虚拟环境
    pip install -r requirements.txt # 更新工具,改过程耗时视情况而定
    progen generate -t uvision      # 生成 MDK工程
    venv/Scripts/deactivate.bat     # 退出虚拟环境
以上是参考文档，最新源码 activate 在 **venv/bin** 目录，因此正确的命令为：
```
$ venv/bin/activate   (For Linux)
$ venv/bin/activate.bat   (For Windows)
```
然后才是执行 `pip install -r requirements.txt` 命令。而Linux环境中退出虚拟环境最新是执行 `deactivate` 命令。
根据官方描述，`progen generate -t uvision` 命令将生成所有项目，单个项目使用如下命令：
```
progen generate -f projects.yaml -p stm32f103xb_stm32f746zg_if -t uvision
-f用于输入项目文件
-p项目名称
-t指定 IDE 名称
```


## 附录：

[DAPLINK源码编译指南](https://www.eemaker.com/daplink-yuanmabianyi.html)
[DAPLink](https://github.com/ARMmbed/DAPLink) : ARMmbed,DAP-LINK官方源码
[树莓派3b安装virtualenv和virtualenvwrapper](https://www.codetd.com/article/18896)
[project_generator](https://github.com/project-generator/project_generator)
[DAPLink 开发人员指南](https://github.com/ARMmbed/DAPLink/blob/master/docs/DEVELOPERS-GUIDE.md) : 官方开发文档
[Automated Tests](https://github.com/ARMmbed/DAPLink/blob/master/docs/AUTOMATED_TESTS.md)
[使用virtualenv创建Python虚拟环境](https://www.jianshu.com/p/94a047301f4a)