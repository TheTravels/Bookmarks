# vsftp 服务器安装
```
/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : vsftp install.md
* Author             : Merafour
* Last Modified Date : June 19, 2020 
* Description        : vsftp 服务器安装.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
* https://github.com/TheTravels/Bookmarks/tree/master/Note/Linux
******************************************************************************/
```

## 1.基础命令
这里以Centos为例，在线安装软件的命令为 yum，管理员权限 sudo。
##### 1.1安装
	sudo yum install -y vsftpd
##### 1.2设置开机启动
	sudo systemctl enable vsftpd.service
##### 1.3启动
	sudo systemctl start vsftpd.service
##### 1.4重启
	sudo systemctl restart vsftpd.service
##### 1.5停止
	sudo systemctl stop vsftpd.service
##### 1.6查看状态
	sudo systemctl status vsftpd.service
##### 1.7查看端口
	sudo firewall-cmd --list-ports
##### 1.8卸载
	sudo yum erase vsftpd -y
    sudo rm -rf /etc/vsftpd

参考：[CentOS7 安装vsftpd 服务器](https://blog.csdn.net/uq_jin/article/details/51684722)

## 2.FTP初体验-本地匿名登录
##### 2.1 安装 vsftp： 
	sudo yum install -y vsftpd
##### 2.2 本地测试
	[admin@centos7_test_server ~]$ ftp localhost
	Trying 127.0.0.1...
	ftp: connect to address 127.0.0.1拒绝连接
	Trying ::1...
	ftp: connect: 没有到主机的路由
	ftp>
在测试之前我们需要先启动服务器：
```
	[admin@centos7_test_server ~]$ sudo systemctl start vsftpd.service
	[admin@centos7_test_server ~]$ ftp localhost
	Trying 127.0.0.1...
	Connected to localhost (127.0.0.1).
	220 (vsFTPd 3.0.2)
	Name (localhost:admin): anonymous
	331 Please specify the password.
	Password:
	230 Login successful.
	Remote system type is UNIX.
	Using binary mode to transfer files.
	ftp> ls
	227 Entering Passive Mode (127,0,0,1,107,128).
	150 Here comes the directory listing.
	drwxr-xr-x    2 0        0            4096 Apr 01 04:55 pub
	226 Directory send OK.
	ftp>
```
这表示我们在本地通过匿名(anonymous)的方式登录成功了。
但是，当我们通过外部主机访问：
```
obd@obd-VirtualBox:~$ ftp 39.108.51.xx
Connected to 39.108.51.xx.
220 (vsFTPd 3.0.2)
Name (39.108.51.xx:obd): anonymous
331 Please specify the password.
Password:
230 Login successful.
Remote system type is UNIX.
Using binary mode to transfer files.
ftp> ls
200 PORT command successful. Consider using PASV.
```
我们会发现登录成功但是访问失败了，这个是正常的，因为我们需要对 FTP服务器进行配置。
这里简单说下 FTP的两种工作模式，即 port模式和PASV模式，也叫主动模式和被动模式。我们都知道 FTP使用的是21端口，但其实 FTP还使用了一个 20端口，在 FTP的工作模式中 21端口是用来传控制指令的， 20端口是用来传数据的，两个端口分工不同，但是呢在主动模式服务器主动连接客户端来建立数据传输通道，这就带来了一个问题，即服务器一定是可以被连接的，但是客户端由于处于内网服务器是连接不进来的，因此数据通道就无法建立，从而无法正常传输数据，后来人们开发了另外一种模式，即被动模式，不同在于被动模式数据通道的建立也是客户端发起的，而被动模式需要开放一组连续的断开供外部访问，这里会访问失败也正是因为没有把服务器配置为被动模式。

参考：[FTP协议主动（Port）模式和被动（Passive）两种模式详解](https://www.cnblogs.com/linn/p/4169986.html)

当然，我的服务器是处于云服务器的内网，因此需要在云服务器后台开放这组端口，而我的已经开放了。当然，服务器本机开放对应的断口也是必须的。

## 3.FTP配置-外部主机访问
所以要让外部主机访问现在需要把服务器配置成被动模式：
```
sudo vim /etc/vsftpd/vsftpd.conf
```
加入文件末尾：
```
pasv_promiscuous=YES
pasv_address=39.108.51.xx
pasv_enable=YES
pasv_min_port=15000
pasv_max_port=15500
"/etc/vsftpd/vsftpd.conf" 133L, 5219C    
```
更改配置之后一定要重启vsftp：
```
[admin@centos7_test_server ~]$ sudo systemctl stop vsftpd.service
[admin@centos7_test_server ~]$ sudo systemctl start vsftpd.service
[admin@centos7_test_server ~]$ ftp 39.108.51.xx
Connected to 39.108.51.xx (39.108.51.xx).
220 (vsFTPd 3.0.2)
Name (39.108.51.xx:admin): anonymous
331 Please specify the password.
Password:
230 Login successful.
Remote system type is UNIX.
Using binary mode to transfer files.
ftp> ls
227 Entering Passive Mode (0,0,0,0,59,198).
150 Here comes the directory listing.
drwxr-xr-x    2 0        0            4096 Apr 01 04:55 pub
226 Directory send OK.
ftp>
```
这里我没有使用外部服务器，而是直接使用公网 IP进行测试，
但是我们用外部主机会发现：
```
obd@obd-VirtualBox:~$ ftp 39.108.51.xx
Connected to 39.108.51.xx.
220 (vsFTPd 3.0.2)
Name (39.108.51.xx:obd): anonymous
331 Please specify the password.
Password:
230 Login successful.
Remote system type is UNIX.
Using binary mode to transfer files.
ftp> ls
200 PORT command successful. Consider using PASV.
425 Failed to establish connection.
ftp> 
```
但是改用 WinSCP进行 FTP连接能够正常看懂 pub目录(被动模式访问)，说明在obd-VirtualBox这台主机上 ftp客户端并没有以被动模式访问服务器。
至此，外部匿名访问配置完成。

## 4.系统用户访问
系统用户访问首先要设置，确认一下配置：
```
local_enable=YES
userlist_enable=YES
userlist_deny=NO
```
这里把 /etc/vsftpd/user_list配置成了一个白名单，下面我们要在白名单中添加我们需要的系统用户：
[admin@centos7_test_server ~]$ sudo vim /etc/vsftpd/user_list
```
[admin@centos7_test_server ~]$ sudo cat /etc/vsftpd/user_list
# vsftpd userlist
# If userlist_deny=NO, only allow users in this file
# If userlist_deny=YES (default), never allow users in this file, and
# do not even prompt for a password.
# Note that the default vsftpd pam config also checks /etc/vsftpd/ftpusers
# for users that are denied.
root
bin
daemon
adm
lp
sync
shutdown
halt
mail
news
uucp
operator
games
nobody
obd4g
[admin@centos7_test_server ~]$
```
obd4g便是我们 FTP登录的系统用户，需要创建(sudo useradd obd4g)。然后重启服务器登录测试：
```
[admin@centos7_test_server ~]$ sudo systemctl stop vsftpd.service
[admin@centos7_test_server ~]$ sudo systemctl start vsftpd.service
[admin@centos7_test_server ~]$ ftp 39.108.51.xx
Connected to 39.108.51.xx (39.108.51.xx).
220 (vsFTPd 3.0.2)
Name (39.108.51.xx:admin): obd4g
331 Please specify the password.
Password:
230 Login successful.
Remote system type is UNIX.
Using binary mode to transfer files.
ftp> ls
227 Entering Passive Mode (0,0,0,0,58,152).
150 Here comes the directory listing.
drwxr-xr-x    2 1016     1016         4096 Jun 18 03:26 ZKHY
226 Directory send OK.
ftp>
```
可以看到我们使用系统用户 obd4g 已经乘车访问了。

参考：[Centos7 安装配置FTP服务器](https://www.jianshu.com/p/4409cb0d4f73)

## 5.限制用户不能离开主目录
出于安全考虑，不能让用户离开自己的主目录随意访问系统数据，确认以下配置：
```
chroot_local_user=YES
chroot_list_enable=YES
# (default follows)
chroot_list_file=/etc/vsftpd/chroot_list
allow_writeable_chroot=YES
```
接着测试：
```
[admin@centos7_test_server ~]$ sudo systemctl stop vsftpd.service
[admin@centos7_test_server ~]$ sudo systemctl start vsftpd.service
[admin@centos7_test_server ~]$ ftp 39.108.51.xx
Connected to 39.108.51.xx (39.108.51.xx).
220 (vsFTPd 3.0.2)
Name (39.108.51.xx:admin): obd4g
331 Please specify the password.
Password:
500 OOPS: could not read chroot() list file:/etc/vsftpd/chroot_list
Login failed.
ftp> ls
421 Service not available, remote server has closed connection
Passive mode refused.
ftp>
```
出错了！！！原因在于没有 /etc/vsftpd/chroot_list文件，直接"sudo touch /etc/vsftpd/chroot_list"创建一个空文件即可。
我们再次测试
```
[admin@centos7_test_server ~]$ sudo touch /etc/vsftpd/chroot_list
[admin@centos7_test_server ~]$ sudo systemctl stop vsftpd.service
[admin@centos7_test_server ~]$ sudo systemctl start vsftpd.service
[admin@centos7_test_server ~]$ ftp 39.108.51.xx
Connected to 39.108.51.xx (39.108.51.xx).
220 (vsFTPd 3.0.2)
Name (39.108.51.xx:admin): obd4g
331 Please specify the password.
Password:
230 Login successful.
Remote system type is UNIX.
Using binary mode to transfer files.
ftp> ls
227 Entering Passive Mode (0,0,0,0,60,106).
150 Here comes the directory listing.
drwxr-xr-x    2 1016     1016         4096 Jun 18 07:45 ZKHY
-rw-rw-r--    1 1016     1016            0 Jun 18 07:45 test.txt
226 Directory send OK.
ftp> cd ../
250 Directory successfully changed.
ftp> ls
227 Entering Passive Mode (0,0,0,0,59,176).
150 Here comes the directory listing.
drwxr-xr-x    2 1016     1016         4096 Jun 18 07:45 ZKHY
-rw-rw-r--    1 1016     1016            0 Jun 18 07:45 test.txt
226 Directory send OK.
ftp> ls ../
227 Entering Passive Mode (0,0,0,0,59,230).
150 Here comes the directory listing.
drwxr-xr-x    2 1016     1016         4096 Jun 18 07:45 ZKHY
-rw-rw-r--    1 1016     1016            0 Jun 18 07:45 test.txt
226 Directory send OK.
ftp>
```
剋看到即使我访问上一级目录，我实际访问到的还是主目录。这样我们就把用户限制在了主目录。

## 6.禁止匿名访问
禁止匿名访问秩序配置：
```
anonymous_enable=NO
```
然后重启服务器即可，同时记得把 ftpd_banner取消注释：
```
# You may fully customise the login banner string:
ftpd_banner=Welcome to blah FTP service.
```
测试：
```
[admin@centos7_test_server ~]$ sudo vim /etc/vsftpd/vsftpd.conf
[admin@centos7_test_server ~]$ sudo systemctl stop vsftpd.service
[admin@centos7_test_server ~]$ sudo systemctl start vsftpd.service
[admin@centos7_test_server ~]$ ftp 39.108.51.xx
Connected to 39.108.51.xx (39.108.51.xx).
220 (vsFTPd 3.0.2)
Name (39.108.51.xx:admin): anonymous
530 Permission denied.
Login failed.
ftp>
```
这样我们就不能再使用匿名的方式进行登录了。

## 附录： 

[vsftpd被动模式下端口映射外网无法访问的问题](https://www.cnblogs.com/huangweimin/articles/10468637.html)
[Linux vsftpd 内网服务器 被动模式配置外网访问 部分填坑](https://blog.csdn.net/hajistark/article/details/82954777)
[vsftpd设置被动模式](https://www.cnblogs.com/linn/p/4169986.html)
[CentOS7 安装vsftpd 服务器](https://blog.csdn.net/uq_jin/article/details/51684722)
[FTP协议主动（Port）模式和被动（Passive）两种模式详解](https://www.cnblogs.com/linn/p/4169986.html)
[Centos7 安装配置FTP服务器](https://www.jianshu.com/p/4409cb0d4f73)
