# COOLFLY-地面端

标签（空格分隔）： 未分类

---

推荐配置:

+ 系统: Ubuntu 16.04
+ SDK 版本: gcc-arm-none-eabi-5_2-2015q4

## 一、安装 SDK 

国内用户推荐下载使用

1.  下载 gcc 编译器；

    ```
    https://launchpadlibrarian.net/231142403/gcc-arm-none-eabi-5_2-2015q4-20151219-linux.tar.bz2 
    ```
    
2. 切换到 SDK 下载目录（一般为 Downloads)，解压到 `/opt/toolchain/`；
    
    ```
    sudo mkdir -p /opt/toolchain/ && cd ~/Downloads 
    
    sudo tar jxvf gcc-arm-none-eabi-5_2-2015q4-20151219-linux.tar.bz2 -C /opt/toolchain/
    
    ls /opt/toolchain/
    ```
3. 安装相关工具
    
    ```
    sudo apt-get install gawk
    ```
4. 下载地面 SDK

    4.1 国内用户推荐[码云-Gnd](https://gitee.com/flyiscool/Gnd) 下载 
    
    ```
    mkdir -p ~/coolfly && cd coolfly
    
    git clone https://gitee.com/flyiscool/Gnd.git
    
    cd Gnd
    ```
    
    4.2 国外用户推荐[Github Gnd](https://github.com/flyiscool/Gnd) 下载
    
    ```
    mkdir -p ~/coolfly && cd coolfly
    
    git clone https://github.com/flyiscool/Gnd.git
    
    cd Gnd
    ```
    
5. 编译使用
    
    ```
    cd AR8020SDK/Application/
    
    make
    ```
    在 `AR8020SDK/Application/` 目录下会生成一个 `app.bin`， 在该目录下，使用指令 `make upload`， 连接 USB 即可上传到遥控器！

> 注意： 切勿讲地面端程序烧录到天空端板子里面，也不可以将天空端程序烧录到地面端板子里面去！！！
    
## 二、目录介绍

### 2.1 目录结构
```
.
├── AR8020SDK
│   ├── Application
│   ├── Build
│   ├── Example
│   ├── Inc
│   ├── Lib
│   └── Utility
├── AR8020SDK_00.01.15
│   ├── c201d
│   └── Example
└── Staging
    ├── Inc
    └── Lib
```

#### AR8020SDK
+ `Application`： 应用层代码，这个里面包含三个 CPU 的入口函数 `main` 以及上传的脚本文件;
+ `Build`: 编译配置文件，一般不需要进行更改；
+ `Example`： 示例文件，里面包含提供的示例代码，可作参考；
+ `Inc`： 头文件
+ `Lib`： 各个 CPU 依赖的已经编译好的库文件，一般无需更改；
+ `Utility`： 工具脚本，链接三个 CPU 以及配置文件，生成最终的 bin 文件；

#### AR8020SDK_00.01.15
+ `c201d`： 板子名称，里面包含压缩包以及 md5 文件,`ground_app.bin`使我们提供的，可以直接烧录；
+ `Example`： 示例文件；