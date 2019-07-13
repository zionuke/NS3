## ns3中如何通过命令行改变文件参数运行

```bash
#以下2行告诉我们可以通过命令行传入参数进行编译
CommandLine cmd;
cmd.Parse(argc, argv);
#具体方法如下
#先声明一个变量
uint32_t usersNum = 4;
#再调用cmd的AddValue函数，最后一个参数为你要通过命令行修改的变量
cmd.AddValue("userNumber", "number of users pre beam", usersNum);
#之后在命令行输入如下命令即可，不管该变量原始值如何，会修改为你设置的值，这里是4
./waf --run "scratch/training1 --userNumber=4"
```

NS3编译运行程序基本步骤如下

```bash
#1.首先是进入相应目录，不说了
cd ns3-simulation/ 
#拉取最新的分支 
git clone ....
#设置编译选项 
./waf configure --enable-examples --enable-tests 
#编译 
./waf 
#验证，这里first是一个cpp文件名
./waf --run first
#具体跑scratch目录下某个命令
./waf --run "scratch/training1 --userNumber=4 --distAllocer=1 --distance=200 --choice=3"
```

