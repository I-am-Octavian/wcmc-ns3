#include "ns3/applications-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <bits/stdc++.h>
// #include "ns3/gtk-config-store.h"

using namespace ns3;
using std::vector;

NS_LOG_COMPONENT_DEFINE("ASSIGNMENT");

uint32_t totalZeroSpeed = 0;
uint32_t totalTenSpeed = 0;
vector<uint32_t> throughputArray0;
vector<uint32_t> throughputArray1;
uint16_t numberOfUes = 10;
uint16_t numberOfEnbs = 4;
FlowMonitorHelper flowHelper0;
FlowMonitorHelper flowHelper1;

static void
PlotGraph3(Ptr<FlowMonitor> mon1, Ptr<FlowMonitor> mon2, NodeContainer* cont) {
    for (int i = 0; i < numberOfEnbs; i++) {
        for (int j = 0; j < numberOfUes;j++) {
            Ptr<MobilityModel> mobility =
                cont[i].Get(j)->GetObject<MobilityModel>();

            std::cout << "Mobility\n\n" << mobility->GetVelocity().x << " " << mobility->GetVelocity().y << std::endl;
            if (mobility->GetVelocity().x == 0 && mobility->GetVelocity().y == 0) {
                std::cout << "MON1\n";
                mon1 = flowHelper0.Install(cont[i].Get(j));
            } else {
                std::cout << "MON2\n";
                mon2 = flowHelper1.Install(cont[i].Get(j));
            }

        }
    }
}

static void
PlotGraph4(Ptr<FlowMonitor> mon, Ptr<Node> node) {
    std::ofstream os;
    os.open("graph/graph5RR.txt", std::ios::app);
    std::map<FlowId, FlowMonitor::FlowStats> stats = mon->GetFlowStats ();
    std::cout << "SIZE: " << stats.size() << std::endl;
    for (auto i = stats.begin (); i != stats.end (); ++i)
    {
        Ptr<MobilityModel> mobility =
            node->GetObject<MobilityModel>();
        os << i->second.txBytes * 8.0 / 10.0 / 1024 / 1024 << " "
            << Simulator::Now().GetSeconds()  << std::endl;

    }

    Simulator::Schedule(Seconds(1.0), &PlotGraph4, mon, node);
}


static void
CalculateThroughput(Ptr<FlowMonitor> monitor, bool flag
        ) {
    // FlowMonitorHelper flowmon;
    // Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
    std::cout << "SIZE: " << stats.size() << std::endl;
    for (auto i = stats.begin (); i != stats.end (); ++i)
    {
          // Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        if (flag) {
          throughputArray0.push_back(i->second.rxBytes);
        } else {
          throughputArray1.push_back(i->second.rxBytes);
        }
          std::cout << "Flow at " << Simulator::Now().GetSeconds()  << " " <<  i->first - 1 << "\n";
         std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
         std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
          std::cout << " Throughput: " << i->second.txBytes * 8.0 / 10.0 / 1024 / 1024  << " Mbps\n";

          // in
    }

    Simulator::Schedule(Seconds(1.0), &CalculateThroughput, monitor, flag);
}


