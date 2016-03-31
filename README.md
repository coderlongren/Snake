简单贪吃蛇
===

* 该程序是Unix系统下的贪吃蛇，没有提供对Windows系统的支持
* 该程序只实现了贪吃蛇的核心算法及最基本的功能
* 该程序主要是供初级C语言学者学习交流

## 编译运行

```sh
cd ~/path/to/your/workspace
git clone git@github.com:huster-zhangpeng/Snake.git
cd Snake
gcc -o snake snake.c
./snake
```

也可以使用cmake执行外部编译

```sh
cd ~/path/to/your/workspace
git clone git@github.com:huster-zhangpeng/Snake.git
cd Snake
mkdir build && cd build
cmake ..
make
./snake

```

## 变量说明

> int world[ HEIGHT ][ WIDTH ]

程序中的world变量，既是贪吃蛇所在的世界舞台，这里它数据结构上是一个二维数组，代表着控制台窗口的坐标系，X轴自左向右，Y轴自上向下，坐标(0, 0)即表示控制台窗口左上角第一个输出字符的地方，`world[y][x]`即表示坐标(x, y)，也是第y行第x列。为简单起见，world的边缘都是'#'墙壁。大家可以想一想为什么使用一个二维数组表示，关于这个变量也有初学者命名成map的，意为地图，很好，不过map在计算机世界更多的意义是hash表，如C++ STL里面的map以及golang里面的map。

在真正的2D即二维游戏开发中，world都是必须有的概念，world是对游戏过程的抽象，然后再借由world反映到屏幕上，即渲染绘制。在正式的2D游戏开发中，可能墙壁，蛇头，以及食物都不再是一个字符表示的，而是换成更加美观逼真的图片代替，以使游戏更加拟物化，增加可玩性。

> direction、DX/DY

在此程序中，枚举类型direction表示方向，有4个，上(UP=0)、右(RIGHT=1)、下(DOWN=2)、左(LEFT=3)，顺时针方向排列，也可以逆时针排列，但不能是上下左右这种，因为为了计算这些方向的反方向，直接把方向加上2然后模4即可。

方向在数学上是个向量，在这个world定义的坐标世界里，我们当然也用(DX, DY)表示的单位向量表示方向，如向上为(0, -1), 向右为(1, 0)，向下为(0, 1)，向左为(-1, 0)，别忘了我们的坐标系怎么定义的。

表示方向的单位向量本该定义为struct类型的，但使用数组更能省掉定义类型的麻烦，于是有了以下:

```C
int DX[4] = {0, 1, 0, -1};
int DY[4] = {-1, 0, 1, 0};
```

表示向上的向量，就是`(DX[UP], DY[UP])`，其他方向类似，加上贪吃蛇是一步一步走的，每次只能沿当前方向前进一个，那蛇头所在的坐标向量加上（向量加法）当前方向向量就是下一步的位置，然后可轻松将下一步的位置反映的world中，并检测是否碰到了墙壁或者是否碰到了自己。

> struct Food_ food

贪吃蛇游戏中，贪吃蛇要吃食物长大，这个食物就用变量food表示，其实也是一个坐标而已。有的初学者将其命名为rock，意为石头，未尝不可。在此不多做解释。

> L, D, X/Y

这些全局变量统统为了表示贪吃蛇的状态，其中L表示贪吃蛇的长度，包括头部；D表示贪吃蛇的方向，是UP、RIGTH、DOWN、LEFT之一；X/Y表示贪吃蛇的身体，`(X[i], Y[i])`就表示第i节（0 < i < L）身体的坐标位置，i = 0时就表示贪吃蛇的头。

这里我为了简单，统统使用了缩写，因为这是个小程序，加上注释无关紧要，但要是在复杂点的大项目中，大家千万不要这么做，因为在你使用变量L、D、X、Y时没人知道这些变量是什么含义。其实更规范的做法是定义贪吃蛇一个结构体，里面有属性长度length, 方向direction，每节身体的坐标集合。身体的集合，为了符合贪吃蛇身体长度可长可短的特点，应该使用链表最为合适，链表的每一个元素都是一个点坐标(x, y)，所以一个点也应该有个结构体定义。这里我偷懒使用了数组表示身体，并没有完全对贪吃蛇世界作很好的抽象，但效率却是最高的哦。

## 流程

在此之前，要说一下，任何一个游戏的流程表现在计算机里面，就是一个while循环，这个循环不停地做检测事件、将事件产生的影响表现到世界上、然后将世界映射到人机交互接口（如cli——命令行接口，图形化界面）上。再扩展一下，目前任何软件，甚至web程序都符合这样的规则，只不过人机交互接口换成了浏览器，事件都是由人机交互过程中产生的。

这个贪吃蛇的流程亦是如此，只不过为了简单，我们的人机交互接口也是最简单的cli，产生事件也是靠键盘产生。捕捉事件，也是使用最简单的读取标准输入来获取的，即`getchar()`，但即使这样一个函数，其内部也是深不可测、妙不可言，这些东西牵涉到操作系统层面流的概念，这里不做介绍。有的同学说是`getch()`，其实是不同的系统不同的API，`getch()`只不过是只存在windows上的函数，而`getchar()`则是所有满足标准C里面的流函数，在所有Unix系统上都是此函数。

既是`getchar()`捕获事件，那每次输入事件时都要按下一个回车键，以使内容送交到输入缓冲区使`getchar()`能够读的到。所以该例程里面，w+enter: 表示向上, s+enter: 表示向下, a+enter: 表示向左, d+enter: 表示向右, q+enter: 表示退出。

需要注意的一点，流是一个缓冲区，每次从标准输入流里面读一个字符后，要把流里面多余的字节清空，以免影响到下一次的事件采集。


