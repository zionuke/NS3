## 一.生成新模块

​	在许多研究情况下，用户将不会满足现已存在的模型，可能需要用全新的方式扩展模拟器的内核，这一次将列举一个给ns-3添加ErrorModle的例子来介绍如何生成新模块。

#### 1.设计方法

​	想让新模块工作，首先需要考虑如下事情：

​	①功能：它应该有什么样的功能？应该具备什么样的属性？

​	②可重复性：多少人可能重复我的设计？用户如何整合模型进行模拟？

​	③依赖：怎样减少新的代码对外部的依赖？如何使它更加模块化？举例来说，如果想让它也可用于IPv6，是否应该避免任何IPv4上的依赖？

​	ns-2中有个ErrorModel，它允许数据分组传递给一个有状态的对象，这个对象根据随机变量确定该数据分组是否已经损坏，调用者可以据此决定如何处理这个分组（例如放弃等）。这个ErrorModel的主要API是传递数据分组的函数，这个函数的返回值是一个布尔值，告诉调用者是否损坏已经发生。注意：根据ErrorModel，分组数据缓冲器可能会被损坏，然后调用这个函数“IsCorupt()”。

​	目前，在我们的设计中，有如下代码：

```c++
class ErrorModel
{
public：
/**
*\如果分组被损坏返回true
*/
bool IsCorrupt（Ptr<Packet>pkt）；
}；
```

​	请注意，本代码不是通过一个const指针去允许函数修改数据分组。实际上，并不是所有的ErrorModel都修改数据分组，但应记录分组数据缓冲区是否已损坏。你也许会想在这一点上，“为什么不能让IsCorupt()成为虚拟方法？”。一种方法是让公共非虚拟函数间接地连接一个私有虚拟函数（这在C++被称为非虚拟接口，并适用于ns-3ErrorModel类）。

​	接下来，这个装置应该依赖于IP或其他协议么？我们不希望建立对互联网协议（误差模型应该是适用于非互联网协议）的依赖，所以要记住这一点。另一个需要考虑的是对象如何包含这个错误模型。我们设想将其在NetDevice上实现，比如：

```c++
/**
*将ErrorMode1连接到PointToPointNetDevice
*PointToPointNetDevice 可任选分组中有ErrorMode1的接收链
*/
void PointToPointNetDevice:：SetReceiveErrorModel（Ptr<ErrorMode1>em）；
```

同样地，这不是唯一的选择，有误差模型可以进行聚合以达到许多其他的目的，但在满足主要用途的情况下，是允许用户在NetDevice水平出现强制错误的。经过一番思考和寻找现有ns-2的代码，找到如下一个示例，是API的基类和第一个子类，可用于初步审查发布。

```c++
class ErrorMode1
{
    public: 
    ErrorModel(); 
    virtual~ErrorModel(); 
    bool IsCorrupt (Ptr<Packet>pkt); 
    void Reset (void); 
    void Enable (void); 
    void Disable (void); 
    bool IsEnabled (void)const; 
    private:
    virtual bool DoCorrupt（Ptr<Packet>pkt）=0；
    virtual void DoReset（void）=0；
}；
enum ErrorUnit
{
EU BIT，
EU BYTE，
EU PKT
}；
//确定哪些数据分组对有潜在的差错
//随机变量分布、误差率、速率单元
class RateErrorModel:public ErrorModel
{
public: 
RateErrorModel(); 
virtual ~RateErrorModel(); 
enum ErrorUnit GetUnit (void)const; 
void SetUnit (enum ErrorUnit error unit); 
double GetRate (void)const;
void SetRate (double rate); 
void SetRandomVariable (const RandomVariable &ranvar); 
private: 
virtual bool DoCorrupt (Ptr<Packet>pkt); 
virtual void DoReset (void);
};
```



#### 2.脚手架(scaffolding)

​	当你准备好开始实施，对于你想建立什么要有一个相当清晰的画面，可能已经经过一些初步审查或征求建议。接着下一个步骤是创建脚手架，并记录详细的设计过程。

