#uefi原理与编程
<table border=0><tr><td valign='top'><a href="#QuickStart"><b>QuickStart</b><a><br><a href='#Correction'><b>勘误</b></a><br></td><td><img src="http://images.cppblog.com/cppblog_com/djxzh/uefi_principles_and_programming_1.jpg" width="600"></td></tr></table>
<div class="vt" id="wikimaincol">
 <h1><a name="QuickStart">编译源代码的步骤：</a></h1>
 <h3><a name="EDK2源代码版本号为13087"></a>EDK2源代码版本号为13087<a href="#EDK2源代码版本号为13087" class="section_anchor"></a></h3><ul><li>下载EDK2源代码 </li><ul><li>svn co <a href="https://svn.code.sf.net/p/edk2/code/trunk/edk2" rel="nofollow">https://svn.code.sf.net/p/edk2/code/trunk/edk2</a> -r 13087 </li></ul><li>在EDK2根目录下建立uefi目录。 </li><li>将文件夹book复制到uefi目录。 </li><li>打开CMD命令行 </li><li>在命令行切换到EDK2根目录 </li><li>在命令行执行 </li><ul><li><tt> edksetup.bat --nt32</tt> </li></ul><li>在命令行执行 </li><ul><li><tt> build -p uefi\book\Nt32Pkg\Nt32Pkg.dsc </tt> </li><li><tt> build -p uefi\book\AppPkg\AppPkg.dsc </tt> </li></ul><li>把<tt>StdLib\Include\sys\EfiCdefs.h</tt>第330行注释掉 </li><blockquote><tt> #ifdef _NATIVE_WCHAR_T_DEFINED</tt><br> 
<tt>//  #error You must specify /Zc:wchar_t- to the compiler to turn off intrinsic nwchar_t.</tt><br> 
<tt> #endif</tt><br> 
</blockquote><li>在命令行执行 </li><ul><li><tt> build -p uefi\book\GUIPkg\GUIPkg.dsc </tt> </li></ul></ul><h3><a name="UDK2014"></a>UDK2014<a href="#UDK2014" class="section_anchor"></a></h3><ul><li>在EDK2根目录下建立uefi目录。 </li><li>将文件夹book复制到uefi目录。 </li><li>打开CMD命令行 </li><li>在命令行切换到EDK2根目录 </li><li>在命令行执行 </li><ul><li><tt> edksetup.bat --nt32</tt> </li></ul><li>将<tt>uefi\book\Nt32Pkg\Nt32Pkg.inc</tt>文件添加到<tt>Nt32Pkg\Nt32Pkg.dsc</tt>末尾 </li><ul><li><tt>type uefi\book\Nt32Pkg\Nt32Pkg.inc &gt;&gt; Nt32Pkg\Nt32Pkg.dsc</tt> </li></ul><li>在命令行执行 </li><ul><li><tt> build </tt> </li></ul></ul><h3><a name="EDK2源代码版本号为16682"></a>EDK2源代码版本号为16682<a href="#EDK2源代码版本号为16682" class="section_anchor"></a></h3><ul><li>下载EDK2源代码 </li><ul><li>svn co <a href="https://svn.code.sf.net/p/edk2/code/trunk/edk2" rel="nofollow">https://svn.code.sf.net/p/edk2/code/trunk/edk2</a> -r 16682 </li></ul><li>在EDK2根目录下建立uefi目录。 </li><li>将文件夹book复制到uefi目录。 </li><li>打开CMD命令行 </li><li>在命令行切换到EDK2根目录 </li><li>在命令行执行 </li><ul><li><tt> edksetup.bat --nt32</tt>  </li></ul><li>把<tt>StdLib\Include\sys\EfiCdefs.h</tt>第330行注释掉 </li><blockquote><tt> #ifdef _NATIVE_WCHAR_T_DEFINED</tt><br> 
<tt>//  #error You must specify /Zc:wchar_t- to the compiler to turn off intrinsic nwchar_t.</tt><br> 
<tt> #endif</tt><br> 
</blockquote><li>编译<tt>Nt32Pkg</tt> </li><ul><li>将<tt>uefi\book\Nt32Pkg\Nt32Pkg-2.4.inc</tt>文件添加到<tt>Nt32Pkg\Nt32Pkg.dsc</tt>末尾 </li><ul><li><tt>type uefi\book\Nt32Pkg\Nt32Pkg-2.4.inc &gt;&gt; Nt32Pkg\Nt32Pkg.dsc</tt> </li></ul><li>在命令行执行 </li><ul><li><tt> build -p Nt32Pkg\Nt32Pkg.dsc</tt> </li></ul></ul><li>编译<tt>AppPkg</tt> </li><ul><li>将<tt>uefi\book\AppPkg\AppPkg-2.4.inc</tt>文件添加到<tt>AppPkg\AppPkg.dsc</tt>末尾 </li><ul><li><tt>type uefi\book\AppPkg\AppPkg-2.4.inc &gt;&gt; AppPkg\AppPkg.dsc</tt> </li><li><tt> build -p AppPkg\AppPkg.dsc</tt> </li></ul></ul></ul>
 </div>
 
