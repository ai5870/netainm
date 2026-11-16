/*
 * Title: Program to Display Routing Table
 * WNM Lab - ns-3 Simulation
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("RouteOnlySim");

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
    LogComponentEnable("RouteOnlySim", LOG_LEVEL_INFO);

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

    // Print node 0 routing table at t = 2 s
    Simulator::Schedule(Seconds(2.0), &PrintRoutingTable, nodes.Get(0));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
