#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/animation-interface.h"
#include "ns3/ai-module.h"
#include <cstdint>

#include "mcs-model.h"

// #include "ns3/gtk-config-store.h"

using namespace ns3;
using std::vector;

NS_LOG_COMPONENT_DEFINE("ASSIGNMENT");

const uint16_t numberOfUes = 10;
const uint16_t numberOfEnbs = 4;
FlowMonitorHelper flowHelper0;
FlowMonitorHelper flowHelper1;
vector<uint32_t> throughputArray0;
vector<uint32_t> throughputArray1;
std::string schType = "RR";
float speed = 10;
uint64_t previousRx = 0;

static void
PlotGraph3(ApplicationContainer& serverApps) {
    std::cout << serverApps.GetN() << "\n\n\n";
    for(size_t i = 0; i < serverApps.GetN(); ++i)
    {
        // Ptr<Node> node = serverApps.Get(i)->GetNode();
        Ptr<PacketSink> sink = DynamicCast<PacketSink>(serverApps.Get(i));
        // Ptr<MobilityModel> mobility =
        //     node->GetObject<MobilityModel>();
        // if (mobility->GetVelocity().x == 0 && mobility->GetVelocity().y == 0) {
        if (i % 2 == 0) {
            throughputArray0.push_back(sink->GetTotalRx());
        } else {
            throughputArray1.push_back(sink->GetTotalRx());
        }

    }
    Simulator::Schedule(Seconds(1), &PlotGraph3, serverApps);

}

