# ProjectVoid
这是基于 <a href="https://github.com/Ninetyfiv3/ProjectVoid">Ninetyfiv3/ProjectVoid</a> 学习改动的项目。

### 代码规范
当前项目使用 <a href="https://google.github.io/styleguide/cppguide.html#Variable_Names">google styleguide</a> 代码命名规范  
主要有以下应用：  
1. 文件名：全小写没有间隔符号
2. 类型名称:大写字母开头，对于每个新单词首字母大写
3. 函数名：大写字母开头，应该是动词或动词短语，用于描述函数执行的操作
4. 普通变量名：单词全小写以下划线间隔
5. 类的成员变量：类似普通变量名，但是以下划线结尾
6. 常量：以前导“k”命名，后跟通过混合大小写
---  
为了减少项目体积大小删除了背景音乐资源，可以到res.qrc添加，并到SoundPlayer配置路径  
代码含有中文注释，用MSVC编译器编译。

---  

## 与原项目的差异：
1. 删除各个类中的字体加载，将其放到GameWidget实现一次即可，原项目内存占用越来越大主要是这个原因
2. 合并QGraphicsButton类，选择信号合并到一起
3. 合并Weapon类，将各种武器类合并到一起
4. 合并Enemy类，将各类敌人类合并到一起
5. 合并升级道具操作到UpgradeMod类实现，一些升级道具只能升级一次，改了几个图标文件名
6. 单例GlobalTimer，实现主计时器
7. 单例SoundPlayer，实现各个音效的管理
8. 单例Log类实现打印游戏信息，正向反馈给玩家。
9. 另外设计了游戏菜单，删除原有左上角的按钮
10. 另外设计了游戏结束界面
11. 增加了自动开火的功能
12. 改善了开始游戏后，释放开始界面资源

// TODO 碰撞检测优化，移动优化，重新开始游戏会存在的问题
// TODO 在人物移动的时候画面卡顿

---
## 画面预览
升级道具直接显示效果描述  
![](/preview/levelup.png "levelup")  

游戏菜单和左下角的日志显示  
![](/preview/gamemenu.png "gamemenu")  

游戏结束界面  
![升级道具直接显示效果](/preview/gameend.png "gameend")
