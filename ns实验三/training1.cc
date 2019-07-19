/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/mmwave-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store.h"
#include "ns3/mmwave-helper.h"
#include "ns3/log.h"
#include "ns3/mmwave-point-to-point-epc-helper.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/eps-bearer-tag.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/test.h"
#include "ns3/abort.h"
#include "ns3/object.h"

using namespace ns3;

int main(int argc, char **argv)
{
    // LogComponentEnable("MmWaveMacSchedulerUeInfoPF",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MmWaveMacSchedulerTdmaPF",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MmWaveMacSchedulerTdmaRR",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MmWaveMacSchedulerTdmaMR",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MmWaveMacSchedulerNs3",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("LteRlcAm",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("LteRlcUm",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("LteEnbRrc",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MmWaveEnbMac",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MmWaveEnbNetDevice",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MmWaveEnbPhy",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MmWavePhy",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MmWave3gppChannel",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MmWaveMacSchedulerUeInfoPF",LOG_LEVEL_INFO);


    // LogComponentEnableAll(LOG_LEVEL_FUNCTION);

    double Velocity = 40;
    uint32_t numerology = 5; // the numerology to be used
    uint32_t rb = 60;
    double bw1 = 347.83e6;      // bandwidth of bandwidth part 1
    bool isDownlink = 1;     // whether to generate the downlink traffic
    uint32_t usersNum = 12;   // number of users
    std::string schedulerType[] = {"ns3::MmWaveMacSchedulerTdmaRR", "ns3::MmWaveMacSchedulerTdmaPF", "ns3::MmWaveMacSchedulerTdmaMR", "ns3::MmWaveMacSchedulerOfdmaRR", "ns3::MmWaveMacSchedulerOfdmaPF", "ns3::MmWaveMacSchedulerOfdmaMR"};
    //这个就是算法类型
    // set simulation time and mobility
    double simTime = 0.2;
    uint16_t gNbNum = 1;
    uint32_t packetSize = 1000;
    uint32_t choice = 4;//这个是算法类型索引，用来确定用什么算法
    std::string dtrt = "100Mbps";
    int distAllocer = 1;t
    double delta = 0.2;
    double longlength = 2;
    double distance = 400;
    double fi = 1;
    double power = 20.0;
    std::string scenario="UMa";
    bool Shadowing = false;
    bool beamsearch = false;

    CommandLine cmd;
    cmd.AddValue("simTime", "simTime in [s]", simTime);
    cmd.AddValue("numerology", "the numerology to be used", numerology);
    cmd.AddValue("isDownlink", "whether to generate the downlink traffic", isDownlink);
    cmd.AddValue("choice", "mac scheduler choice", choice);
    cmd.AddValue("userNumber", "number of users pre beam", usersNum);
    cmd.AddValue("datarate", "datarate", dtrt);
    cmd.AddValue("scenario", "scenario", scenario);
    cmd.AddValue("fi", "fi", fi);
    cmd.AddValue("power", "power", power);
    cmd.AddValue("rb", "rb", rb);
    cmd.AddValue("Shadowing", "Shadowing", Shadowing);
    cmd.AddValue("beamsearch", "beamsearch", beamsearch);


    cmd.AddValue("distance", "Distance between eNBs [m]", distance);
    cmd.AddValue("distAllocer", "distAllocer", distAllocer);
    cmd.AddValue("delta", "delta", delta);
    cmd.AddValue("long", "longlength", longlength);
    cmd.Parse(argc, argv);


    bw1=400e6/69*rb+1e-6;

    DataRate udpRate = DataRate(dtrt);
    Time udpAppStartTimeDl = MilliSeconds(100);
    Time udpAppStartTimeUl = MilliSeconds(100);
    Time udpAppStopTimeDl = Seconds(simTime);
    Time udpAppStopTimeUl = Seconds(simTime);
//////////////////////////////////////////////////////////////////////////////////////////////Config
    Config::SetDefault("ns3::MmWave3gppPropagationLossModel::ChannelCondition", StringValue("l"));//信道状态：l即LOS
    Config::SetDefault("ns3::MmWave3gppPropagationLossModel::Scenario", StringValue(scenario)); // with antenna height of 10 m  UMi-StreetCanyon，场景：可配置
    Config::SetDefault("ns3::MmWave3gppPropagationLossModel::Shadowing", BooleanValue(Shadowing));//阴影衰落：可开关
    Config::SetDefault("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue(999999999));//RLC缓存大小：设为无限
    Config::SetDefault("ns3::MmWave3gppChannel::CellScan", BooleanValue(beamsearch));//波束扫描开关:可配置
    Config::SetDefault("ns3::MmWave3gppChannel::BeamSearchAngleStep", DoubleValue(10.0));//扫描步长:可配置
    // Config::SetDefault("ns3::MmWave3gppChannel::Blockage", BooleanValue(true));        // use blockage or not
    // Config::SetDefault("ns3::MmWave3gppChannel::PortraitMode", BooleanValue(true));    // use blockage model with UT in portrait mode
    // Config::SetDefault("ns3::MmWave3gppChannel::NumNonselfBlocking", IntegerValue(4)); // number of non-self blocking obstacles
    Config::SetDefault("ns3::MmWaveEnbPhy::TxPower", DoubleValue(power));//发射能量：可设置
    Config::SetDefault("ns3::MmWaveUePhy::TxPower", DoubleValue(power));//发射能量：可设置
    // Config::SetDefault("ns3::MmWaveMacSchedulerNs3::StartingMcsDl", UintegerValue(28));
    // Config::SetDefault("ns3::MmWaveMacSchedulerNs3::StartingMcsUl", UintegerValue(28));
    Config::SetDefault("ns3::EpsBearer::Release", UintegerValue(15));//

    Config::SetDefault("ns3::MmWaveMacSchedulerTdmaPF::FairnessIndex", DoubleValue(fi));//PF算法的FairnessIndex设为1
    Config::SetDefault("ns3::MmWaveMacSchedulerOfdmaPF::FairnessIndex", DoubleValue(fi));
    // Config::SetDefault("ns3::MmWaveUeNetDevice::AntennaNumDim1", UintegerValue(2));
    // Config::SetDefault("ns3::MmWaveUeNetDevice::AntennaNumDim2", UintegerValue(4));
    // Config::SetDefault("ns3::MmWaveEnbNetDevice::AntennaNumDim1", UintegerValue(4));
    // Config::SetDefault("ns3::MmWaveEnbNetDevice::AntennaNumDim2", UintegerValue(8));

    // Config::SetDefault("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue(640));

    // Config::SetDefault("ns3::MmWaveEnbPhy::NoiseFigure", DoubleValue(0));
    // Config::SetDefault("ns3::MmWaveUePhy::NoiseFigure", DoubleValue(0));

    Config::SetDefault("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_UM_ALWAYS));//EpsBearerToRlcMapping使用UM模式
    // Config::SetDefault("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_UM_ALWAYS));

//////////////////////////////////////////////////////////////////////////////////////////Config mmwave helper
    // setup the mmWave simulation
    //使用mmwaveHelper建立配置NR的衰落模型MmWave3gppPropagationLossModel，信道MmWave3gppChannel，波频率SetCentreFrequency，调度器类型MacSchedulerType，核心网epcHelper
    Ptr<MmWaveHelper> mmWaveHelper = CreateObject<MmWaveHelper>();
    mmWaveHelper->SetAttribute("PathlossModel", StringValue("ns3::MmWave3gppPropagationLossModel"));
    mmWaveHelper->SetAttribute("ChannelModel", StringValue("ns3::MmWave3gppChannel"));

    Ptr<BandwidthPartsPhyMacConf> bwpConf = CreateObject<BandwidthPartsPhyMacConf>();
    Ptr<MmWavePhyMacCommon> phyMacCommonBwp = CreateObject<MmWavePhyMacCommon>();
    phyMacCommonBwp->SetCentreFrequency(28e9);
    phyMacCommonBwp->SetBandwidth(bw1);
    phyMacCommonBwp->SetNumerology(numerology);
    std::cout << schedulerType[choice] << std::endl;
    phyMacCommonBwp->SetAttribute("MacSchedulerType", TypeIdValue(TypeId::LookupByName(schedulerType[choice])));

    bwpConf->AddBandwidthPartPhyMacConf(phyMacCommonBwp);
    mmWaveHelper->SetBandwidthPartMap(bwpConf);

    Ptr<MmWavePointToPointEpcHelper> epcHelper = CreateObject<MmWavePointToPointEpcHelper>();
    mmWaveHelper->SetEpcHelper(epcHelper);
    mmWaveHelper->Initialize();

    // create base stations and mobile terminals
    NodeContainer gNbNodes;
    NodeContainer ueNodes;
    MobilityHelper mobility;
    gNbNodes.Create(gNbNum);
    ueNodes.Create(usersNum * gNbNum);
    ueNodes.Create(2);//创建2个运动node
////////////////////////////////////////////////////////////////////////////////////////////Location allocation
    double gNbHeight = 25;
    double ueHeight = 1.5;
    if (scenario=="RMa")
    {
        gNbHeight = 35;
        ueHeight = 1.5;
    }
    else if (scenario=="UMa")
    {
        gNbHeight = 25;
        ueHeight = 1.5;
        if(distance<35){
            puts("Distance too small");
        }
    }
    else if (scenario=="UMi-StreetCanyon")
    {
        gNbHeight = 10;
        ueHeight = 1.5;
    }
    else if (scenario=="InH-OfficeMixed" || scenario=="InH-OfficeOpen" || scenario=="InH-ShoppingMall")
    {
        //The fast fading model does not support 'InH-ShoppingMall' since it is listed in table 7.5-6
        gNbHeight = 3;
        ueHeight = 1;
    }
    else
    {
        return 1;
    }
    Ptr<ListPositionAllocator> apPositionAlloc = CreateObject<ListPositionAllocator>();
    Ptr<ListPositionAllocator> staPositionAlloc = CreateObject<ListPositionAllocator>();

    double gNbx = 0;
    double gNby = 0;

    for (uint gNb = 0; gNb < gNbNum; gNb++)
    {
        apPositionAlloc->Add(Vector(gNbx, gNby, gNbHeight));

        for (uint ueBeam = 0; ueBeam < usersNum; ueBeam++)
        {
            double the = 1.0 * ueBeam / usersNum * 2 * 3.1416;
            if (distAllocer == 1)//圆上UE均匀分配，distance为半径
            {
                staPositionAlloc->Add(Vector(gNbx + distance * cos(the), gNby + distance * sin(the), ueHeight));
            }
            else if (distAllocer == 2)//UE椭圆分布，longlength取2，长轴为短轴2倍长
            {
                staPositionAlloc->Add(Vector(gNbx + distance * longlength * cos(the), gNby + distance * sin(the), ueHeight));
            }
            else if (distAllocer == 3)
            {
                staPositionAlloc->Add(Vector(gNbx + distance, gNby + 0, ueHeight));
            }
            else if (distAllocer == 4)
            {
                staPositionAlloc->Add(Vector(gNbx + distance + ueBeam * delta, gNby + 0, ueHeight));
            }
            else if (distAllocer == 5)
            {
                staPositionAlloc->Add(Vector(distance, 0, ueHeight));
                staPositionAlloc->Add(Vector(distance * 2, 0, ueHeight));
                staPositionAlloc->Add(Vector(distance * 3, 0, ueHeight));
            }
            else if (distAllocer == 6)
            {
                switch (ueBeam / (usersNum / 3))
                {
                case 0:
                    staPositionAlloc->Add(Vector(gNbx + distance * cos(the), gNby + distance * sin(the), ueHeight));
                    break;
                case 1:
                    staPositionAlloc->Add(Vector(gNbx + 3 * distance * cos(the), gNby + 3 * distance * sin(the), ueHeight));
                    break;
                case 2:
                    staPositionAlloc->Add(Vector(gNbx + 5 * distance * cos(the), gNby + 5 * distance * sin(the), ueHeight));
                    break;

                default:
                    staPositionAlloc->Add(Vector(gNbx + distance * cos(the), gNby + distance * sin(the), ueHeight));
                    break;
                };
            }
            else
            {
                staPositionAlloc->Add(Vector(0, 0, 2));
            }
        }
        gNbx += 1000;
        // gNby += 1;
    }
    
    //添加2个运动节点
    staPositionAlloc->Add(Vector(gNbx - distance * 0.25, gNby - Velocity*simTime*0.5, ueHeight));
    staPositionAlloc->Add(Vector(gNbx + distance * 0.25, gNby - Velocity*simTime*0.5, ueHeight));
    //设置移动模型，ConstantPositionMobilityModel（静止模型），此外有ConstantVelocityMobilityModel（恒定速率运动模型），RandomWalk2dMobilityModel（随机行走模型）等模型
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(apPositionAlloc);
    mobility.Install(gNbNodes);

    //mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.SetPositionAllocator(staPositionAlloc);
    uint g;
    for(g = 0; g < usersNum; g++)
    {
        mobility.Install(ueNodes.Get(g));
    }
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(ueNodes.Get(g));
    mobility.Install(ueNodes.Get(g+1));

    Vector sp1(0, Velocity, 0);
    ueNodes.Get(g)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(sp1);
    ueNodes.Get(g+1)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(sp1);
    /* 
    Vector sp1(-Velocity, 0, 0);
    ueNodes.Get(0)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(sp1);

    Vector sp2(0, -Velocity, 0);
    ueNodes.Get(1)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(sp2);

    Vector sp3(Velocity, 0, 0);
    ueNodes.Get(2)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(sp3);

    Vector sp4(0, Velocity, 0);
    ueNodes.Get(3)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(sp4);
    */
    ////////////////////////////////////////////////////////////////////////////////////////////Build network
    // install mmWave net devices
    NetDeviceContainer enbNetDev = mmWaveHelper->InstallEnbDevice(gNbNodes);
    NetDeviceContainer ueNetDev = mmWaveHelper->InstallUeDevice(ueNodes);
    // create the internet and install the IP stack on the UEs
    // get SGW/PGW and create a single RemoteHost
    Ptr<Node> pgw = epcHelper->GetPgwNode();
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);
    // connect a remoteHost to pgw. Setup routing too
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.000)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    // in this container, interface 0 is the pgw, 1 is the remoteHost

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);
    internet.Install(ueNodes);
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNetDev));

    // Set the default gateway for the UEs
    for (uint32_t j = 0; j < ueNodes.GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(ueNodes.Get(j)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    // attach UEs to the closest eNB
    mmWaveHelper->AttachToClosestEnb(ueNetDev, enbNetDev);
//////////////////////////////////////////////////////////////////////////////////////////////////Config UDP client and server
    // assign IP address to UEs, and install UDP downlink applications
    uint16_t dlPort = 1234;
    ApplicationContainer clientAppsDl;
    ApplicationContainer serverAppsDl;
    ObjectMapValue objectMapValue;

    Time udpInterval = Time::FromDouble((packetSize * 8) / static_cast<double>(udpRate.GetBitRate()), Time::S);

    if (isDownlink)
    {
        UdpServerHelper dlPacketSinkHelper(dlPort);
        serverAppsDl.Add(dlPacketSinkHelper.Install(ueNodes));

        // configure here UDP traffic
        for (uint32_t j = 0; j < ueNodes.GetN(); ++j)
        {
            UdpClientHelper dlClient(ueIpIface.GetAddress(j), dlPort);
            dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
            dlClient.SetAttribute("PacketSize", UintegerValue(packetSize));
            dlClient.SetAttribute("Interval", TimeValue(udpInterval)); 
            clientAppsDl.Add(dlClient.Install(remoteHost));
        }
        // start UDP server and client apps
        serverAppsDl.Start(udpAppStartTimeDl);
        clientAppsDl.Start(udpAppStartTimeDl);
        serverAppsDl.Stop(udpAppStopTimeDl);
        clientAppsDl.Stop(udpAppStopTimeDl);
    }
/////////////////////////////////////////////////////////////////////////////////////Flow monitor
    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add (remoteHost);
    endpointNodes.Add (ueNodes);

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install (endpointNodes);
    monitor->SetAttribute ("DelayBinWidth", DoubleValue (0.001));
    monitor->SetAttribute ("JitterBinWidth", DoubleValue (0.001));
    monitor->SetAttribute ("PacketSizeBinWidth", DoubleValue (20));
////////////////////////////////////////////////////////////////////////////////////Run simulation
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();
///////////////////////////////////////////////////////////////////////////////////Simulation result
    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      std::stringstream protoStream;
      protoStream << (uint16_t) t.protocol;
      if (t.protocol == 6)
        {
          protoStream.str ("TCP");
        }
      if (t.protocol == 17)
        {
          protoStream.str ("UDP");
        }
      std::cout << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> " << t.destinationAddress << ":" << t.destinationPort << ") proto " << protoStream.str () << "\n";
      std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
      std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
      std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / (simTime - 0.1) / 1024 / 1024  << " Mbps\n";
      std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      if (i->second.rxPackets > 0)
        {
          // Measure the duration of the flow from receiver's perspective
          //double rxDuration = i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ();
          double rxDuration = (simTime - 0.1);

          averageFlowThroughput += i->second.rxBytes * 8.0 / rxDuration / 1024 / 1024;
          averageFlowDelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets;

          std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / rxDuration / 1024 / 1024  << " Mbps\n";
          std::cout << "  Mean delay:  " << 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets << " ms\n";
          //std::cout << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 << " Mbps \n";
          std::cout << "  Mean jitter:  " << 1000 * i->second.jitterSum.GetSeconds () / i->second.rxPackets  << " ms\n";
        }
      else
        {
          std::cout << "  Throughput:  0 Mbps\n";
          std::cout << "  Mean delay:  0 ms\n";
          std::cout << "  Mean jitter: 0 ms\n";
        }
      std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
    }
////////////////////////////////////////////////////////////////////////////Clear
    Simulator::Destroy();
    return 0;
}
