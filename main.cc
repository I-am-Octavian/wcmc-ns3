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

#include "mcs-model.h"

// #include "ns3/gtk-config-store.h"

using namespace ns3;
using std::vector;

NS_LOG_COMPONENT_DEFINE("ASSIGNMENT");


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
            return 10;
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
const int TOT_COUNT = 160;
int currCount = 1;

void
SetMCS(NetDeviceContainer* ndc)
{
    NS_LOG_UNCOND("setmcs");
    Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>* msgInterface =
        Ns3AiMsgInterface::Get()->GetInterface<MCSFeature, MCSPredicted>();

    Ptr<NetDevice> netDevice;
#if 0
    for(int j = 0; j < 1; ++j)
    {
        NetDeviceContainer c = ndc[j];
        netDevice = (*c.Begin());
        
        Ptr<MobilityModel> mobility = netDevice->GetNode()->GetObject<MobilityModel>();
        float x = mobility->GetPosition().x;
        float y =mobility->GetPosition().y;

        Ptr<LteUeNetDevice> lteUe = DynamicCast<LteUeNetDevice>(netDevice);
        if (lteUe)
        {
            Ptr<LteUeRrc> ueRrc = lteUe->GetRrc();
            uint64_t imsi = ueRrc->GetImsi();
            if(ns3::PhyRxStatsCalculator::imsiToMcs.contains(imsi))
            {
                uint8_t mcs = ns3::PhyRxStatsCalculator::imsiToMcs[imsi];
                // Sending the model velocities
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

                    msgInterface->CppRecvBegin();
                    uint32_t ret = msgInterface->GetPy2CppStruct()->mcsPredicted;
                    msgInterface->CppRecvEnd();
                    NS_LOG_UNCOND(x << ' ' << y << ' ' << (int)mcs << ' ' << ret);
                }
                currCount++;
            }
        }
        
    }
#else
    for(int j = 0; j < 4; ++j)
    {
        NetDeviceContainer c = ndc[j];
        for (auto i = c.Begin(); i != c.End(); ++i)
        {
            netDevice = (*i);

            Ptr<MobilityModel> mobility = netDevice->GetNode()->GetObject<MobilityModel>();
            float x = mobility->GetPosition().x;
            float y =mobility->GetPosition().y;

            Ptr<LteUeNetDevice> lteUe = DynamicCast<LteUeNetDevice>(netDevice);
            if (lteUe)
            {
                Ptr<LteUeRrc> ueRrc = lteUe->GetRrc();
                uint64_t imsi = ueRrc->GetImsi();
                if(ns3::PhyRxStatsCalculator::imsiToMcs.contains(imsi))
                {
                    uint8_t mcs = ns3::PhyRxStatsCalculator::imsiToMcs[imsi];
                    // Sending the model velocities
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

                        msgInterface->CppRecvBegin();
                        uint32_t ret = msgInterface->GetPy2CppStruct()->mcsPredicted;
                        msgInterface->CppRecvEnd();
                        NS_LOG_UNCOND("pred " << x << ' ' << y << ' ' << (int)mcs << ' ' << ret);
                    }
                    currCount++;
                }
            }
        }
    }