​	你应该考虑定义脚手架或者将落实非功能性的骨架模型，一旦API整合是正确的，就能添加功能。需要注意的是将要为模型做出如下修改，因为如果你跟随误差模型，会发现你的代码与现有的误差模型产生碰撞。

①查看ns3编码风格文档。
你需要读取ns3编码风格文档，特别是如果考虑把代码贡献给这个工程，将编码风格文档链接在项目的主页上：ns-3 coding style。

②确定模型驻留的源代码树。
所有的ns3的模型源代码都在目录src下，你需要选择在哪个子目录驻留。如果是一个新的模型代码，将它放在src目录下，将便于同系统结合。

③waf and wscript。
ns3使用WAF构建系统。所以你将要使用WAF构建系统整合新的ns-3。你将整合新的源文件到系统中，这需要把文件加到每个目录的wscript文件里。
拿到空文件error-model.h和error-model.cc，把它加入到src/network/wscript。将.cc的文件加入到剩下的源文件中，将.h的文件加到头文件的列表中。

```c++
include guards
加一些include guards到头文件中。
#ifndef ERROR_MODEL_H
#define ERROR_MODEL_H
...
#endif
```

④namespace ns3
ns-3使用ns3命名空间，通常情况下，用户将ns3命名空间块添加到.cc和.h的文件中。

```c++
namespace ns3{
...
}
```

基于这一点，我们有一些框架文件可以设定新类，头文件像这样：

```c++
#ifndef ERROR MODEL H
#define ERROR MODEL H 
namespace ns3{
}//namespace ns3
#endif
```

然而error-model.cc文件像这样：

```c++
#include"error-model.h"
namespace ns3（
}//namespace ns3
```

这些文件应该编译，因为它们没有什么实质性的内容，现在开始添加类。



#### 3.初次执行

​	在这点上，仍然在脚手架上工作，但是开始定义类，以便稍后添加功能。

​	①使用Object类么？

​	这是一个重要的设计步骤，使用Object类作为新类的基类。本章中所描述的ns-3的Object 模型是从Object类继承而得到的特殊属性：

- ns-3的类型和属性系统；
- 对象聚合系统；
- 智能指针参考计数系统（PKT类）。

派生的类从ObjectBase类中得到上述前2个属性，但是没有得到智能指针。派生的类从RefCountBase类中只获得智能指针参考计数系统。在实践中，时常遇到的是上述类Object的3种变形。在本节的例子中，大家要利用属性系统，将通过此对象的实例来探究ns-3公共的API，所以Object类是恰当的。

②初始类
刚开始时可以定义一些最低限度的功能，看看是否编译，大家回顾一下从Object类派生的时候需要实现些什么。

```c++
#ifndef ERROR_MODEL_H
#define ERROR_MODEL_H
# include "ns3/object.h"
namespace ns3{
class ErrorMode1: public Object 
{
public: 
static TypeId GetTypeId (void); 
ErrorModel(); 
virtual~ErrorModel();
}; 
class RateErrorModel: public ErrorModel
{
public: 
static TypeId GetTypeId(void); 
RateErrorModel(); 
virtual ~RateErrorModel();
};
# endif
```

有几件事情需要注意，**需要object.h，ns3中的惯例是，如果头文件位于同一目录中，可以没有任何路径前缀。因此，如果读者在src/core/model目录下实施ErrorModel，只能写#include"object.h"。但是在src/network/model目录下实施时，要写#include"ns3/object.h"。还要注意这种超出命名空间的声明。其次，每个类都必须实现一个静态公有成员函数GetTypeld（void）。再次，自己执行构造函数和析构函数，而不是让编译器生成，并且使析构函数虚拟化。在C++中注意到，如果拷贝赋值运算符和拷贝构造函数没有被定义，它们会自动生成。所以如果你不希望这样，应该把它们作为私有成员。**

看看cc文件中相关框架实施代码：

