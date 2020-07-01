# OBD4G 测试服务器 FrameServer 编译
```
/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : EC20-FTP.md
* Author             : Merafour
* Last Modified Date : June 21, 2020 
* Description        : OBD4G 测试服务器 FrameServer 编译.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
* https://github.com/TheTravels/Bookmarks/tree/master/Note/Linux
******************************************************************************/
```
FTP:39.108.51.99:21
user:obd4g
passwd:obd.4g

## 1.Login to FTP Server 
```
AT+QICSGP=1,1,"UNINET","","",1    

OK
AT+QIACT=1 

OK
AT+QIACT?

+QIACT: 1,1,1,"10.40.79.31"

OK
AT+QFTPCFG="contextid",1

OK
AT+QFTPCFG="account","obd4g","obd.4g"

OK
AT+QFTPCFG="filetype",1

OK
AT+QFTPCFG="transmode",1

OK
AT+QFTPCFG="rsptimeout",90

OK
AT+QFTPOPEN="39.108.51.99",21

OK

+QFTPOPEN: 0,0
```

## 2.Folder Operation 
```
AT+QFTPCWD="/"  

OK

+QFTPCWD: 0,0
AT+QFTPPWD 

OK

+QFTPPWD: 0,/
AT+QFTPMKDIR="TEST"

OK

+QFTPMKDIR: 0,0
AT+QFTPRENAME="TEST","TEST_NEW"  

OK

+QFTPRENAME: 0,0
AT+QFTPRMDIR="TEST_NEW" 

OK

+QFTPRMDIR: 0,0
```

## 3.File Operation 
```
AT+QFTPCWD="/"

OK

+QFTPCWD: 0,0
AT+QFTPPWD

OK

+QFTPPWD: 0,/
AT+QFTPSIZE="test.txt"

OK

+QFTPSIZE: 0,9
AT+QFTPMDTM="test.txt"

OK

+QFTPMDTM: 0,"20200621093626"

```


AT+QFTPCFG="account","obd4g","obd.4g"                  //Set user name and password. 
AT+QFTPOPEN="39.108.51.99",21

AT+QFTPSIZE="test.txt"
AT+QFTPMDTM="test.txt"
AT+QFTPLIST=".","RAM:test.txt"
AT+QFTPLIST="ZKHY","COM:"
AT+QFTPNLST=".","RAM:nlst.txt"
AT+QFTPGET="tcp.cpp","COM:"
+QFTPGET: 0,505927

AT+QFTPGET="tcp.cpp","RAM:test.txt"

AT+QFDWL="RAM:test.txt"
AT+QFTPGET="tcp.cpp","COM:",0,2048
AT+QFTPGET="tcp.cpp","COM:",2048,2048
AT+QFTPGET="tcp.cpp","COM:",4096,20480
AT+QFTPGET="tcp.cpp","COM:",6144,2048
AT+QFTPGET="tcp.cpp","COM:",8192,2048
AT+QFTPCLOSE 
## 附录