static void
CalculateCumulativeThroughput() {
    std::ofstream os0;
    os0.open("graph/graph3speed0.txt", std::ios::app);

    std::ofstream os1;
    os1.open("graph/graph3speed1.txt", std::ios::app);
    uint32_t prev0;
    if (!throughputArray0.empty()) {
        sort(throughputArray0.begin(), throughputArray0.end());
        prev0 = throughputArray0[0];
    } else  {
        std::cout << "Array is empty" << std::endl;
    }


    uint32_t prev1;
    if (!throughputArray1.empty()) {
        sort(throughputArray1.begin(), throughputArray1.end());
        prev1 = throughputArray1[0];
    }


    std::cout << "Array size : " << throughputArray0.size() << std::endl;
    for (int i = 1; i < throughputArray0.size(); i++) {
        if (throughputArray0[i] != prev0) {
            os0 << throughputArray0[i] << " " << i << std::endl;
            std::cout << throughputArray0[i] << " " << i << std::endl;
            prev0 = throughputArray0[i];
        } else if (i == throughputArray0.size() -1) {
            os0 << throughputArray0[i] << " " << i << std::endl;
            std::cout << throughputArray0[i] << " " << i << std::endl;
        }
    }

    for (int i = 1; i < throughputArray1.size(); i++) {
        if (throughputArray1[i] != prev1) {
            os1 << throughputArray1[i] << " " << i << std::endl;
            std::cout << throughputArray1[i] << " " << i << std::endl;
            prev1 = throughputArray1[i];
        } else if (i == throughputArray0.size() -1) {
            os0 << throughputArray1[i] << " " << i << std::endl;
            std::cout << throughputArray1[i] << " " << i << std::endl;
        }
    }
    os0.close();
    os1.close();

    Simulator::Schedule(Seconds(1.0), &CalculateCumulativeThroughput);

}

// to write the velocity from callback
static void
CourseChange(std::ostream* os, std::string foo, Ptr<const MobilityModel> mobility)
{
    Vector vel = mobility->GetVelocity(); // Get velocity
    Vector pos = mobility->GetPosition();
    // Prints position and velocities
    *os << vel.x << " " << vel.y <<  " " << pos.x << " " << pos.y << std::endl;
}

static void
BytesTransferredCallback(std::string foo, Ptr<const Packet> packet) {
    static uint32_t totalBytesTransferred = 0;
    totalBytesTransferred += packet->GetSize();
    std::cout << "Bytes transferred: " << totalBytesTransferred << std::endl;
}


/**
 * UE Connection established notification.
 *
 * \param context The context.
 * \param imsi The IMSI of the connected terminal.
 * \param cellid The Cell ID.
 * \param rnti The RNTI.
 */
void
NotifyConnectionEstablishedUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " UE IMSI " << imsi
              << ": connected to CellId " << cellid << " with RNTI " << rnti << std::endl;
}

/**
 * UE Start Handover notification.
 *
 * \param context The context.
 * \param imsi The IMSI of the connected terminal.
 * \param cellid The actual Cell ID.
 * \param rnti The RNTI.
 * \param targetCellId The target Cell ID.
 */
void
NotifyHandoverStartUe(std::string context,
                      uint64_t imsi,
                      uint16_t cellid,
                      uint16_t rnti,
                      uint16_t targetCellId)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " UE IMSI " << imsi
              << ": previously connected to CellId " << cellid << " with RNTI " << rnti
              << ", doing handover to CellId " << targetCellId << std::endl;
}

/**
 * UE Handover end successful notification.
 *
 * \param context The context.
 * \param imsi The IMSI of the connected terminal.
 * \param cellid The Cell ID.
 * \param rnti The RNTI.
 */
void
NotifyHandoverEndOkUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " UE IMSI " << imsi
              << ": successful handover to CellId " << cellid << " with RNTI " << rnti << std::endl;
}

/**
 * eNB Connection established notification.
 *
 * \param context The context.
 * \param imsi The IMSI of the connected terminal.
 * \param cellid The Cell ID.
 * \param rnti The RNTI.
 */
void
NotifyConnectionEstablishedEnb(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
              << ": successful connection of UE with IMSI " << imsi << " RNTI " << rnti
              << std::endl;
}

/**
 * eNB Start Handover notification.
 *
 * \param context The context.
 * \param imsi The IMSI of the connected terminal.
 * \param cellid The actual Cell ID.
 * \param rnti The RNTI.
 * \param targetCellId The target Cell ID.
 */
void
NotifyHandoverStartEnb(std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti,
                       uint16_t targetCellId)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
              << ": start handover of UE with IMSI " << imsi << " RNTI " << rnti << " to CellId "
              << targetCellId << std::endl;
}

/**
 * eNB Handover end successful notification.
 *
 * \param context The context.
 * \param imsi The IMSI of the connected terminal.
 * \param cellid The Cell ID.
 * \param rnti The RNTI.
 */