```c++
# include "error-model.h"
namespace ns3{
NS_OBJECT_ENSURE_REGISTERED (ErrorModel); 
TypeId ErrorModel:: GetTypeId (void)
{
static TypeId tid=TypeId("ns3:: ErrorModel")
. SetParent<Object>()
;
return tid;
}
ErrorMode1:: ErrorModel()
{
}
ErrorMode1::～ErrorModel()
{
}
NS_OBJECT_ENSURE_REGISTERED (RateErrorModel);
TypeId RateErrorModel:: GetTypeId (void)
{
static TypeId tid=TypeId("ns3:: RateErrorModel")
. SetParent<ErrorModel>()
. AddConstructor<RateErrorModel>()
;
return tid;    
}
RateErrorModel:: RateErrorModel()
{
}
RateErrorMode1::~RateErrorModel()
{
}
```

GetTypeld（void）的功能是什么？这个函数有如下几个功能。注册了唯一的串进入typeid系统。它在属性系统中建立了对象的结构层次（通过SetParent建立），某些特定的对象可以通过对象创建框架来建立（AddConstructor）。每个类都需要宏NS_OBJECT_ENSURE_REGISTERED（类名）来定义一个新的GetTypeld方法。



#### 4.加入样本脚本

​	在这点上，大家可能尝试上述定义的脚手架并将其添加到系统中，现在添加到系统中可以用一个更简单的模型，也能反映是否需要做出API修改。一旦做到这一点，将返回到建立ErrorModels本身的功能。
在类中加入基本支持：

```c++
point-to-point-net-device.h 
class ErrorModel；
Ptr<ErrorModel>m_receiveErrorModel；
//添加访问器：
void 
PointToPointNetDevice:：SetReceiveErrorMode1（Ptr<ErrorModel>em）
{
NS_LOG_FUNCTION（this<<em）；
m_receiveErrorModel=em；
}
.AddAttribute（"ReceiveErrorModel"，
"The receiver error model used to simulate packet 1oss"，
PointerValue（），
MakePointerAccessor（&PointToPointNetDevice:：m receiveErrorMode1），MakePointerChecker<ErrorModel>（））
//植入到系统中
void PointToPointNetDevice::Receive (Ptr<Packet>packet)
{
NS_LOG_FUNCTION (this<< packet); 
uint16_t protocol=0; 
if (m_ receiveErrorModel && m_receiveErrorModel->IsCorrupt (packet))
{
//
//如果有个误差模型，并且是时候丢弃一个损坏的分组，不要转发这个分组，丢弃它
//
m_dropTrace(packet);
}
else
{
//
//命中接收跟踪钩，剥离点到点协议头并且在协议栈中转发这个分组
//
mrxTrace（packet）；
ProcessHeader（packet，protocol）；
m_rxCallback（this，packet，protocol，GetRemote（））；
if（！m promiscCallback.IsNull（））
{m promiscCallback（this，packet，protocol，GetRemote（），
GetAddress（），NetDevice:：PACKET HOST）；
}
}
}
//创建空的功能脚本
simple-error-model.cc
//误差模型
//添加一个误差模型到节点3的NetDevice
//通过信道和节点指针掌控NetDevice 
Ptr<PointToPointNetDevice>nd3=Point ToPointTopology:：GetNetDevice（n3，channe12）；Ptr<ErrorMode1>em=Create<ErrorModel>（）；
nd3->SetReceiveErrorMode1（em）；
bool 
ErrorMode1：：DoCorrupt（Packet&p）
{
NS LOG FUNCTION；
NS LOG UNCOND（"Corrupt！"）；
return false；
}
```

在这点上，大家可以运行这个程序（ErrorModel植入到Point ToPoint NetDevice的接收路径上），节点3接收到的每个分组打印出字串“Corrupt！”，下一步回到误差模型添加子类。



#### 5.添加子类

​	基类ErrorModel提供了一个有用的基类接口（Corrupt()和Reset()），可以成为虚函数的子类。可以称之为BasicError-Model，从用户界面角度来看，作者想令用户轻易换出在NetDevice 使用的ErrorModel并且能够设置配置参数。
如下内容大家需要考虑：

