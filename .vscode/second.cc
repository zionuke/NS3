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
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");

int
main(int argc, char* argv[])
{
    //定义变量，用于决定是否开启2个UdpApplication的Logging组建组件;默认true开启  
    bool verbose = true;
    uint32_t nCsma = 3;//LAN中另有3个node

    CommandLine cmd;//命令行
    cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    //命令行参数设置是否开启logging

    cmd.Parse(argc, argv);

    if(verbose)
    {
        LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    nCsma = nCsma == 0 ? 1 : nCsma;
    /**********网络拓扑部分***********/
    //创建使用P2P链路链接的2个node
    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    //创建另一个NodeContainer类对象，用于总线(CSMA)网络
    NodeContainer csmaNodes;
    /*将之前P2P的NodeContainer的第二个节点(索引1)添加到CSMA的NodeContainer，
    以获得CSMA device,这个node将会有2个device*/
    csmaNodes.Add(p2pNodes.Get(1));
    csmaNodes.Create(nCsma);//再创建Bus network上另外的node
    
    //设置传送速率和信道延迟，同first.cc
    PointToPointHelper pointToPoint;//注意使用Helper固定格式
    //1/helper对象声明及属性设置
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue ("2ms"));

    //2/devices对象声明及接收helper对象安装方法的返回列表，安装方法的参数为节点对象
    //安装P2P网卡设备到P2P网络节点，同first.cc
    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    //类似P2PHelper，CsmaHelper帮助创建和连接CSMA设备及信道
    CsmaHelper csma;
    //数据率由Channel属性指定，而非Device属性;
    //因为CSMA不允许同一信道上有多个不同数据率的设备
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install (csmaNodes);

    //安装网络协议
    InternetStackHelper stack;
    stack.Install (p2pNodes.Get (0));//P2P链路中的第一个节点
    stack.Install (csmaNodes);

    //P2P链路中的第二个节点包含在csmaNodes中
    Ipv4AddressHelper address;//2个网段的IP地址类对象
    address.SetBase ("10.1.1.0", "255.255.255.0");//安装P2P网段的地址
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign (p2pDevices);

    address.SetBase ("10.1.2.0", "255.255.255.0");//安装CSMA网段地址
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign (csmaDevices);
    /***********网络拓扑部分结束************/

    /**********应用程序部分**************/
    UdpEchoServerHelper echoServer (9);

    //将Server服务器安装在CSMA网段的最后一个节点上，nCsma是可变的，所以不能用3
    ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (10.0));

    UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

    //将客户端安装在P2P网段的第一个节点上
    ApplicationContainer clientApps = echoClient.Install (p2pNodes.Get (0));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));
    /**********应用程序部分结束**************/

    /***********调用全局路由Helper帮助建立网络路由**************/
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    /*********开启pcap跟踪*************/
    //pointToPoint.EnablePcapAll("second");
    //开启P2PHelper类对象的pcap;"second"为保存文件的前缀名
    //前缀后的节点号是"全局节点号"，不用担心名称相同！
    //csma.EnablePcap("second", csmaDevices.Get(1), true);
    //开启csmaHelper类对象的pcap
    //使用CSMA网段索引1的设备(第二个)进行sniff,True开启Promiscuous mode
    //NodeContainer类对象的Get方法用于获得容器中给定索引下的节点，返回指向请求节点的指针！
    //Node类对象的GetId返回节点的全局ID(即节点列表中的索引号)
    //注意之前使用的Get是NetDevice类的方法，以下使用的是Node类的方法！
    //Netdevice不用取得ID,可以直接使用(已验证)；但Node需要进一步查找ID!!(已验证，不使用GetId无法通过)
    //所以后边的两句和这样的两句是等效的(已验证)
    //"csma.EnablePcap("second", csmaDevices.Get(nCsma), 0);" 
    //"csma.EnablePcap("second", csmaDevices.Get(nCsma-1), 0);" 
    pointToPoint.EnablePcapAll ("second", p2pNodes.Get(0)->GetId(), 0);
    //最后一项为explicitFilename,默认false,不加也可；若为true,将prefix作为文件名
    //倒数第二项promiscuous,默认false,此处仅想跟踪一个设备，此处仅像跟踪一个设备，故设为0(false);当有一个节点和设备的promiscuous模式为true时，
    //CSMA网段其他节点便不再产生trace文件。)
    csma.EnablePcap ("second", csmaNodes.Get (nCsma)->GetId(), 0, false);
    csma.EnablePcap ("second", csmaNodes.Get (nCsma-1)->GetId(), 0, false);

    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
