/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;
/*不用为了使用ns-3的代码而必须在所有的ns-3代码前打上ns3::作用域操作符，
但如果使用标准C++的内容可能就要加上std::前缀*/

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");
/*用特殊的名字定义一个日志组件。语句实际上是生成一个LogComponent类型的对象g_log，
并且通过构造函数LogComponent(name)初始化，变量name通过宏定义传递参数。*/

int
main (int argc, char *argv[])
{
    CommandLine cmd;
    cmd.parse(argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    /*这2行将“UdpEcho”应用程序的客户端和服务器端的日志级别设为“INFO”级。这样，
    当仿真产生数据分组发送和接收时，对应的应用就会输出相应的日志消息到相关的日志模块。*/

    /*第一行只是声明了一个名为“nodes”的NodeContainer节点容器类。第二行调用nodes对象
    的Create()方法创建了2个节点。节点是ns-3对网络设备节点的一个抽象概念，
    可以简单地认为节点代表一台能够加入诸如协议栈，应用以及外设卡等的计算机。*/
    NodeContainer nodes;
    nodes.Create(2);

    /*实际中物理连接两台计算机成为网络一般来说需要使用网卡和网线，ns-3中将对应的物理实体
    抽象为网络设备和信道2个概念，下面的语句就实现了网络节点物理连接：
    PointToPointHelper类负责设置网络设备和信道属性，并通过Install方法把设备安装到节点中。*/
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRata", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    /*这两行代码会完成设备和信道的配置。第一行声明了上面提到的设备容器，第二行完成了主要工作。
    PointToPointHelper的Install()方法以一个NodeContainer对象作为一个参数。在Install()方法内，一个NetDeviceContainer被创建了。
    对于在NodeContainer对象中的每一个节点（对于一个点到点链路必须明确有2个节点），一个PointToPointNetDevice被创建和保存在设备容器内。
    一个PointToPointChannel对象被创建，2个PointToPointNetDevices与之连接。当PointToPointHelper对象创建时，
    那些在助手中就被设置的属性被用来初始化对象对应的属性值。
    当调用了pointToPoint.Install(nodes)后，会有2个节点，每一个节点安装了点到点网络设备，
    在它们之间是一个点到点信道。2个设备会被配置在一个有2ms传输时延的信道上以5Mbit/s的速率传输数据。*/
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    /*类InternetStackHelper会为每一个节点容器中的节点安装一个网络协议栈，主要是IP层。
    Ipv4AddressHelper为节点上的设备设置IP地址。提供了一个拓扑助手来管理IP地址的分配。
    当执行实际的地址分配时，唯一用户可见的API是设置基IP地址和子网掩码。*/
    InternetStackHelper stack;
    stack.Install(nodes);

    /*声明了一个地址助手对象，并且告诉它应该从10.1.1.0开始以子网掩码为255.255.255.0分配地址。
    地址分配默认是从1开始并单调增长，所以在这个基础上第一个分配的地址会是10.1.1.1，
    紧跟着的是10.1.1.2等。底层ns-3系统事实上会记住所有分配的IP地址。*/
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    /*这行代码完成了真正的地址配置。在ns-3中使用Ipv4Interface对象将一个IP地址同一个设备关联起来。
    正如有时候需要一个被助手创建的网络设备列表一样，有时候需要一个Ipv4Interface对象的列表。
    Ipv4InterfaceContainer提供了这样的功能。现在有了一个安装了协议栈，配置了IP地址类的点到点的网络。
    这时所要做的事情是运用它来产生数据通信。*/
    Ipv4InterfaceContainer interfaces = address.Assign (devices);

    /*第一行声明了UdpEchoServerHelper。像往常一样，这个并非应用本身，是一个用来帮助创建真正应用的对象。
    本书约定在助手中放置必需的属性。本例中除非我们告知助手服务器和客户端所共知的一个端口号，
    否则这个助手是不会起任何作用的。本书并没有随机选择，而是把这个端口号作为助手构造函数的一个参数。
    只要你愿意，就能够使用SetAttribute设置“Port”参数到另一个值。*/
    UdpEchoServerHelper echoServer (9);

    /*同其他助手对象类似，UdpEchoServerHelper对象有一个Install方法。实际上是这个方法的执行，
    才初始化回显服务器的应用，并将应用连接到一个节点上去。安装方法把NodeContainer当作一个参数，
    正如其他安装方法一样。这里有一个C++隐式转换，此转换以nodes.Get(1)的结果作为输入，
    并把它作为一个未命名的NodeContainer构造函数的参数，最终这个未命名的NodeContainer容器索引号为1的机节点上安装一个UdpEchoServerApplication。
    安装会返回一个容器，这个容器中包含了指向所有被助手创建的应用指针。*/
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    /*应用对象需要一个时间参数来“开始”产生数据通信并且可能在一个可选的时间点“停止”。
    我们提供了开始和停止的2个参数，时间点是用Application Container的方法Start和Stop来设置的，
    这些方法以“Time”对象为参数。在这种情况下，使用了一种明确的C++转换序列来获得C++双精度(double)的1.0，
    并且用一个Seconds转换来把它转换到ns-3的Time对象。
    
    这两行会使echo服务应用在1s时开始（生效）并在10s时停止（失效）。
    既然已经声明了一个模拟事件（就是应用的停止事件）在10s时被执行，模拟至少会持续10s。*/
    serverApps.Start(Seconds(1.0));
    serverAPPs.Stop(Seconds(10.0));

    /*客户端应用的设置与服务器端类似。也有一个UdpEchoClientHelper来管理UdpEchoClientApplication。
    然而，对于echo客户端，需要设置5个不同的属性。首先2个属性是在UdpEchoClientHelper的构建过程中被设置的。
    按照助手构造函数的格式，本文把“RemoteAddress”和“RemotePort”属性传递给了助手（实际上是作为助手构造函数的2个必须参数传递的）。

    回忆一下使用Ipv4InterfaceContainer来追踪配置给设备的IP地址。在界面容器中位置零的界面对象将会和节点容器中位置零的节点对象对应。
    同样在界面容器中位置一的界面对象将会和节点容器中位置一的节点对象对应。所以，在上面的第一行代码中，
    本文创建了一个助手并告诉它设置客户端的远端地址为服务器节点的IP地址。同样告诉它准备发送第二个数据分组到端口9。

    “MaxPackets”属性告诉客户端所允许它在模拟期间能发送的最大数据分组个数。
    “Interval”属性告诉客户端在2个数据分组之间要等待多长的时间，
    而“PacketSize”属性告诉客户端它的数据分组应该承载多少数据。本例中让客户端发送一个1024byte的数据分组。
    正如echo服务端一样，告诉echo客户端何时来开始和停止，但是这里本文使客户端在服务端生效1s后才开始（在模拟器中时间2s的时候）。*/
    UdpEchoClientHelper echoClient(interfaces.GetAddres(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop (Seconds (10.0));

    /*当Simulator::Run被调用时，系统会开始遍历预设事件的列表并执行。首先它会在1.0s时运行事件，这个事件会使echo服务端应用生效。
    接下来仿真器会运行在t=2.0s时的事件，即让echo客户端应用开始。同样地，这个事件可能会预定更多的其他事件。
    在echo客户端应用中，开始事件的执行会通过给服务端传送一个数据分组来开始仿真的数据传送阶段。
    发送一个数据分组给服务端会引发一系列更多的事件。这些事件会被预设在此事件之后，并根据已经在脚本中设定的时间参数来执行数据分组的应答。

    我们只发送了一个数据分组，在此之后，那个被单独的客户端应答请求所引发的连锁反应会停止，并且模拟器会进入空闲状态。
    当这发生时，接下来的事件就是服务端和客户端的Stop事件。当这些事件被执行后，就没有将来的事件来执行了，函数Simulator::Run会返回。
    整个模拟过程结束。*/
    Simulator::Run ();
    /*下面剩下的事情就是清理。清理通过调用全局函数来完成，助手函数被执行后，
    助手安排的钩子函数就被插入到模拟器中来销毁所有被创建的对象，ns-3系统会帮你料理繁杂的任务。*/
    Simulator::Destroy ();
    return 0;
}
