#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include <string>
#include <typeinfo>
using namespace ns3;



NS_LOG_COMPONENT_DEFINE("ComplexTopology");
const int SERVER_PORT = 9;
const int NODES = 8;
const int CONECCIONS = 9;
const int TOPOLOGY[CONECCIONS][2] = {
		{0,2},
		{2,3},{1,2},
		{1,4},{4,3},
		{4,5},{4,6},
		{6,7},{5,7}
};


char IP[CONECCIONS][10] = { "10.1.1.0", "10.1.2.0", "10.1.3.0",
							"10.1.4.0", "10.1.5.0", "10.1.6.0",
							"10.1.7.0","10.1.8.0","10.1.9.0"};
char MASK[] = "255.255.255.0";


const int SERVER = 0; //We set the first node as the server
const int CLIENT = NODES - 1; //We set the last node as the client.
const int PACKAGES = 10000;
const int INTERVAL = 0.0001;
const int PACKET_SIZE = 10000;


int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);
    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(NODES); //Creates a network topology with n = NODES devices.

    /*
     * SETS THE PARAMETERS FOR THE COMMUNICATION BETWEEN TWO NODES.
    */

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices[CONECCIONS];  //Setting network devices


    for(int i = 0; i < CONECCIONS;i++){
    	devices[i] = pointToPoint.Install(nodes.Get(TOPOLOGY[i][0]), nodes.Get(TOPOLOGY[i][1]));

       }



    InternetStackHelper stack;
    stack.Install(nodes);





    Ipv4AddressHelper address[CONECCIONS];
    Ipv4InterfaceContainer interfaces[CONECCIONS];


    for(int i = 0; i < CONECCIONS; i++){

    	address[i].SetBase(IP[i], MASK);


    }


    for(int i = 0; i < CONECCIONS; i++){
    	interfaces[i] = address[i].Assign(devices[i]);

    }



    UdpEchoServerHelper echoServer(SERVER_PORT);  //Port of Network 1


    ApplicationContainer serverApps = echoServer.Install(nodes.Get(SERVER));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));



    UdpEchoClientHelper echoClient(interfaces[0].GetAddress(0), SERVER_PORT); //Since the first node is always the server.
    echoClient.SetAttribute("MaxPackets", UintegerValue(PACKAGES));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(INTERVAL)));
    echoClient.SetAttribute("PacketSize", UintegerValue(PACKET_SIZE));



    ApplicationContainer clientApps = echoClient.Install(nodes.Get(CLIENT));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(6.0));



    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    pointToPoint.EnablePcapAll("second");




    AnimationInterface anim("ComplexTopology.xml");
    anim.SetConstantPosition(nodes.Get(0),0.0,50.0);


    anim.SetConstantPosition(nodes.Get(1),35.0,25.0);
    anim.SetConstantPosition(nodes.Get(2),25.0,50.0);
    anim.SetConstantPosition(nodes.Get(3),35.0,75.0);
    anim.SetConstantPosition(nodes.Get(4),50.0,50.0);
    anim.SetConstantPosition(nodes.Get(5),75.0,25.0);
    anim.SetConstantPosition(nodes.Get(6),75.0,75.0);

    anim.SetConstantPosition(nodes.Get(NODES - 1),100.0,50.0);


    anim.EnablePacketMetadata(true); // Optional
    anim.EnableIpv4RouteTracking ("LTE-animation-ComplexTopology.xml", Seconds(0), Seconds(5), Seconds(0.25)); //Optional


    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
