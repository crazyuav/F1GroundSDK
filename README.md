# COOLFLY-Ground SDK Instructions 


The recommended configuration:

+ OS: Ubuntu 16.04
+ Compiler: gcc-arm-none-eabi-5_2-2015q4


## Install the SDK 


1.  Download the  gcc compiler;  

    ```
    https://launchpadlibrarian.net/231142403/gcc-arm-none-eabi-5_2-2015q4-20151219-linux.tar.bz2 
    ```
    
2. Unzip the file `gcc-arm-none-eabi-5_2-2015q4-20151219-linux.tar.bz2`, copy to  `/opt/toolchain/`;  
    
    ```    
    sudo tar jxvf gcc-arm-none-eabi-5_2-2015q4-20151219-linux.tar.bz2 -C /opt/toolchain/
    
    ls /opt/toolchain/
    ```

3. Installation dependency tools:
    ```
    sudo apt-get install gawk
    sudo dpkg --add-architecture i386
    sudo apt-get update
    sudo apt-get install libc6:i386 libgcc1:i386 libstdc++5:i386 libstdc++6:i386
    ```

4. Download the F1GroundSDK
    
    https://github.com/flyiscool/F1GroundSDK
    
    ```
    mkdir -p ~/coolfly && cd coolfly
    
    git clone https://github.com/flyiscool/F1GroundSDK
    
    cd F1GroundSDK
    ```
    
5. Build and Download the firmware
    
    ```
    cd AR8020SDK/Application/
    
    make
    ```
    The `app.bin` in the `AR8020SDK/Application/` is the file we need. 
    Use the `make upload` can download the bin to the F1Ground through the USB.

Note: Don't download the file to the sky side coolfly-f1 board!!!
    
