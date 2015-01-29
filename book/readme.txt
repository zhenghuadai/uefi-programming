编译源代码的步骤(EDK2更新到版本号13087)：
0. svn co https://svn.code.sf.net/p/edk2/code/trunk/edk2 -r 13087
1. 在EDK2根目录下建立uefi目录。
2. 将文件夹book复制到uefi目录。
3. 打开CMD命令行
4. 在命令行切换到EDK2根目录
5. 在命令行执行
    edksetup.bat
6. 在命令行执行
    build -p uefi\book\Nt32Pkg\Nt32Pkg.dsc
    build -p uefi\book\AppPkg\AppPkg.dsc
    build -p uefi\book\GUIPkg\GUIPkg.dsc

编译源代码的步骤(UDK2014)：
1. 在EDK2根目录下建立uefi目录。
2. 将文件夹book复制到uefi目录。
3. 打开CMD命令行
4. 在命令行切换到EDK2根目录
5. 在命令行执行
    edksetup.bat
6. 将uefi\book\Nt32Pkg\Nt32Pkg.inc文件添加到Nt32Pkg\Nt32Pkg.dsc末尾
    type uefi\book\Nt32Pkg\Nt32Pkg.inc >> Nt32Pkg\Nt32Pkg.dsc
6. 在命令行执行
    build

源文件索引
第1章
第2章
第3章
  infs\
第4章  
第5章
  systemtable\
第6章
  Event\
  mouse\
第7章
  disk\
  FileIo\
第8章
  ffmpeg\
第9章
  auido\
第10章
  GcppPkg\
  CppPkg\
第11章
  Guibasics\
第12章
  GUIPkg\
  ffmpeg\ffplayer\
第13章
  multitask\
第14章
  Network\
第15章
  sstdPkg\
  std\
第16章
  Shell\