static void
CalculateThroughput(Ptr<FlowMonitor> monitor, bool flag) {
    // FlowMonitorHelper flowmon;
    // Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
    std::cout << "SIZE: " << stats.size() << std::endl;
    for (auto i = stats.begin (); i != stats.end (); ++i)
    {
          // Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

        if (flag) {
            throughputArray0.push_back(i->second.rxBytes);
        } else if (!flag) {
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
    os1.open("graph/graph3speed5.txt", std::ios::app);

    uint32_t prev0 = -1;
    if (!throughputArray0.empty()) {
        sort(throughputArray0.begin(), throughputArray0.end());
        prev0 = throughputArray0[0];
    } else  {
        std::cout << "Array 0 is empty" << std::endl;
    }


    uint32_t prev1 = -1;
    if (!throughputArray1.empty()) {
        sort(throughputArray1.begin(), throughputArray1.end());
        prev1 = throughputArray1[0];
    } else  {
        std::cout << "Array 1 is empty" << std::endl;
    }


    std::cout << "Array size 0: " << throughputArray0.size() << std::endl;
    for (uint32_t i = 1; i < throughputArray0.size(); i++) {
        if (throughputArray0[i] != prev0) {
            os0 << throughputArray0[i] << " " << i << std::endl;
            std::cout << throughputArray0[i] << " " << i << std::endl;
            prev0 = throughputArray0[i];
        } else if (i == throughputArray0.size() -1) {
            os0 << throughputArray0[i] << " " << i << std::endl;
            std::cout << throughputArray0[i] << " " << i << std::endl;
        }
    }

    std::cout << "Array size 1: " << throughputArray1.size() << std::endl;
    for (uint32_t i = 1; i < throughputArray1.size(); i++) {
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

// Plotting graph for 4, 5 and 6 since they all use the same data on different params
static void
PlotInstantaneousThroughput(ApplicationContainer& apps, uint32_t graphNum) {
    std::string fileName;
    fileName = "graph/graph";
    fileName.append(std::to_string(graphNum));
    fileName.append(schType);
    fileName.append(".txt");

    std::ofstream os;
    os.open(fileName, std::ios::app);
    Ptr<PacketSink> sink;
    Ptr<Node> node;
    if (graphNum == 4 || graphNum == 6) {
        sink = DynamicCast<PacketSink>(apps.Get(25));
        node = apps.Get(0)->GetNode();
    } else if (graphNum == 5 || graphNum == 7) {
        sink = DynamicCast<PacketSink>(apps.Get(26));
        node = apps.Get(1)->GetNode();
    }

    // for (int i = 0; i < apps.GetN(); i++) {
        // sink = DynamicCast<PacketSink>(apps.Get(i));

    uint64_t currRx = sink->GetTotalRx();
    Ptr<MobilityModel> mob = node->GetObject<MobilityModel>();
    NS_LOG_UNCOND(mob->GetVelocity().x << " " << mob->GetVelocity().y);
    os << (currRx - previousRx) * 8.0 / 1024 / 1024 << " "
        << Simulator::Now().GetSeconds()  << std::endl;
    previousRx = currRx;
    // }

    Simulator::Schedule(Seconds(1.0), &PlotInstantaneousThroughput, std::ref(apps), graphNum);
    os.close();
}

// to write the velocity from callback
//static void
//CourseChange(std::ostream* os, std::string foo, Ptr<const MobilityModel> mobility)
//{
//    Vector vel = mobility->GetVelocity(); // Get velocity
//    // Prints position and velocities
//    *os << vel.x << " " << vel.y << std::endl;
//}

/**
 * UE Connection established notification.
 *
 * \param context The context.
 * \param imsi The IMSI of the connected terminal.
 * \param cellid The Cell ID.
 * \param rnti The RNTI.
 */
//void
//NotifyConnectionEstablishedUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
//{
//    std::cout << Simulator::Now().As(Time::S) << " " << context << " UE IMSI " << imsi
//              << ": connected to CellId " << cellid << " with RNTI " << rnti << std::endl;
//}

/**
 * UE Start Handover notification.
 *
 * \param context The context.
 * \param imsi The IMSI of the connected terminal.
 * \param cellid The actual Cell ID.
 * \param rnti The RNTI.
 * \param targetCellId The target Cell ID.
 */
//void
//NotifyHandoverStartUe(std::string context,
//                      uint64_t imsi,
//                      uint16_t cellid,
//                      uint16_t rnti,
//                      uint16_t targetCellId)
//{
//    std::cout << Simulator::Now().As(Time::S) << " " << context << " UE IMSI " << imsi
//              << ": previously connected to CellId " << cellid << " with RNTI " << rnti
//              << ", doing handover to CellId " << targetCellId << std::endl;
//}
//
///**
// * UE Handover end successful notification.
// *
// * \param context The context.
// * \param imsi The IMSI of the connected terminal.
// * \param cellid The Cell ID.
// * \param rnti The RNTI.
// */
//void
//NotifyHandoverEndOkUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
//{
//    std::cout << Simulator::Now().As(Time::S) << " " << context << " UE IMSI " << imsi
//              << ": successful handover to CellId " << cellid << " with RNTI " << rnti << std::endl;
//}
//
///**
// * eNB Connection established notification.
// *
// * \param context The context.
// * \param imsi The IMSI of the connected terminal.
// * \param cellid The Cell ID.
// * \param rnti The RNTI.
// */
////void
////NotifyConnectionEstablishedEnb(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
////{
////    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
////              << ": successful connection of UE with IMSI " << imsi << " RNTI " << rnti
////              << std::endl;
////}
//
///**
// * eNB Start Handover notification.
// *
// * \param context The context.
// * \param imsi The IMSI of the connected terminal.
// * \param cellid The actual Cell ID.
// * \param rnti The RNTI.
// * \param targetCellId The target Cell ID.
// */
//void
//NotifyHandoverStartEnb(std::string context,
//                       uint64_t imsi,
//                       uint16_t cellid,
//                       uint16_t rnti,
//                       uint16_t targetCellId)
//{
//    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
//              << ": start handover of UE with IMSI " << imsi << " RNTI " << rnti << " to CellId "
//              << targetCellId << std::endl;
//}
//
///**
// * eNB Handover end successful notification.
// *
// * \param context The context.
// * \param imsi The IMSI of the connected terminal.
// * \param cellid The Cell ID.
// * \param rnti The RNTI.
// */
//void
//NotifyHandoverEndOkEnb(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
//{
//    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
//              << ": completed handover of UE with IMSI " << imsi << " RNTI " << rnti << std::endl;
//}
//
///**
// * Handover failure notification
// *
// * \param context The context.
// * \param imsi The IMSI of the connected terminal.
// * \param cellid The Cell ID.
// * \param rnti The RNTI.
// */
//void
//NotifyHandoverFailure(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
//{
//    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
//              << " IMSI " << imsi << " RNTI " << rnti << " handover failure" << std::endl;
//}
//
//
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
            return speed;
        }
        return -1;
    }

    uint32_t GetInteger() {
        return static_cast<uint32_t>(GetValue());
    }

};

class ConstVariable: public RandomVariableStream {
    public:
    double GetValue() {
        return 10;
    }

    uint32_t GetInteger() {
        return static_cast<uint32_t>(GetValue());
    }

};

// PART B


void GetMCS() {
    // Communicating with the AI
    std::cout << "MCS MCS\n";
    Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>* msgInterface =
        Ns3AiMsgInterface::Get()->GetInterface<MCSFeature, MCSPredicted>();
    msgInterface->CppRecvBegin();
    uint32_t ret = msgInterface->GetPy2CppStruct()->mcsPredicted;
    msgInterface->CppRecvEnd();

    std::cout << ret << std::endl;
    Simulator::Schedule (Seconds (0.1), &GetMCS);
};
const int TOT_COUNT = 12;
int currCount = 1;

void
SetMCS(NetDeviceContainer* ndc)
{
#ifdef GRAPH_PARTB
    Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>* msgInterface =
        Ns3AiMsgInterface::Get()->GetInterface<MCSFeature, MCSPredicted>();
#endif

    Ptr<NetDevice> netDevice;
    for(int j = 0; j < 4; ++j)
    {
        NetDeviceContainer c = ndc[j];
        for (auto i = c.Begin(); i != c.End(); ++i)
        {
            netDevice = (*i);

            Ptr<MobilityModel> mobility = netDevice->GetNode()->GetObject<MobilityModel>();

            Ptr<LteUeNetDevice> lteUe = DynamicCast<LteUeNetDevice>(netDevice);
            if (lteUe)
            {
                // Ptr<LteUeRrc> ueRrc = lteUe->GetRrc();
                // uint64_t imsi = ueRrc->GetImsi();
                // if(ns3::PhyRxStatsCalculator::imsiToMcs.contains(imsi))
                // {
                //     uint8_t mcs = ns3::PhyRxStatsCalculator::imsiToMcs[imsi];
                    // NS_LOG_UNCOND(imsi << ',' << pos.x << ',' << pos.y << ',' << (int)mcs);
                // }
                // Sending the model velocities
                NS_LOG_UNCOND(mobility->GetPosition().x << ',' << mobility->GetPosition().y);


#ifdef GRAPH_PARTB
                float x = mobility->GetPosition().x;
                float y =mobility->GetPosition().y;
                float decX = 0, decY = 0;
                decX = (x - (int)x) * 1000;
                decY = (y - (int)y) * 1000;
                if (TOT_COUNT > currCount) {
                    msgInterface->CppSendBegin();
                    msgInterface->GetCpp2PyStruct()->posX = (int)x;
                    msgInterface->GetCpp2PyStruct()->posY = (int)y;
                    msgInterface->GetCpp2PyStruct()->decimalX = (int)decX;
                    msgInterface->GetCpp2PyStruct()->decimalY = (int)decY;
                    msgInterface->CppSendEnd();
                }
                else {
                    msgInterface->CppSendBegin();
                    msgInterface->GetCpp2PyStruct()->posX = (int)x;
                    msgInterface->GetCpp2PyStruct()->posY = (int)y;
                    msgInterface->GetCpp2PyStruct()->decimalX = (int)decX;
                    msgInterface->GetCpp2PyStruct()->decimalY = (int)decY;
                    msgInterface->CppSendEnd();

                    std::cout << "MCS MCS\n";
                    msgInterface->CppRecvBegin();
                    uint32_t ret = msgInterface->GetPy2CppStruct()->mcsPredicted;
                    msgInterface->CppRecvEnd();
                    std::cout << ret << std::endl;
                }
                currCount++;
                std::cout << "COUNTER: " << currCount << std::endl;
#endif
            }
        }
    }

    Simulator::Schedule (Seconds (0.2), &SetMCS, ndc);
}


int
main(int argc, char* argv[])
{
    //Config::SetDefault("ns3::FdMtFfMacScheduler::HarqEnabled", BooleanValue(false));

    // LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);
//
    // LogComponentEnable ("LteHelper", logLevel);
    // LogComponentEnable ("EpcHelper", logLevel);
    // LogComponentEnable ("EpcEnbApplication", logLevel);
    // LogComponentEnable ("EpcMmeApplication", logLevel);
    // LogComponentEnable ("EpcPgwApplication", logLevel);
    // LogComponentEnable ("EpcSgwApplication", logLevel);
    // LogComponentEnable ("EpcX2", logLevel);
//
    // LogComponentEnable ("LteEnbRrc", logLevel);
    // LogComponentEnable ("LteEnbNetDevice", logLevel);
    // LogComponentEnable ("LteUeRrc", logLevel);
    // LogComponentEnable ("LteUeNetDevice", logLevel);
    //
    // LogComponentEnable("UdpClient", LOG_LEVEL_ALL);
    // LogComponentEnable("UdpServer", LOG_LEVEL_ALL);

    uint16_t numBearersPerUe = 1;
    Time simTime = Seconds(30); //NOTE: Not using Bi random var
    // double distance = 100.0;
    bool disableDl = false;
    bool disableUl = true;

    uint32_t ulResourceBlocks = 50;
    uint32_t dlResourceBlocks = 50;

    uint64_t runNumber = 1;
	unsigned int rngRun = 1;
	unsigned int seed = 50;
    bool fullBufferFlag = true;
    unsigned int graphNum = 1;

    // Command line arguments
    CommandLine cmd(__FILE__);
    cmd.AddValue("scheduler", "Type of schedular to use, PF, RR, MT, PSS", schType);
    cmd.AddValue("graphNumber", "Graph to Plot: 1, 2, 3, 4, 5, 6, 7 (graph 6 b), \
            8 (part b generation), 9 (part b prediction)", graphNum);
	cmd.AddValue("speed", "Speed of the UE under simulator", speed);
	cmd.AddValue("seed", "Speed of the UE under simulator", seed);
	cmd.AddValue("rngRun", "Seed for the random simulation", rngRun);
	cmd.AddValue("fullBufferFlag", "Flag to enable or disable full Buffer",
				 fullBufferFlag);
    cmd.Parse(argc, argv);

    ns3::RngSeedManager::SetSeed(seed);
    ns3::RngSeedManager::SetRun(runNumber);

    // change some default attributes so that they are reasonable for
    // this scenario, but do this before processing command line
    // arguments, so that the user is allowed to override these settings
    Config::SetDefault("ns3::UdpClient::Interval", TimeValue(MilliSeconds(200)));
    Config::SetDefault("ns3::UdpClient::MaxPackets", UintegerValue(4000000000));
    Config::SetDefault("ns3::UdpClient::PacketSize", UintegerValue(1500));
    Config::SetDefault("ns3::LteHelper::UseIdealRrc", BooleanValue(false));

    Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(40));
    Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (dlResourceBlocks));
    Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (ulResourceBlocks));


     double lambda = 1000;
     double bandwidth = 100e6;
     uint32_t udpPacketSize = 1500;
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

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    if (schType == "PF") {
        lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");
    } else if (schType == "RR") {
        lteHelper->SetSchedulerType("ns3::RrFfMacScheduler");
    } else if (schType == "MT") {
        lteHelper->SetSchedulerType("ns3::FdMtFfMacScheduler");
    } else if (schType == "PSS") {
        lteHelper->SetSchedulerType("ns3::PssFfMacScheduler");
    }
    lteHelper->SetHandoverAlgorithmType("ns3::A2A4RsrqHandoverAlgorithm"); // disable automatic handover
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);
    lteHelper->SetAttribute("PathlossModel", StringValue("ns3::FriisSpectrumPropagationLossModel"));
    NS_LOG_UNCOND(lteHelper->GetSchedulerType());

    Ptr<Node> pgw = epcHelper->GetPgwNode();

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // Create the Internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("12Mb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(1500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.0001)));
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

    std::vector<NodeContainer> ueNodes;
    NodeContainer enbNodes;
    enbNodes.Create(numberOfEnbs);
    for (int i = 0; i < numberOfEnbs; i++) {
        NodeContainer ctr;
        ctr.Create(numberOfUes);
        ueNodes.push_back(ctr);
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
    ranVar->SetAttribute("Max", DoubleValue(1000));

    Ptr<BiRandomVariable> bi = CreateObject<BiRandomVariable>();
    // Ptr<ConstVariable> constSpeed = CreateObject<ConstVariable>();

    Ptr<UniformRandomVariable> urv = CreateObject<UniformRandomVariable>();
    urv->SetAttribute("Min", DoubleValue(0));
    urv->SetAttribute("Max", DoubleValue(10));

    //ueMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

    ueMobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-1100, 6100, -1100, 6100)), // Setting bounds
                             "Speed", PointerValue (bi));
                             // "Mode", StringValue("Time"),
                             //"Distance", DoubleValue (100), // The distance to travel before changing direction
                             // "Time", TimeValue (Seconds (0.50))); // The time to travel before changing direction


    // FIXED

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

    // FIXED
    NetDeviceContainer ueLteDevs[numberOfEnbs];

    for (int i = 0; i < numberOfEnbs; i++) {
        ueLteDevs[i] = lteHelper->InstallUeDevice(ueNodes[i]);
        internet.Install(ueNodes[i]);
    }

    // Install the IP stack on the UEs
    Ipv4InterfaceContainer ueIpIfaces[numberOfEnbs];

    // FIXED
    for (int i = 0; i < numberOfEnbs; i++) {
        ueIpIfaces[i] = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs[i]));
    }

    // Attach all UEs to the first eNodeB
    for (int i = 0; i < numberOfEnbs; i++) {
        for (uint16_t j = 0; j < numberOfUes; j++)
        {
            // FIXED
            lteHelper->Attach(ueLteDevs[i].Get(j), enbLteDevs.Get(i));
        }
    }



    //AnimationInterface anim("anim.xml");

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

    ApplicationContainer clientApps;
    ApplicationContainer serverApps;

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
                Ptr<EpcTft> tft = Create<EpcTft>();

                if (!disableDl)
                {
                    ++dlPort;

                    NS_LOG_LOGIC("installing UDP DL app for UE " << u);

                    // FIXED
                    UdpClientHelper dlClientHelper(ueIpIfaces[i].GetAddress(u), dlPort);
                    clientApps.Add(dlClientHelper.Install(remoteHost));
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

                // FIXED
                lteHelper->ActivateDedicatedEpsBearer(ueLteDevs[i].Get(u), bearer, tft);

            } // end for b
        }
    }
    Time startTime = Seconds(startTimeSeconds->GetValue());
    serverApps.Start(startTime);
    clientApps.Start(startTime);
    clientApps.Stop(simTime);

    // Add X2 interface
    lteHelper->AddX2Interface(enbNodes);

    // X2-based Handover
    //for (int i = 0; i < numberOfEnbs; i++)
    //{
    //    lteHelper->HandoverRequest(MilliSeconds(300),
    //                               ueLteDevs[i].Get(0),
    //                               enbLteDevs.Get(0),
    //                               enbLteDevs.Get(1));
    //}

    // Uncomment to enable PCAP tracing
    // p2ph.EnablePcapAll("lena-x2-handover");

    lteHelper->EnablePhyTraces();
    lteHelper->EnableMacTraces();
    lteHelper->EnableRlcTraces();
    lteHelper->EnablePdcpTraces();
    Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats();
    rlcStats->SetAttribute("EpochDuration", TimeValue(Seconds(0.05)));
    Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats();
    pdcpStats->SetAttribute("EpochDuration", TimeValue(Seconds(0.05)));
    // connect custom trace sinks for RRC connection establishment and handover notification


    //std::ofstream velocityOs;
    //velocityOs.open("velocity");
    //Config::Connect("/NodeList/*/$ns3::MobilityModel/CourseChange",
    //        MakeBoundCallback(&CourseChange, &velocityOs));
    //Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
    //                MakeCallback(&NotifyConnectionEstablishedEnb));
    //Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
    //                MakeCallback(&NotifyConnectionEstablishedUe));

    //Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
    //                MakeCallback(&NotifyHandoverStartEnb));
    //Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
    //                MakeCallback(&NotifyHandoverStartUe));
    //Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
    //                MakeCallback(&NotifyHandoverEndOkEnb));
    //Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
    //                MakeCallback(&NotifyHandoverEndOkUe));