- 能够设置管辖损失的随机变量（默认为UniformVariable）；
- 能够设置粒度的单位（位、字节、分组、时间）；
- 能够对应上述单元粒度设置错误率（例如，10的-3次方）；
- 能够启用/禁用（默认启用）。

定义BasicErrorModel为ErrorModel的子类，如下：

```c++
class BasicErrorModel:public ErrorModel 
{
public：
static TypeId GetTypeId（void）；
..
private：
//实现基类纯虚函数
virtual bool DoCorrupt（Ptr<Packet>p）；
virtual bool DoReset（void）；
..
}
并且通过设置独特的TypeId字符串来配置子类GetTypeId函数
TypeId RateErrorModel:：GetTypeId（void）
{
static TypeId tid=TypeId（"ns3：：RateErrorModel"）
.SetParent<ErrorModel>（）
.AddConstructor<RateErrorMode1>（）
...
```



## 二.添加新模块

当你创建一组相关的类、实例和测试之后，可以组合到一起进入ns-3模块，就能够被其他研究人员使用。

#### 1.熟悉模块布局

​	所有的模块都可以在src目录下找到。每个模块都可以在模块所具有的名称目录中找到。一个典型的模块具有如下的目录结构和所需文件。

src/
module-name/
bindings/
doc/
examples/
wscripthelper/
model/
test/
examples-to-run.py 

wscript

#### 2.基于模板模块创建新的模块

源目录中提供的python程序能够为新的模块创建框架。
src/create-module.py

我们将假设新的模块称为“新模块”。从src目录中，执行以下操作来创建新的模块：

```c++
./create-module.py new-module
```

然后进入新的模块文件夹会看到新的文件
Examples  helper  model  test  wscript

接下来研究如何自定义这些模块，所有的ns-3模块都依赖于核心模块，在wscript文件中指定了这种依赖关系。假设新模块依赖互联网、移动模型和AODV模块，然后在编辑之前调用这个模块函数：

```c++
def build（bld）：
module=bld.create ns3 module（'new-module'，['core']）
```

编辑之后：

```c++
def build（b1d）：
module=b1d.create ns3 module（'new-module'，['internet'，'mobility'，'aodv']）
```

你的模块将极有可能拥有模型源文件，在model/new-module.cc和model/new-module.h.中建立初始框架。
如果你的模块有帮助源文件，它们会在Helper/directory路径下，初始框架将在此目录下建立。
最终，test/directory目录下会建立框架测试套件和测试用例，如下的构造函数指定名称为新模块的单元测试。

```c++
New-moduleTestSuite:：New-moduleTestSuite（）
：TestSuite（"new-module"，UNIT）
{
AddTestCase（new New-moduleTestCase1）；
}
```

#### 3.加入到模块的源文件

如果你的模块有model源文件或Helper源文件，那么它们必须在如下路径被指定：
src/new-module/wscript

用文本编辑器修改文件。举例说明，spectrum模块的源文件可以在src/spectrum/wscript中被指定。有如下源文件：

```c++
module.source=[
'mode1/spectrum-model.cc'，
'mode1/spectrum-value.cc'，
...
'mode1/microwave-oven-spectrum-value-helper.cc'，
'helper/spectrum-helper. cc', 
'helper/adhoc-aloha-noack-ideal-phy-helper. cc', 
'helper/waveform-generator-helper. cc',' 
'helper/spectrum-analyzer-helper. cc',
]
```

#### 4.指定模块的头文件

​	如果新模块有model头文件和helper头文件，那么需要在src/new-module/wscript中被指定。用文本编辑器修改文件。
举例说明，spectrum模块的头文件可以在src/spectrum/wscript中被指定。伴随如下的函数调用、模块名字、头文件列表。注意到函数 new_task_gen()的参数令waf安装这个模块的头。