#endif

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

    const uint16_t numberOfUes = 10;
    const uint16_t numberOfEnbs = 4;
    uint16_t numBearersPerUe = 1;
    Time simTime = Seconds(51); //NOTE: Not using Bi random var
    // double distance = 100.0;
    bool disableDl = false;
    bool disableUl = true;
    std::string schType = "ns3::PfFfMacScheduler";

    uint32_t ulResourceBlocks = 50;
    uint32_t dlResourceBlocks = 50;

    uint64_t runNumber = 1;

    // Command line arguments
    //CommandLine cmd(__FILE__);
    //cmd.AddValue("numberOfUes", "Number of UEs", numberOfUes);
    //cmd.AddValue("numberOfEnbs", "Number of eNodeBs", numberOfEnbs);
    //cmd.AddValue("simTime", "Total duration of the simulation", simTime);
    //cmd.AddValue("disableDl", "Disable downlink data flows", disableDl);
    //cmd.AddValue("disableUl", "Disable uplink data flows", disableUl);
    //cmd.AddValue("schType", "Type of scheduler", schType);
    //cmd.AddValue("fullBuffer", "Full Buffer or not", fullBuffer);
    //cmd.AddValue("runNumber", "Run Number", runNumber);
    //cmd.Parse(argc, argv);

    ns3::RngSeedManager::SetSeed(50);
    ns3::RngSeedManager::SetRun(runNumber);

    // change some default attributes so that they are reasonable for
    // this scenario, but do this before processing command line
    // arguments, so that the user is allowed to override these settings
    Config::SetDefault("ns3::UdpClient::Interval", TimeValue(MilliSeconds(8)));
    //Config::SetDefault("ns3::UdpClient::MaxPackets", UintegerValue(4000000000));
    Config::SetDefault("ns3::UdpClient::PacketSize", UintegerValue(1500));
    Config::SetDefault("ns3::LteHelper::UseIdealRrc", BooleanValue(false));

    Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(40));
    Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (dlResourceBlocks));
    Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (ulResourceBlocks));

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);
    lteHelper->SetSchedulerType(schType);
    lteHelper->SetHandoverAlgorithmType("ns3::A2A4RsrqHandoverAlgorithm"); // disable automatic handover
    lteHelper->SetAttribute("PathlossModel", StringValue("ns3::FriisSpectrumPropagationLossModel"));

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

    // FIXED
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
    ranVar->SetAttribute("Max", DoubleValue(500));

    //Ptr<BiRandomVariable> bi = CreateObject<BiRandomVariable>();
    Ptr<ConstVariable> constSpeed = CreateObject<ConstVariable>();

    Ptr<UniformRandomVariable> urv = CreateObject<UniformRandomVariable>();
    urv->SetAttribute("Min", DoubleValue(0));
    urv->SetAttribute("Max", DoubleValue(10));

    //ueMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

    ueMobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-600, 5600, -600, 5600)), // Setting bounds
                             "Speed", PointerValue (constSpeed),
                             "Mode", StringValue("Time"),
                             //"Distance", DoubleValue (100), // The distance to travel before changing direction
                             "Time", TimeValue (Seconds (0.50))); // The time to travel before changing direction


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


    // Ptr<RadioEnvironmentMapHelper> remHelper = CreateObject<RadioEnvironmentMapHelper>();
    // remHelper->SetAttribute("Channel", PointerValue(lteHelper->GetDownlinkSpectrumChannel()));
    // remHelper->SetAttribute("OutputFile", StringValue("rem.out"));
    // remHelper->SetAttribute("XMin", DoubleValue(-1000.0));
    // remHelper->SetAttribute("XMax", DoubleValue(6000.0));
    // remHelper->SetAttribute("YMin", DoubleValue(-1000.0));
    // remHelper->SetAttribute("YMax", DoubleValue(6000.0));
    // remHelper->SetAttribute("Z", DoubleValue(0.0));
    // remHelper->Install();

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

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> distrib(1, 10);

    // for (int i = 0; i < NUM_ENV; ++i)
    // {
    //     msgInterface->CppSendBegin();
    //     std::cout << "set: ";
    //     uint32_t temp_a = distrib(gen);
    //     uint32_t temp_b = distrib(gen);
    //     std::cout << temp_a << "," << temp_b << ";";
    //     msgInterface->GetCpp2PyStruct()->posX = temp_a;
    //     msgInterface->GetCpp2PyStruct()->posY = temp_b;
    //     std::cout << "\n";
    //     msgInterface->CppSendEnd();
    //
    //     msgInterface->CppRecvBegin();
    //     std::cout << "get: ";
    //     std::cout << msgInterface->GetPy2CppStruct()->mcsPredicted;
    //     std::cout << "\n";
    //     msgInterface->CppRecvEnd();
    // }

    FlowMonitorHelper flowHelper;

    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Schedule (Seconds (1), &SetMCS, ueLteDevs);

    Simulator::Stop(simTime + MilliSeconds(20));
    Simulator::Run();

    flowHelper.SerializeToXmlFile ("scratch.flowmonitor", true, true);

    NS_LOG_UNCOND("server " << DynamicCast<PacketSink>(serverApps.Get(0))->GetTotalRx());
    
    //Ptr<PacketSink> sink = DynamicCast<PacketSink> (sinkApps.Get (0));
    //std::cout << "Total Packets Received: " << sink->GetTotalRx () << std::endl;

    flowHelper.SerializeToXmlFile ("scratch.flowmonitor", true, true);

    uint64_t totalRx = 0;
    for(size_t i = 0; i < serverApps.GetN(); ++i)
    {
        Ptr<PacketSink> sink = DynamicCast<PacketSink>(serverApps.Get(i));
        totalRx += sink->GetTotalRx();
    }
    NS_LOG_UNCOND("Simulation Run for " << simTime.GetSeconds());
    NS_LOG_UNCOND("Total Rx packets = " << totalRx);
    double throughput = totalRx*8.0/simTime.GetSeconds() /1024 /1024;
    NS_LOG_UNCOND("Aggregate throughput = " << throughput << "Mbps");

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