//
    //// Hook a trace sink (the same one) to the four handover failure traces
    //Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureNoPreamble",
    //                MakeCallback(&NotifyHandoverFailure));
    //Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureMaxRach",
    //                MakeCallback(&NotifyHandoverFailure));
    //Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureLeaving",
    //                MakeCallback(&NotifyHandoverFailure));
    //Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureJoining",
    //                MakeCallback(&NotifyHandoverFailure));


    auto interface = Ns3AiMsgInterface::Get();
    interface->SetIsMemoryCreator(false);
    interface->SetUseVector(false);
    interface->SetHandleFinish(true);


    Ptr<FlowMonitor> flowMon0;
    Ptr<FlowMonitor> flowMon1;
    flowMon0 = flowHelper0.Install(ueNodes[0].Get(0));
    // flowMon0 = flowHelper0.Install(remoteHost);
    flowMon1 = flowHelper1.Install(remoteHost);

    // Setting Up Part A graphs
    switch(graphNum) {
        case 1:
        {
            Ptr<RadioEnvironmentMapHelper> remHelper = CreateObject<RadioEnvironmentMapHelper>();
            remHelper->SetAttribute("Channel", PointerValue(lteHelper->GetDownlinkSpectrumChannel()));
            remHelper->SetAttribute("OutputFile", StringValue("rem.out"));
            remHelper->SetAttribute("XMin", DoubleValue(-1000.0));
            remHelper->SetAttribute("XMax", DoubleValue(6000.0));
            remHelper->SetAttribute("YMin", DoubleValue(-1000.0));
            remHelper->SetAttribute("YMax", DoubleValue(6000.0));
            remHelper->SetAttribute("Z", DoubleValue(0.0));
            remHelper->Install();
            break;
        }
        case 2:
        {
#define GRAPH2
            break;
        }
        case 3:
        {
            // PlotGraph3(flowMon0, flowMon1, &ueNodes[0]);
            // Ptr<ApplicationContainer> apps = CreateObject<ApplicationContainer>(serverApps);
            Simulator::Schedule(Seconds(1), &PlotGraph3, std::ref(serverApps));
            Simulator::Schedule(simTime - Seconds(0.3), &CalculateCumulativeThroughput);

            // Simulator::Schedule(Seconds(1.5), &CalculateThroughput, flowMon0, true);
            // Simulator::Schedule(Seconds(1.5), &CalculateThroughput, flowMon1, false);

        }
        case 4:
        case 5:
        case 6:
        case 7:
        {
            PlotInstantaneousThroughput(std::ref(serverApps), graphNum);
            break;
            }
        case 8: // dataset generation
        {
            Simulator::Schedule (Seconds (0.1), &SetMCS, ueLteDevs);
            break;
        }
        case 9: // machine learning prediction graph
        {
#define GRAPH_PARTB
            Simulator::Schedule (Seconds (0.1), &SetMCS, ueLteDevs);
            break;
        }
        default:
            NS_LOG_UNCOND("Enter a valid Graph Number to plot");
            break;
    }
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(simTime + MilliSeconds(2));
    Simulator::Run();

    flowHelper.SerializeToXmlFile ("scratch.flowmonitor", true, true);

    NS_LOG_UNCOND("Here");
    monitor->CheckForLostPackets();
    auto stats = monitor->GetFlowStats();
    NS_LOG_UNCOND(stats.size());
    Ptr<PacketSink> sink = DynamicCast<PacketSink>(serverApps.Get(1));
    NS_LOG_UNCOND(sink->GetTotalRx());


#ifdef GRAPH2
    float average_throughput = 0;
    counter = 0;
    for(const auto& flow : stats)
    {
        // Change to GetMiliSeconds()
        float throughput = (flow.second.rxBytes*8.0/(flow.second.timeLastRxPacket.GetSeconds() - flow.second.timeFirstRxPacket.GetSeconds()) /1024/1024);
        if (throughput > 0)
        {
            ++counter;
            average_throughput += throughput;
            std::cout << "THROUGHPUT " << throughput << std::endl;
        }
    }
    NS_LOG_UNCOND(counter);
    NS_LOG_UNCOND(average_throughput / 44);
#endif

// Yashwanth's Code for throughput:-
//if (i->second.rxPackets > 0)
//        {
//            // Measure the duration of the flow from receiver's perspective
//            double rxDuration = i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ();
//
//            averageFlowThroughput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
//            averageFlowDelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets;
//
//            outFile << "  Throughput: " << i->second.rxBytes * 8.0 /

    // GtkConfigStore config;
    // config.ConfigureAttributes ();

    Simulator::Destroy();
    return 0;
}
