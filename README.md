#uefi原理与编程
<div class="vt" id="wikimaincol">
 <h1><a name="编译源代码的步骤："></a>编译源代码的步骤：<a href="#编译源代码的步骤：" class="section_anchor"></a></h1><ul><li>在EDK根目录下建立uefi目录。 </li><li>将文件夹book复制到uefi目录。 </li><li>打开CMD命令行 </li><li>在命令行切换到EDK2根目录 </li><li>在命令行执行 </li><ul><li><tt> edksetup.bat --nt32</tt> </li></ul><li>在命令行执行 </li><ul><li><tt> build -p uefi\book\Nt32Pkg\Nt32Pkg.dsc </tt> </li><li><tt> build -p uefi\book\AppPkg\AppPkg.dsc </tt> </li><li><tt> build -p uefi\book\GUIPkg\GUIPkg.dsc </tt> </li></ul></ul>
 </div>
 
<div class="vt" id="wikimaincol">
<h1>勘误</h1>
 <p>1. <a title="感谢robin.xu" rel="nofollow">第15页，2.1.2 配置EDK2 开发环境</a><br> <font color="#555555">原文</font> ：C:\&gt; EDK2Edksetup.bat<br>  <font color="#555555">修订</font> ：<strong><font color="#ff0000">C:\EDK2&gt;</font></strong> Edksetup.bat  </p><p>2.<a title="感谢robin.xu" rel="nofollow">第7页 （2）SEC阶段执行流程</a><br> <font color="#555555">原文</font> ：2）从实模式转换到32位平坦模式（包含模式）<br> <font color="#555555">修订</font> ：2）从实模式转换到<strong><font color="#ff0000">保护模式、32位平坦内存模型</font></strong><br> </p><h3><a name="感谢"></a>感谢<a href="#感谢" class="section_anchor"></a></h3><p><a rel="nofollow">感谢robin.xu</a> </p>
 </div>
