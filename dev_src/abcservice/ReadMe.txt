========================================================================
    控制台应用程序：abcservice 项目概述
========================================================================

应用程序向导已为您创建了此 abcservice 应用程序。

本文件概要介绍组成 abcservice 应用程序的每个文件的内容。


abcservice.vcxproj
    这是使用应用程序向导生成的 VC++ 项目的主项目文件，
    其中包含生成该文件的 Visual C++ 
    的版本信息，以及有关使用应用程序向导选择的平台、配置和项目功能的信息。

abcservice.vcxproj.filters
    这是使用“应用程序向导”生成的 VC++ 项目筛选器文件。 
    它包含有关项目文件与筛选器之间的关联信息。 在 IDE 
    中，通过这种关联，在特定节点下以分组形式显示具有相似扩展名的文件。
    例如，“.cpp”文件与“源文件”筛选器关联。

abcservice.cpp
    这是主应用程序源文件。

/////////////////////////////////////////////////////////////////////////////
其他标准文件：

StdAfx.h，StdAfx.cpp
    这些文件用于生成名为 abcservice.pch 的预编译头 (PCH) 文件和
    名为 StdAfx.obj 的预编译类型文件。

/////////////////////////////////////////////////////////////////////////////
其他注释：

应用程序向导使用“TODO:”注释来指示应添加或自定义的源代码部分。

/////////////////////////////////////////////////////////////////////////////




16号，上午修改如下部分的配置数据。如果有异常，把如下的数据还原回。即可
查找全部 "../data/mu-68-1d.conf", 大小写匹配, 全字匹配, 子文件夹, 查找结果 1, "整个解决方案"
  D:\windows phone7\Browser 备份文件夹\FriendouBrowserService备份\abcservice 1-16 stasm方式实现尝试版\abcservice\stasm\asmsearch.hpp(64):    const char sConfFile0[]="../data/mu-68-1d.conf", // in: 1st config filename
  D:\windows phone7\Browser 备份文件夹\FriendouBrowserService备份\abcservice 1-16 stasm方式实现尝试版\abcservice\stasm\stasm_dll.cpp(30):        strcpy(sConfFile0, "../data/mu-68-1d.conf");
  D:\windows phone7\Browser 备份文件夹\FriendouBrowserService备份\abcservice 1-16 stasm方式实现尝试版\abcservice\winapps\iland\asmsearch1.hpp(26):    const char sConfFile0[]="../data/mu-68-1d.conf", // in: 1st config filename
  匹配行: 3    匹配文件: 3    合计搜索文件: 355


  warper.cpp文件。修改操作半径。
  	Rectangle bound(ptTopLeft.X, ptTopLeft.Y,
			std::min(m_imgOriginal->Width - ptTopLeft.X, 4*iBrushSize),
			std::min(m_imgOriginal->Height - ptTopLeft.Y, 4*iBrushSize));