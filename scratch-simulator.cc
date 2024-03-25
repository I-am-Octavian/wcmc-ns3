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
#include "ns3/mobility-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/radio-environment-map-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/multi-model-spectrum-channel.h"
#include "ns3/friis-spectrum-propagation-loss.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ScratchSimulator");

int
main(int argc, char* argv[])
{
    NS_LOG_UNCOND("Scratch Simulator");

    uint32_t ulResourceBlocks = 50;
    uint32_t dlResourceBlocks = 50; 

    Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(40));
    Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (dlResourceBlocks));
    Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (ulResourceBlocks));

    // Create eNB and UE
    NodeContainer enbNodes;
    std::vector<NodeContainer> ueNodes;
    enbNodes.Create(4);
    for(uint32_t i = 0; i < enbNodes.GetN(); ++i)
    {
        NodeContainer iueNodes;
        iueNodes.Create(10);
        ueNodes.push_back(iueNodes);
    }

    Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator>();
    enbPositionAlloc->Add(Vector(0.0, 0.0, 0.0));
    enbPositionAlloc->Add(Vector(5000.0, 0.0, 0.0));
    enbPositionAlloc->Add(Vector(5000.0, 5000.0, 0.0));
    enbPositionAlloc->Add(Vector(0.0, 5000.0, 0.0));

    // Install Mobility Model
    MobilityHelper enbMobility;
    enbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    enbMobility.SetPositionAllocator(enbPositionAlloc);
    enbMobility.Install(enbNodes);


    // Set UE positions
    MobilityHelper ueMobility;
    Ptr<UniformRandomVariable> ranVar = CreateObject<UniformRandomVariable>();
    ranVar->SetAttribute("Min", DoubleValue(0));
    ranVar->SetAttribute("Max", DoubleValue(500));

    ueMobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", 
                             "Bounds", RectangleValue (Rectangle (-1000, 6000, -1000, 6000)), // Setting bounds
                             "Speed", StringValue ("ns3::UniformRandomVariable[Min=0|Max=10]"), // Min speed 0, Max speed 10
                             "Distance", DoubleValue (100), // The distance to travel before changing direction
                             "Time", TimeValue (Seconds (1.0))); // The time to travel before changing direction
    uint32_t counter = 0;
    for(const auto& ueGroup : ueNodes)
    {
        ueMobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                    "X", DoubleValue((counter & 1)? 0 : 5000),
                                    "Y", DoubleValue(((counter >> 1) & 1)? 0 : 5000),
                                    "Rho", PointerValue(ranVar));
        ueMobility.Install(ueGroup);
        counter++;
    }

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    Ptr<Node> pgw = epcHelper->GetPgwNode();

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // Create the Internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("1Gb/s")));
    p2ph.SetChannelAttribute("Delay", TimeValue(MilliSeconds(1)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    // interface 0 is localhost, 1 is the p2p device
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    // Create devices and Install Nodes
    NetDeviceContainer enbDevices;
    enbDevices = lteHelper->InstallEnbDevice(enbNodes);
    Ipv4InterfaceContainer ueIpIface;
    std::vector<NetDeviceContainer> ueDevices;
    for(const auto& ueGroup : ueNodes)
    {
        NetDeviceContainer aueDeviceGroup;
        aueDeviceGroup = lteHelper->InstallUeDevice(ueGroup);
        internet.Install(ueGroup);
        ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(aueDeviceGroup));
        // Assign IP address to UEs, and install applications
        for (uint32_t u = 0; u < ueGroup.GetN(); ++u)
        {
            Ptr<Node> ueNode = ueGroup.Get(u);
            // Set the default gateway for the UE
            Ptr<Ipv4StaticRouting> ueStaticRouting =
                ipv4RoutingHelper.GetStaticRouting(ueNode->GetObject<Ipv4>());
            ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
        }
        
        ueDevices.push_back(aueDeviceGroup);
    }

    // Attach a UE to closest eNB automatically
    for(const auto& ueDevice : ueDevices)
    {
        lteHelper->Attach(ueDevice);
    }

    // Activate an EPS bearer
    //EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    //EpsBearer bearer(q);
    //for(const auto& ueDevice : ueDevices)
    //{
    //    lteHelper->ActivateDataRadioBearer(ueDevice, bearer);
    //}

    // Configure Radio Environment Map (REM) output
    // for LTE-only simulations use lteHelper downlink channel
    Ptr<RadioEnvironmentMapHelper> remHelper = CreateObject<RadioEnvironmentMapHelper>();
    remHelper->SetAttribute("Channel", PointerValue(lteHelper->GetDownlinkSpectrumChannel()));
    remHelper->SetAttribute("OutputFile", StringValue("rem.out"));
    remHelper->SetAttribute("XMin", DoubleValue(-1000.0));
    remHelper->SetAttribute("XMax", DoubleValue(6000.0));
    remHelper->SetAttribute("YMin", DoubleValue(-1000.0));
    remHelper->SetAttribute("YMax", DoubleValue(6000.0));
    remHelper->SetAttribute("Z", DoubleValue(0.0));
    remHelper->Install();

    bool disableDl = false;
    bool disableUl = false;
    Time interPacketInterval = MilliSeconds(1);

    // Install and start applications on UEs and remote host
    uint16_t dlPort = 1100;
    uint16_t ulPort = 2000;
    ApplicationContainer clientApps;
    ApplicationContainer serverApps;
    for(const auto& ueGroup : ueNodes)
    {

        for (uint32_t u = 0; u < ueGroup.GetN(); ++u)
        {
            if (!disableDl)
            {
                PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                                    InetSocketAddress(Ipv4Address::GetAny(), dlPort));
                serverApps.Add(dlPacketSinkHelper.Install(ueGroup.Get(u)));

                UdpClientHelper dlClient(ueIpIface.GetAddress(u), dlPort);
                dlClient.SetAttribute("Interval", TimeValue(interPacketInterval));
                dlClient.SetAttribute("MaxPackets", UintegerValue(1000000));
                clientApps.Add(dlClient.Install(remoteHost));
            }

            if (!disableUl)
            {
                ++ulPort;
                PacketSinkHelper ulPacketSinkHelper("ns3::UdpSocketFactory",
                                                    InetSocketAddress(Ipv4Address::GetAny(), ulPort));
                serverApps.Add(ulPacketSinkHelper.Install(remoteHost));

                UdpClientHelper ulClient(remoteHostAddr, ulPort);
                ulClient.SetAttribute("Interval", TimeValue(interPacketInterval));
                ulClient.SetAttribute("MaxPackets", UintegerValue(1000000));
                clientApps.Add(ulClient.Install(ueGroup.Get(u)));
            }
        }
    }


    serverApps.Start(MilliSeconds(500));
    clientApps.Start(MilliSeconds(500));
    lteHelper->EnableTraces();

    Simulator::Stop(Seconds(30));

    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