void
NotifyHandoverEndOkEnb(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
              << ": completed handover of UE with IMSI " << imsi << " RNTI " << rnti << std::endl;
}

/**
 * Handover failure notification
 *
 * \param context The context.
 * \param imsi The IMSI of the connected terminal.
 * \param cellid The Cell ID.
 * \param rnti The RNTI.
 */
void
NotifyHandoverFailure(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
              << " IMSI " << imsi << " RNTI " << rnti << " handover failure" << std::endl;
}

/**
 * Sample simulation script for a X2-based handover.
 * It instantiates two eNodeB, attaches one UE to the 'source' eNB and
 * triggers a handover of the UE towards the 'target' eNB.
 */

class BiRandomVariable: public RandomVariableStream {
    int m_flag;
    public:
    BiRandomVariable() {
        m_flag = 0;
    }
    double GetValue() {
        if (m_flag == 0) {
            m_flag = 1;
            return 0;
        }
        else if (m_flag == 1) {
            m_flag = 0;
            return 10;
        }
        return -1;
    }

    uint32_t GetInteger() {
        return static_cast<uint32_t>(GetValue());
    }

};

int
main(int argc, char* argv[])
{
    // Command line arguments
    std::string scheduler = "MT";
	float speed = 0.f;
	unsigned int rngRun = 1;
	bool fullBufferFlag = false;

    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(5);
    CommandLine cmd(__FILE__);
    cmd.AddValue("scheduler", "Type of schedular to use, PF, RR, MT, PSS", scheduler);
	cmd.AddValue("speed", "Speed of the UE under simulator", speed);
	cmd.AddValue("rngRun", "Seed for the random simulation", rngRun);
	cmd.AddValue("fullBufferFlag", "Flag to enable or disable full Buffer",
				 fullBufferFlag);
    cmd.Parse(argc, argv);

    // seed values
    RngSeedManager::SetSeed(rngRun);

    // full bufferueNumPergNb
     double lambda = 1000;
     double bandwidth = 100e6;
     uint32_t udpPacketSize = 1000;


     if (fullBufferFlag)
     {
         double bitRate = 75000000; // 75 Mbps will saturate the NR system of 20 MHz with the
                                    // NrEesmIrT1 error model
         bitRate /= 160;    // Divide the cell capacity among UEs
         if (bandwidth > 20e6)
         {
             bitRate *= bandwidth / 20e6;
         }
         lambda = bitRate / static_cast<double>(udpPacketSize * 8);
     }

    // LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);

    // LogComponentEnable ("LteHelper", logLevel);
    // LogComponentEnable ("EpcHelper", logLevel);
    // LogComponentEnable ("EpcEnbApplication", logLevel);
    // LogComponentEnable ("EpcMmeApplication", logLevel);
    // LogComponentEnable ("EpcPgwApplication", logLevel);
    // LogComponentEnable ("EpcSgwApplication", logLevel);
    // LogComponentEnable ("EpcX2", logLevel);

    // LogComponentEnable ("LteEnbRrc", logLevel);
    // LogComponentEnable ("LteEnbNetDevice", logLevel);
    // LogComponentEnable ("LteUeRrc", logLevel);
    // LogComponentEnable ("LteUeNetDevice", logLevel);
    LogComponentEnable("UdpClient", LOG_LEVEL_ALL);
    LogComponentEnable("UdpServer", LOG_LEVEL_ALL);

    uint16_t numBearersPerUe = 1;
    Time simTime = Seconds(3);
    bool disableDl = false;
    bool disableUl = false;

    uint32_t ulResourceBlocks = 50;
    uint32_t dlResourceBlocks = 50;

    // change some default attributes so that they are reasonable for
    // this scenario, but do this before processing command line
    // arguments, so that the user is allowed to override these settings
    Config::SetDefault("ns3::UdpClient::Interval", TimeValue(MilliSeconds(10)));
    Config::SetDefault("ns3::UdpClient::MaxPackets", UintegerValue(1000000));
    Config::SetDefault("ns3::LteHelper::UseIdealRrc", BooleanValue(false));

    Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(40));
    Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (dlResourceBlocks));
    Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (ulResourceBlocks));

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    if (scheduler == "PF") {
        lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");
    } else if (scheduler == "RR") {
        lteHelper->SetSchedulerType("ns3::RrFfMacScheduler");
    } else if (scheduler == "MT") {
        lteHelper->SetSchedulerType("ns3::TdMtFfMacScheduler");
    } else if (scheduler == "PSS") {
        lteHelper->SetSchedulerType("ns3::PssFfMacScheduler");
    }
    lteHelper->SetHandoverAlgorithmType("ns3::NoOpHandoverAlgorithm"); // disable automatic handover

    Ptr<Node> pgw = epcHelper->GetPgwNode();

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // Create the Internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(1500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.010)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);

    // Routing of the Internet Host (towards the LTE network)
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    // interface 0 is localhost, 1 is the p2p device
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    NodeContainer ueNodes[numberOfUes];
    NodeContainer enbNodes;
    enbNodes.Create(numberOfEnbs);

    for (int i = 0; i < numberOfUes; i++) {
        ueNodes[i].Create(numberOfUes);
    }

    // Install Mobility Model
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(5000.0, 0.0, 0.0));
    positionAlloc->Add(Vector(5000.0, 5000.0, 0.0));
    positionAlloc->Add(Vector(0.0, 5000.0, 0.0));

    // setting the position of ENBs
    MobilityHelper enbMobility;
    enbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    enbMobility.SetPositionAllocator(positionAlloc);
    enbMobility.Install(enbNodes);

    // enbMobility.Install(ueNodes);

    MobilityHelper ueMobility;
    Ptr<UniformRandomVariable> ranVar = CreateObject<UniformRandomVariable>();
    ranVar->SetAttribute("Min", DoubleValue(0));
    ranVar->SetAttribute("Max", DoubleValue(500));

    Ptr<BiRandomVariable> bi = CreateObject<BiRandomVariable>();

    ueMobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-1000, 6000, -1000, 6000)), // Setting bounds
                             "Speed", PointerValue (bi),
                             "Mode", StringValue("Time"),
                             // "Distance", DoubleValue (100), // The distance to travel before changing direction
                             "Time", TimeValue (Seconds (10.0))); // The time to travel before changing direction


    uint32_t counter = 0;
    for (int i = 0; i < numberOfEnbs; i++) {

        ueMobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                    "X", DoubleValue((counter & 1)? 0 : 5000),
                                    "Y", DoubleValue(((counter >> 1) & 1)? 0 : 5000),
                                    "Rho", PointerValue(ranVar));
        ueMobility.Install(ueNodes[i]);
        counter++;
    }

    // Install LTE Devices in eNB and UEs
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueLteDevs[numberOfEnbs];

    for (int i = 0; i < numberOfEnbs; i++) {
        ueLteDevs[i] = lteHelper->InstallUeDevice(ueNodes[i]);
        internet.Install(ueNodes[i]);
    }

    // Install the IP stack on the UEs
    Ipv4InterfaceContainer ueIpIfaces[numberOfEnbs];

    for (int i = 0; i < numberOfEnbs; i++) {
        ueIpIfaces[i] = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs[i]));
    }

    // Attach all UEs to the first eNodeB
    for (int i = 0; i < numberOfEnbs; i++) {
        for (uint16_t j = 0; j < numberOfUes; j++)
        {
            lteHelper->Attach(ueLteDevs[i].Get(j), enbLteDevs.Get(i));
        }
    }


    NS_LOG_LOGIC("setting up applications");

    // Install and start applications on UEs and remote host
    uint16_t dlPort = 10000;
    uint16_t ulPort = 20000;

    // randomize a bit start times to avoid simulation artifacts
    // (e.g., buffer overflows due to packet transmissions happening
    // exactly at the same time)
    Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable>();
    startTimeSeconds->SetAttribute("Min", DoubleValue(0.05));
    startTimeSeconds->SetAttribute("Max", DoubleValue(0.06));
    Time interPacketInterval = MilliSeconds(1);

    AsciiTraceHelper asciiTraceHelper;
    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("trace-file-name.tr");

    for (int i = 0; i < numberOfEnbs; i++) {
        for (uint32_t u = 0; u < numberOfUes; ++u)
        {
            Ptr<Node> ue = ueNodes[i].Get(u);
            // Set the default gateway for the UE
            Ptr<Ipv4StaticRouting> ueStaticRouting =
                ipv4RoutingHelper.GetStaticRouting(ue->GetObject<Ipv4>());
            ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

            for (uint32_t b = 0; b < numBearersPerUe; ++b)
            {
                ApplicationContainer clientApps;
                ApplicationContainer serverApps;
                Ptr<EpcTft> tft = Create<EpcTft>();

                if (!disableDl)
                {
                    ++dlPort;

                    NS_LOG_LOGIC("installing UDP DL app for UE " << u);

                    UdpClientHelper dlClientHelper(ueIpIfaces[i].GetAddress(u), dlPort);
                    clientApps.Add(dlClientHelper.Install(remoteHost));

                    dlClientHelper.SetAttribute("Interval", TimeValue(interPacketInterval));
                    dlClientHelper.SetAttribute("MaxPackets", UintegerValue(1000000));
                    PacketSinkHelper dlPacketSinkHelper(
                        "ns3::UdpSocketFactory",
                        InetSocketAddress(Ipv4Address::GetAny(), dlPort));
                    serverApps.Add(dlPacketSinkHelper.Install(ue));

                    EpcTft::PacketFilter dlpf;
                    dlpf.localPortStart = dlPort;
                    dlpf.localPortEnd = dlPort;
                    tft->Add(dlpf);
                }

                if (!disableUl)
                {
                    ++ulPort;

                    NS_LOG_LOGIC("installing UDP UL app for UE " << u);
                    UdpClientHelper ulClientHelper(remoteHostAddr, ulPort);
                    ulClientHelper.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambda)));

                    clientApps.Add(ulClientHelper.Install(ue));
                    PacketSinkHelper ulPacketSinkHelper(
                        "ns3::UdpSocketFactory",
                        InetSocketAddress(Ipv4Address::GetAny(), ulPort));
                    serverApps.Add(ulPacketSinkHelper.Install(remoteHost));

                    EpcTft::PacketFilter ulpf;
                    ulpf.remotePortStart = ulPort;
                    ulpf.remotePortEnd = ulPort;
                    tft->Add(ulpf);
                }

                EpsBearer bearer(EpsBearer::NGBR_VIDEO_TCP_DEFAULT);

                lteHelper->ActivateDedicatedEpsBearer(ueLteDevs[i].Get(u), bearer, tft);

                // ueLteDevs[i].Get(u)->TraceConnectWithoutContext(
                //         "Tx", MakeCallback(&BytesTransferredCallback));


                serverApps.Start(MilliSeconds(500));
                clientApps.Start(MilliSeconds(500));
                clientApps.Stop(simTime);

            } // end for b
        }
    }

    // Add X2 interface
    lteHelper->AddX2Interface(enbNodes);

    // X2-based Handover
    // for (int i = 0; i < numberOfEnbs; i++) {
    //     lteHelper->HandoverRequest(MilliSeconds(300),
    //                                ueLteDevs[i].Get(0),
    //                                enbLteDevs.Get(0),
    //                                enbLteDevs.Get(1));
    // }

    // Uncomment to enable PCAP tracing
    // p2ph.EnablePcapAll("lena-x2-handover");

    // lteHelper->EnablePhyTraces();
    // lteHelper->EnableMacTraces();
    // lteHelper->EnableRlcTraces();
    // lteHelper->EnablePdcpTraces();

    // lteHelper->EnableTraces();
    // lteHelper->EnableLogComponents();
    // Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats();
    // rlcStats->SetAttribute("EpochDuration", TimeValue(Seconds(3.05)));
    // Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats();
    // pdcpStats->SetAttribute("EpochDuration", TimeValue(Seconds(3.05)));
    // p2ph.EnableAsciiAll(stream);


    // connect custom trace sinks for RRC connection establishment and handover notification


    std::ofstream velocityOs;
    velocityOs.open("velocity");

    for (int i = 0; i < numberOfEnbs; i++) {
        for (uint16_t j = 0; j < numberOfUes; j++)
        {
        }
    }

    Config::Connect("/NodeList/*/$ns3::MobilityModel/CourseChange",
            MakeBoundCallback(&CourseChange, &velocityOs));

    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
                    MakeCallback(&NotifyConnectionEstablishedEnb));
    Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
                    MakeCallback(&NotifyConnectionEstablishedUe));
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
                    MakeCallback(&NotifyHandoverStartEnb));
    Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
                    MakeCallback(&NotifyHandoverStartUe));
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
                    MakeCallback(&NotifyHandoverEndOkEnb));
    Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
                    MakeCallback(&NotifyHandoverEndOkUe));

    // Hook a trace sink (the same one) to the four handover failure traces
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureNoPreamble",
                    MakeCallback(&NotifyHandoverFailure));
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureMaxRach",
                    MakeCallback(&NotifyHandoverFailure));
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureLeaving",
                    MakeCallback(&NotifyHandoverFailure));
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureJoining",
                    MakeCallback(&NotifyHandoverFailure));


    // Flow monitor

    Ptr<FlowMonitor> flowMon0;
    Ptr<FlowMonitor> flowMon1;
    Ptr<FlowMonitor> flowMonitor;


    // graph 3
    Ptr<const MobilityModel> mobility = ueNodes[0].Get(0)->GetObject<MobilityModel>();
    std::cout << mobility->GetPosition().x << std::endl;
    flowMon0 = flowHelper0.Install(remoteHost);
    flowMon1 = flowHelper1.Install(remoteHost);

    std::cout << NodeList::GetNNodes() << std::endl;

    for (int i = 0; i < numberOfEnbs; i++) {
        for (int j = 0; j < numberOfUes;j++) {
            Ptr<MobilityModel> mobility =
                ueNodes[i].Get(j)->GetObject<MobilityModel>();
            std::cout << "Mobility\n\n" << mobility->GetVelocity().x << " " << mobility->GetVelocity().y << std::endl;
            // if (mobility->GetVelocity().x != 0) {
                // flowMon0 = flowHelper.Install(ueNodes[i].Get(j));
            // } else {
                // flowMon1 = flowHelper.Install(ueNodes[i].Get(j));
            // }
        }
    }

    // for GRAPH3
    Simulator::Schedule(Seconds(0.1), &PlotGraph3, flowMon0, flowMon1, &ueNodes[0]);

    Simulator::Schedule(Seconds(1.1), &CalculateThroughput, flowMon0, true);
    Simulator::Schedule(Seconds(1.1), &CalculateThroughput, flowMon1, false);

    // for GRAPH4
    // Simulator::Schedule(Seconds(1.1), &PlotGraph4, flowMon0, ueNodes[0].Get(0));
    // PlotGraph4(flowMon0, ueNodes[0].Get(0));

    // Simulator::Schedule(Seconds(1.1), &CalculateThroughput, flowMonitor, std::ref(throughputArray1));
    // CalculateThroughput(flowMon0, throughputArray0);
    // CalculateThroughput(flowMon1, throughputArray1);
    Simulator::Schedule(Seconds(0.5), &CalculateCumulativeThroughput);
    Simulator::Stop(simTime);
    Simulator::Run();


    // GtkConfigStore config;
    // config.ConfigureAttributes ();


    flowMon0->SerializeToXmlFile("flowMon0.tr", false, false);
    flowMon1->SerializeToXmlFile("flowMon1.tr", false, false);
    Simulator::Destroy();

    return 0;
}