```c++
headers=bld.new_task_gen(features=['ns3header']）
headers.module='spectrum'
headers.source=[
’mode1/spectrum-model.h'，
 mode1/spectrum-value.h'，
 ...
 mode1/microwave-oven-spectrum-value-helper.h'，
 helper/spectrum-helper.h'，
 helper/adhoc-aloha-noack-ideal-phy-helper.h'，
 helper/waveform-generator-helper.h'，
 helper/spectrum-analyzer-helper.h'，
]
```

#### 5.指定模块测试

如果新的模块有测试，那么必须在如下路径被指定：
src/new-module/wscript

用文本编辑器修改文件。举例说明，spectrum模块的测试在src/spectrum/wscript中被指定。有如下函数调用和测试套件：

```c++
module_test=bld.create_ns3_module_test_library（'spectrum'）
module_test.source=[
Itest/spectrum-interference-test.cc'，
Itest/spectrum-value-test.cc'，
]
```

#### 6.指定模块实例

如果新的模块有实例，那么必须在如下路径被指定：
src/new-module/examples/wscript

用文本编辑器修改文件。举例说明，核心模块的实例在src/core/examples/wscript中被指定。核心模块的C++实例被如下的函数调用和源文件所指定。注意到函数create_ns3_program()的第二个参数就是程序被创建所依赖的模块列表。

```c++
obj=bld.create_ns3_program（'main-callback'，['core']）
obj.source='main-callback.cc'
obj=bld.create_ns3_program（'sample-simulator'，['core']）
obj.source='sample-simulator.cc'
//核心模块中Python实例利用如下函数调用被指定，注意到函数register_ns3_script()的第二个参数就是Python实例所依赖的模块列表：
bld.register_ns3_script（'sample-simulator.py'，['core']）
```

#### 7.指定模块中的实例做测试

​	测试框架可以执行实例程序来捕捉回归。然而并非所有的实例都适合回归测试。在每个模块的测试目录中有一个文件叫examples-to-run.py，可以控制实例的调用。
举例说明，被test.py 运行的实例在src/core/test/examples-to-run.py中被指定。使用如下两列C++和python实例。

```c++
#一列C++实例的运行是保证随着时间的推移它们仍然可创造和运行。
#在列表中每个元组包含
#（example_name，do_run，do_valgrind_run）.
#
cpp examples=[
（"main-attribute-value"，"True"，"True"），
（"main-callback"，"True"，"True"），
（"sample-simulator"，"True"，"True"），
（"main-ptr"，"True"，"True"），
（"main-random-variable"，"True"，"True"），
（"sample-random-variable"，"True"，"True"），
]
#一列python实例的运行保证随着时间推移它们仍然可运行
#在列表中每个元组包含
#（example name，do run）.
#
python examples=[
（"sample-simulator.py"，"True"），
]
C++实例列表中的每个元组包含（example_name，do_run，do_valgrind_run）。
请注意python 语句是依靠waf配置变量实现的，例如：
（"tcp-nsc-1fn"，"NSC ENABLED==True"，"NSC_ENABLED==False"）
Python 实例列表中每个元组包含（example_name，do_run）
Example_name 是python脚本运行的位置，do_run是实例运行的条件。请注意python 语句依赖waf配置变量。例如（"realtime-udp-echo.py"，"ENABLE_REAL TIME==False"）。
如果新的模块有实例，必须指定在src/new-module/test/examples-to-run.py目录下哪些应该运行。用文本编辑器修改文件，这些实例由test.py运行。
```

#### 8.建立和测试模块新的模块

```c++
./waf configure --enable-examples --enable-tests
./waf build
./test.py
```

​	本小节主要介绍了两部分：生成模块和添加模块。生成模块时，读者需要研究设计方法与脚手架的相关问题，并且添加脚本和子类。添加模块时，遵循如下几个步骤：熟悉模块布局；基于模板模块创建新的模块；加入到模块的源文件；指定模块的头文件；指定模块测试；指定模块实例；指定模块中的实例做测试；建立和测试新的模块。