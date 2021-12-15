# FAstar
## 算法介绍
本项目提出了一种基于优化边缘搜索的贝叶斯网络学习研究方法 FAstar
(1)构建搜索树，在拓展过程中进行深度优先搜索，实现对搜索深度的限制，减少了无效搜索的花销(2)采用线性表作为搜索节点的存储结构，保存节点的唯一标志信息，在方便检索的同时大大减少内存空间占用。

## 操作环境配置
该项目需要在Ubuntu系统上运行，以下配置假定使用纯Ubuntu 18.04 x64
安装编译器gcc-4.8.5 和 g++-4.8.5 和 依赖库boost1.63.0

## 根据分数文件获取贝叶斯网络
本模块将介绍如何在FAstar算法中生成贝叶斯网络。需要代码包DConBN_Code，请参见下载文件。
### 建立贝叶斯网络
```
(1)使用 "make" 命令构建项目
	cd FAstar
	make
(2)将pss文件放到以下目录:FAstar/dist/Debug_astar/GNU-Linux-x86
(3)使用 astar-debug.exe 来获取网络
  xxx@xxx:~/FAstar/dist/Debug_astar/GNU-Linux-x86$ ./astar-debug asia_s500_v1.pss
  3, 0, 2, 5, 1, 4, 6, 7, 
  best parents :
   optimalParents[0] = {}
   optimalParents[1] = {5, }
   optimalParents[2] = {}
   optimalParents[3] = {}
   optimalParents[4] = {1, }
   optimalParents[5] = {2, 3, }
   optimalParents[6] = {5, }
   optimalParents[7] = {3, 4, }
  Nodes expanded: 257
  shortpath, nodes.size = 256
  running time: 0.013875 s!
  Memory footprint(KB)=29968

注意：
1. 该程序的主要功能在文件中：a_star_internal_main.cpp
2. 如果要修改参数值设置，请特别注意对文件score_cache.h的修改
3. 如果要添加约束，则可以使用命令“ ./astar-debug --help”来查找命令的格式
```
