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

// static void
// CourseChange (std::string foo, Ptr<const MobilityModel> mobility)
// {
//   Vector pos = mobility->GetPosition ();
//   Vector vel = mobility->GetVelocity ();
//   std::cout << Simulator::Now ().GetSeconds() << ", model=" << mobility << ", POS: x=" << pos.x << ", y=" << pos.y
//             << ", z=" << pos.z << "; VEL: x=" << vel.x << ", y=" << vel.y
//             << ", z=" << vel.z << std::endl;
// }

int main(int argc, char **argv)
{
    double Velocity = 20;
    uint32_t numerology = 5; // the numerology to be used
    double bw1 = 347.83e6;      // bandwidth of bandwidth part 1  347.83
    bool isDownlink = 1;     // whether to generate the downlink traffic
    bool isUplink = 0;       // whether to generate the uplink traffic
    uint32_t usersNum = 4;   // number of users
    std::string schedulerType[] = {"ns3::MmWaveMacSchedulerTdmaRR", "ns3::MmWaveMacSchedulerTdmaPF", "ns3::MmWaveMacSchedulerTdmaMR", "ns3::MmWaveMacSchedulerOfdmaRR", "ns3::MmWaveMacSchedulerOfdmaPF", "ns3::MmWaveMacSchedulerOfdmaMR"};

    double simTime = 120;
    uint16_t gNbNum = 1;
    uint32_t packetSize = 1200;
    uint32_t choice = 4;
    std::string dtrt = "50Kbps";
    int distAllocer = 1;
    double delta = 0.2;
    double longlength = 2;
    double distance = 400;
    double fi = 1;
    double power = 20.0;
    std::string updateChannel = "1s";
    std::string scenario = "UMa";
    bool Shadowing = true;
    bool CellScan = true;
    CommandLine cmd;
    cmd.AddValue("numerology", "the numerology to be used", numerology);
    cmd.AddValue("bw1", "bw1", bw1);
    cmd.AddValue("isDownlink", "whether to generate the downlink traffic", isDownlink);
    cmd.AddValue("isUplink", "whether to generate the uplink traffic", isUplink);
    cmd.AddValue("userNumber", "number of users pre beam", usersNum);

    cmd.AddValue("simTime", "simTime in [s]", simTime);
    cmd.AddValue("choice", "mac scheduler choice", choice);
    cmd.AddValue("datarate", "datarate", dtrt);
    cmd.AddValue("fi", "fi", fi);
    cmd.AddValue("power", "power", power);
    cmd.AddValue("update", "update channel", updateChannel);
    cmd.AddValue("scenario", "scenario", scenario);
    cmd.AddValue("sha", "Shadowing", Shadowing);
    cmd.AddValue("cellScan", "CellScan", CellScan);

    cmd.AddValue("distance", "Distance between eNBs [m]", distance);
    cmd.AddValue("distAllocer", "distAllocer", distAllocer);
    cmd.AddValue("delta", "delta", delta);
    cmd.AddValue("long", "longlength", longlength);
    cmd.AddValue("speed", "speed", Velocity);

    cmd.Parse(argc, argv);

    DataRate udpRate = DataRate(dtrt);
    Time udpAppStartTimeDl = MilliSeconds(100);
    Time udpAppStartTimeUl = MilliSeconds(100);
    Time udpAppStopTimeDl = Seconds(simTime);
    Time udpAppStopTimeUl = Seconds(simTime);

    Config::SetDefault("ns3::MmWave3gppChannel::UpdatePeriod", StringValue(updateChannel));
    Config::SetDefault("ns3::MmWave3gppPropagationLossModel::ChannelCondition", StringValue("l"));
    Config::SetDefault("ns3::MmWave3gppPropagationLossModel::Scenario", StringValue(scenario)); // with antenna height of 10 m  UMi-StreetCanyon
    Config::SetDefault("ns3::MmWave3gppPropagationLossModel::Shadowing", BooleanValue(Shadowing));
    Config::SetDefault("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue(999999999));
    Config::SetDefault("ns3::MmWave3gppChannel::CellScan", BooleanValue(true));
    Config::SetDefault("ns3::MmWave3gppChannel::BeamSearchAngleStep", DoubleValue(10.0));
    // Config::SetDefault("ns3::MmWave3gppChannel::Blockage", BooleanValue(true));        // use blockage or not
    // Config::SetDefault("ns3::MmWave3gppChannel::PortraitMode", BooleanValue(true));    // use blockage model with UT in portrait mode
    // Config::SetDefault("ns3::MmWave3gppChannel::NumNonselfBlocking", IntegerValue(4)); // number of non-self blocking obstacles
    Config::SetDefault("ns3::MmWaveEnbPhy::TxPower", DoubleValue(power));
    Config::SetDefault("ns3::MmWaveUePhy::TxPower", DoubleValue(power));
    // Config::SetDefault("ns3::MmWaveMacSchedulerNs3::StartingMcsDl", UintegerValue(28));
    // Config::SetDefault("ns3::MmWaveMacSchedulerNs3::StartingMcsUl", UintegerValue(28));
    Config::SetDefault("ns3::EpsBearer::Release", UintegerValue(15));

    Config::SetDefault("ns3::MmWaveMacSchedulerTdmaPF::FairnessIndex", DoubleValue(fi));
    Config::SetDefault("ns3::MmWaveMacSchedulerOfdmaPF::FairnessIndex", DoubleValue(fi));
    // Config::SetDefault("ns3::MmWaveUeNetDevice::AntennaNumDim1", UintegerValue(2));
    // Config::SetDefault("ns3::MmWaveUeNetDevice::AntennaNumDim2", UintegerValue(4));
    // Config::SetDefault("ns3::MmWaveEnbNetDevice::AntennaNumDim1", UintegerValue(4));
    // Config::SetDefault("ns3::MmWaveEnbNetDevice::AntennaNumDim2", UintegerValue(8));

    // Config::SetDefault("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue(640));

    // Config::SetDefault("ns3::MmWaveEnbPhy::NoiseFigure", DoubleValue(0));
    // Config::SetDefault("ns3::MmWaveUePhy::NoiseFigure", DoubleValue(0));

    Config::SetDefault("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_UM_ALWAYS));
    Config::SetDefault("ns3::MmWavePointToPointEpcHelper::S1uLinkDelay", TimeValue(MilliSeconds(0)));
    Config::SetDefault("ns3::MmWavePointToPointEpcHelper::X2LinkDelay", TimeValue(MilliSeconds(0)));

    Config::SetDefault("ns3::BwpManagerAlgorithmStatic::NGBR_LOW_LAT_EMBB", UintegerValue(0));
    Config::SetDefault("ns3::BwpManagerAlgorithmStatic::GBR_CONV_VOICE", UintegerValue(1));

    Config::SetDefault("ns3::MmWaveHelper::EnbComponentCarrierManager", StringValue("ns3::BwpManagerGnb"));

    // setup the mmWave simulation
    Ptr<MmWaveHelper> mmWaveHelper = CreateObject<MmWaveHelper>(); //SetHarqEnabled

    mmWaveHelper->SetAttribute("PathlossModel", StringValue("ns3::MmWave3gppPropagationLossModel"));
    mmWaveHelper->SetAttribute("ChannelModel", StringValue("ns3::MmWave3gppChannel"));

    Ptr<BandwidthPartsPhyMacConf> bwpConf = CreateObject<BandwidthPartsPhyMacConf>();
    Ptr<MmWavePhyMacCommon> phyMacCommonBwp = CreateObject<MmWavePhyMacCommon>();
    phyMacCommonBwp->SetCentreFrequency(28e9);
    phyMacCommonBwp->SetBandwidth(bw1);
    phyMacCommonBwp->SetNumerology(numerology);
    std::cout << schedulerType[choice] << std::endl;
    phyMacCommonBwp->SetAttribute("MacSchedulerType", TypeIdValue(TypeId::LookupByName(schedulerType[choice])));
    phyMacCommonBwp->SetCcId(0);

    bwpConf->AddBandwidthPartPhyMacConf(phyMacCommonBwp);
    mmWaveHelper->SetBandwidthPartMap(bwpConf);

    Ptr<MmWavePointToPointEpcHelper> epcHelper = CreateObject<MmWavePointToPointEpcHelper>();
    mmWaveHelper->SetEpcHelper(epcHelper);
    mmWaveHelper->Initialize();

    // create base stations and mobile terminals
    NodeContainer gNbNodes;
    NodeContainer ueNodes;
    MobilityHelper mobility;

    double gNbHeight = 25;
    double ueHeight = 1.5;
    if (scenario == "RMa")
    {
        gNbHeight = 35;
        ueHeight = 1.5;
    }
    else if (scenario == "UMa")
    {
        gNbHeight = 25;
        ueHeight = 1.5;
        if (distance < 35)
        {
            puts("Distance too small");
        }
    }
    else if (scenario == "UMi-StreetCanyon")
    {
        gNbHeight = 10;
        ueHeight = 1.5;
    }
    else if (scenario == "InH-OfficeMixed" || scenario == "InH-OfficeOpen" || scenario == "InH-ShoppingMall")
    {
        //The fast fading model does not support 'InH-ShoppingMall' since it is listed in table 7.5-6
        gNbHeight = 3;
        ueHeight = 1;
    }
    else
    {
        return 1;
    }
    gNbNodes.Create(gNbNum);
    ueNodes.Create(usersNum * gNbNum);

    Ptr<ListPositionAllocator> apPositionAlloc = CreateObject<ListPositionAllocator>();
    Ptr<ListPositionAllocator> staPositionAlloc = CreateObject<ListPositionAllocator>();

    double gNbx = 0;
    double gNby = 0;
    apPositionAlloc->Add(Vector(gNbx, gNby, gNbHeight));
    staPositionAlloc->Add(Vector(gNbx - 1000, gNby - distance, ueHeight));
    staPositionAlloc->Add(Vector(gNbx - 10, gNby + distance, ueHeight));
    staPositionAlloc->Add(Vector(gNbx, gNby + distance, ueHeight));
    staPositionAlloc->Add(Vector(gNbx + 10, gNby + distance, ueHeight));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(apPositionAlloc);
    mobility.Install(gNbNodes);

    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    // char boundstr[64];
    // sprintf(boundstr,"-%.3lf|%.3lf|-%.3lf|%.3lf",distance*3,distance*3,distance*3,distance*3);
    // std::cout<<boundstr<<std::endl;
    // mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
    //                          "Mode", StringValue ("Time"),
    //                          "Time", StringValue ("10s"),
    //                          "Speed", StringValue ("ns3::UniformRandomVariable[Min=5.0|Max=15.0]"),
    //                          "Bounds", StringValue (boundstr));
    mobility.SetPositionAllocator(staPositionAlloc);
    mobility.Install(ueNodes);
    (void)(Velocity);

    Vector sp(Velocity, 0, 0);
    ueNodes.Get(0)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(sp);

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
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);

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
    // std::cout<<mmWaveHelper->GetHarqEnabled()<<std::endl;

    // assign IP address to UEs, and install UDP downlink applications
    uint16_t dlPort = 1234;
    uint16_t ulPort = 2000;
    ApplicationContainer clientAppsDl;
    ApplicationContainer serverAppsDl;
    ApplicationContainer clientAppsUl;
    ApplicationContainer serverAppsUl;
    ObjectMapValue objectMapValue;

    Time udpInterval = Time::FromDouble((packetSize * 8) / static_cast<double>(udpRate.GetBitRate()), Time::S);

    if (isUplink)
    {
        // configure here UDP traffic
        for (uint32_t j = 0; j < ueNodes.GetN(); ++j)
        {
            UdpServerHelper ulPacketSinkHelper(ulPort);
            serverAppsUl.Add(ulPacketSinkHelper.Install(remoteHost));

            UdpClientHelper ulClient(remoteHostAddr, ulPort);
            ulClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
            ulClient.SetAttribute("PacketSize", UintegerValue(packetSize));
            ulClient.SetAttribute("Interval", TimeValue(udpInterval)); // we try to saturate, we just need to measure during a short time, how much traffic can handle each BWP
            clientAppsUl.Add(ulClient.Install(ueNodes.Get(j)));
            ulPort++;
        }

        serverAppsUl.Start(udpAppStartTimeUl);
        clientAppsUl.Start(udpAppStartTimeUl);
        serverAppsUl.Stop(udpAppStopTimeUl);
        clientAppsUl.Stop(udpAppStopTimeUl);
    }

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
            dlClient.SetAttribute("Interval", TimeValue(udpInterval)); // we try to saturate, we just need to measure during a short time, how much traffic can handle each BWP
            clientAppsDl.Add(dlClient.Install(remoteHost));

            Ptr<EpcTft> tft = Create<EpcTft>();
            EpcTft::PacketFilter dlpf;
            dlpf.localPortStart = dlPort;
            dlpf.localPortEnd = dlPort;
            dlPort++;
            tft->Add(dlpf);

            enum EpsBearer::Qci q;
            // q = EpsBearer::GBR_CONV_VOICE;

            q = EpsBearer::NGBR_VIDEO_TCP_DEFAULT;

            EpsBearer bearer(q);
            mmWaveHelper->ActivateDedicatedEpsBearer(ueNetDev.Get(j), bearer, tft);
        }
        // start UDP server and client apps
        serverAppsDl.Start(udpAppStartTimeDl);
        clientAppsDl.Start(udpAppStartTimeDl);
        serverAppsDl.Stop(udpAppStopTimeDl);
        clientAppsDl.Stop(udpAppStopTimeDl);
    }
    // mmWaveHelper->EnableTraces();

    // FlowMonitorHelper flowmon;
    // Ptr<FlowMonitor> monitor = flowmon.Install(ueNodes);
    // flowmon.Install(gNbNodes);

    //mmWaveHelper->EnableTraces();
    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add(remoteHost);
    endpointNodes.Add(ueNodes);

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin(); i != stats.end(); ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::stringstream protoStream;
        protoStream << (uint16_t)t.protocol;
        if (t.protocol == 6)
        {
            protoStream.str("TCP");
        }
        if (t.protocol == 17)
        {
            protoStream.str("UDP");
        }
        std::cout << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> " << t.destinationAddress << ":" << t.destinationPort << ") proto " << protoStream.str() << "\n";
        std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
        std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
        std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / (simTime - 0.1) / 1000 / 1000 << " Mbps\n";
        std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            //double rxDuration = i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ();
            double rxDuration = (simTime - 0.1);

            averageFlowThroughput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
            averageFlowDelay += 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;

            std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000 << " Mbps\n";
            std::cout << "  Mean delay:  " << 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets << " ms\n";
            //std::cout << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 << " Mbps \n";
            std::cout << "  Mean jitter:  " << 1000 * i->second.jitterSum.GetSeconds() / i->second.rxPackets << " ms\n";
        }
        else
        {
            std::cout << "  Throughput:  0 Mbps\n";
            std::cout << "  Mean delay:  0 ms\n";
            std::cout << "  Mean jitter: 0 ms\n";
        }
        std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
    }
    Simulator::Destroy();
    return 0;
}
