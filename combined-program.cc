/*
 * Title: Combined Program
 * WNM Lab - ns-3 Simulation
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AclOnlySim");

// ACL filter function: drops every received packet
bool ACLFilter(Ptr<NetDevice> dev, Ptr<const Packet> pkt, uint16_t proto, const Address &from) {
    NS_LOG_INFO("ACL: Packet dropped");
    return false;
}

// Function that prints the static routing table of a node
void PrintRoutingTable(Ptr<Node> node) {
    Ipv4StaticRoutingHelper helper;
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
    Ptr<Ipv4StaticRouting> routing = helper.GetStaticRouting(ipv4);
    routing->PrintRoutingTable(Create<OutputStreamWrapper>(&std::cout));
}

int main() {
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    LogComponentEnable("AclOnlySim", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(2);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices = p2p.Install(nodes);
    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Static host route to node 1 on node 0
    Ipv4StaticRoutingHelper helper;
    Ptr<Ipv4StaticRouting> routing = helper.GetStaticRouting(nodes.Get(0)->GetObject<Ipv4>());
    routing->AddHostRouteTo(interfaces.GetAddress(1), interfaces.GetAddress(1), 1);

    // Attach ACL filter as receive callback on node 1 device
    devices.Get(1)->SetReceiveCallback(MakeCallback(&ACLFilter));

    // UDP echo server on node 1
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // UDP echo client on node 0
    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(4));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(64));
    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // Print node 0 routing table at t = 2 s
    Simulator::Schedule(Seconds(2.0), &PrintRoutingTable, nodes.Get(0));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