<div id="wikicontent">
<div class="vt" id="wikimaincol">
<p><a title="感谢robin.xu" rel="nofollow0">第7页 （2）SEC阶段执行流程</a><br />
<font color="#555555">原文</font> ：2）从实模式转换到32位平坦模式（包含模式）<br />
<font color="#555555">修订</font> ：2）从实模式转换到<strong><font color="#ff0000">保护模式、32位平坦内存模型</font></strong><br />
</p>
<p>&nbsp;</p>
<p><a>第15页 2.1.1安装所需开发工具</a><br />
<table>
     <tbody border="0">
         <tr>
             <td valign="top">
             <font color="#555555">原文</font> ：</td>
             <td>3）安装IASL编译器（https://www.acpica.org/downloads/binary_tools）<br />
             4）IASL用于编译.asl文件。.asl是高级配置与电源接口（Advanced Configuration and Power Interface）源文件。 <br />
             </td>
         </tr>
         <tr>
         </tr>
         <tr>
             <td valign="top">
             <font color="#555555">修订</font> ：</td>
             <td>3）安装IASL编译器（https://www.acpica.org/downloads/binary_tools）<br />
             &nbsp;&nbsp;&nbsp;&nbsp;IASL用于编译.asl文件。.asl是高级配置与电源接口（Advanced Configuration and Power Interface）源文件。 <br />
             4）<font color="#ff0000">安装NASM编译器（www.nasm.us）</font> <br />
             </td>
         </tr>
     </tbody>
</table>
</p>
<p><a title="感谢robin.xu" rel="nofollow0"><br />
第15页，2.1.2 配置EDK2 开发环境</a><br />
<font color="#555555">原文</font> ：C:\&gt; EDK2Edksetup.bat<br />
<font color="#555555">修订</font> ：<strong><font color="#ff0000">C:\EDK2&gt;</font></strong> Edksetup.bat </p>
<p>&nbsp;</p>
<a title="感谢Max" rel="nofollow0">第35页 5.[Protocol]块 （1）语法</a><br />
<font color="#555555">原文</font> ：<font color="#ff0000"><strong>[LibraryClasses]</strong></font><br />
<font color="#555555">修订</font> ：<font color="#ff0000"><strong>[Protocols]</strong></font>
<p>&nbsp;</p>
<a title="感谢Max" rel="nofollow0">第35页 5.[Protocol]块 （2）示例</a><br />
<font color="#555555">原文</font> ：工程文件的<font color="#ff0000"><strong>[LibraryClasses]</strong></font><br />
<font color="#ff0000"><strong>[LibraryClasses]</strong></font><br />
<font color="#555555">修订</font> ：工程文件的<font color="#ff0000"><strong>[Protocols]</strong></font><br />
<font color="#ff0000"><strong>[Protocols]</strong></font><br />
<p>&nbsp;</p>
<a title="感谢kurtqiao" rel="nofollow0">第89页 [示例 5-5] 分配指定地址处的3个页面</a><br />
<font color="#555555">原文</font> ：Status = gBS-&gt;(AllocateAddress, EfiBootServicesData, 3, &amp;pages); <br />
<font color="#555555">修订</font> ：Status = gBS-&gt;<font color="#ff0000"><strong>AllocatePages</strong></font>(AllocateAddress, EfiBootServicesData, 3, &amp;pages);
<p>&nbsp;</p>
<a title="感谢kurtqiao" rel="nofollow0">第115页 6.3.1提升和恢复任务优先级</a><br />
<font color="#555555">原文</font> ：RaiseTPL和RestoreTPL必须成对出现，执行了RaiseTPL后，必须尽快调用<font color="#ff0000"><strong>RaiseTPL</strong></font>将任务优先级恢复到原来的值 <br />
<font color="#555555">修订</font> ：RaiseTPL和RestoreTPL必须成对出现，执行了RaiseTPL后，必须尽快调用<font color="#ff0000"><strong>ResoreTPL</strong></font>将任务优先级恢复到原来的值
<p>&nbsp;</p>
<a title="感谢kurtqiao" rel="nofollow0">第129页 7.1GPT硬盘</a><br />
<font color="#555555">原文</font> ：20世纪90年代末，英特尔<font color="#ff0000"><strong>(Inter)</strong></font>公司开发了 <br />
<font color="#555555">修订</font> ：20世纪90年代末，英特尔<font color="#ff0000"><strong>(Intel)</strong></font>公司开发了
<p>&nbsp;</p>
<a>第221页，6）执行如下命令播放视频</a><br />
<font color="#555555">原文</font> ：<code style="font-size:large">fs0:&gt; testc.efi piano 2.wav</code><br />
<font color="#555555">修订</font> ：<tt style="font-size:large">fs0:&gt; testc.efi<font color="#ff0000"> <strong>piano.wav</strong></font></tt>
<p>&nbsp;&nbsp;</p>
<a title="感谢kurtqiao" rel="nofollow0">第278页</a><br />
<font color="#555555">原文</font> ：Print(L"字体<font color="#ff0000">加载</font>成功");<br />
<font color="#555555">修订</font> ：Print(L"字体<font color="#ff0000">卸载</font>成功");
<p>&nbsp;</p>
<a title="感谢kurtqiao" rel="nofollow0">第278页</a><br />
<font color="#555555">原文</font> ：Print(L"Failed to <font color="#ff0000">load</font> font: %r\n", Status);//字体<font color="#ff0000">注册</font>失败<br />
<font color="#555555">修订</font> ：Print(L"Failed to <font color="#ff0000">unload</font> font: %r\n", Status);//字体<font color="#ff0000">卸载</font>失败<br />
<p>&nbsp;</p>
<p>------------------------ </p>
<h3><a name="感谢"></a>感谢<a href="#感谢" class="section_anchor"></a></h3>
<p><a rel="nofollow0">感谢robin.xu</a> </p>
<p><a rel="nofollow1">感谢kurtqiao</a> </p>
<p><a rel="nofollow1">感谢Max</a> </p>
</div>
</div>
