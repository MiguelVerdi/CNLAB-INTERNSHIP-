
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/aodv-module.h"

/*
    File: WirelessTopology.cpp
    Author: Miguel Verdi
    Description: This C++ program  uses ns3 library to simulate the communication between
    2 devices, in a wireless and PtoP topology as shown.
    Topology:

  sta   10.1.1.0   ap  
  * * * * * * * * * 
  |               |           
 n0               n1          
*/


using namespace ns3;
int
main()
{
	/*
	 * Constants and parameters inicialization.
	 */
	const uint32_t WNETWORK1_DEVICES = 2; // Unit32_t specifies that the variable is exactly 32 bits.
	const int SERVER = 9;                 
	char IP[1][10] =  {"10.1.1.0"};
	char MASK[] = "255.255.255.0";

	
	
	
	
	/*
	 * Logs for console.
	 */
	NS_LOG_COMPONENT_DEFINE("WirelessNetwork2Devices");	
	LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO); //Displays information of the client on the console.
	LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO); //Displays information of the server on the console.

	
	
	
	/*
	 * Wireless Wifi network configuration
	 */
	YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
	YansWifiPhyHelper physicalLayer;
	WifiMacHelper mac;
	WifiHelper wifi;
	physicalLayer.SetChannel(channel.Create());
    Ssid ssid = Ssid("ns-3-ssid");// 802.11 service set identifier (SSID)
	
    
    
    
	/*
	 * Wireless Wifi network  general configuration
	 */
	NodeContainer nodes; //Actual nodes of the network.
	nodes.Create(WNETWORK1_DEVICES); //The nodes of the Wireless network 1 are created.
	
	
	NodeContainer wifiStaNode = nodes.Get(0); //Wireless Wifi network  1 (10.1.1.0) configuration 
	NodeContainer wifiApNode  = nodes.Get(1);
	

	NetDeviceContainer wifiNodesNetwork1Sta;
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    wifiNodesNetwork1Sta =  wifi.Install(physicalLayer,mac,wifiStaNode); 	//Assignation of mac address Node 0 (station node).

    
    NetDeviceContainer wifiNodeNetwork1Ap;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    wifiNodeNetwork1Ap =  wifi.Install(physicalLayer,mac,wifiApNode); //Assignation of mac address Node 1 (Acces point node).
    Ipv4GlobalRoutingHelper::PopulateRoutingTables(); 
     
    
    
    
    /*
     * Routing protocol.
     */	
    OlsrHelper olsr;
    Ipv4StaticRoutingHelper staticRouting;
    Ipv4ListRoutingHelper list;
    list.Add(staticRouting, 0);
    list.Add(olsr, 10);
	
    
    
    
    /*
	 * Mobility simulation.
	 */
	MobilityHelper mobility;
	mobility.SetPositionAllocator("ns3::GridPositionAllocator",
	                              "MinX", DoubleValue(4.0),
	                              "MinY", DoubleValue(4.0),
	                              "DeltaX", DoubleValue(10.0),
	                              "DeltaY", DoubleValue(10.0),
	                              "GridWidth", UintegerValue(10),
	                              "LayoutType", StringValue("RowFirst"));

	mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
	                          "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)));
	mobility.Install(nodes);


	
	
	/*
	 *Comunication protocol in the newtork
	*/
	InternetStackHelper stack;
	stack.Install(nodes);

	
	
	
	/*
	 * IP addressing
	 */	
	Ipv4AddressHelper address;
	address.SetBase(IP[0], MASK); //Both devices are in the same single network.
	Ipv4InterfaceContainer apInterface; 
	apInterface = address.Assign(wifiNodeNetwork1Ap);
	Ipv4InterfaceContainer staInterface;
	staInterface = address.Assign(wifiNodesNetwork1Sta);
	


	
	/*
	 *comunication parameters
	 */ 
	UdpEchoServerHelper echoServer(SERVER);
 
	//Server node configuration
	ApplicationContainer serverApps = echoServer.Install(nodes.Get(0)); //Server is runing in the station device. 
	serverApps.Start(Seconds(0.0));
	serverApps.Stop(Seconds(10.0));


	//Client  configuration
	UdpEchoClientHelper echoClient(staInterface.GetAddress(0),SERVER); //Interface to deliver the package 10.1.1.0.
	
	echoClient.SetAttribute("MaxPackets", UintegerValue(1));
	echoClient.SetAttribute("Interval", TimeValue(Seconds(0.1)));
	echoClient.SetAttribute("PacketSize", UintegerValue(1024));


	//Clien node configuration
	ApplicationContainer clientApps = echoClient.Install(nodes.Get(1)); //Client is runing in the Acces point device.
	clientApps.Start(Seconds(2.0));
	clientApps.Stop(Seconds(10.0));	
	
	
	
	
	
	/*
	 *NetSim animation
	 */
    AnimationInterface anim("WirelessNetwork2Devices.xml"); //For netanim visualization
	anim.EnablePacketMetadata(true); 
	anim.EnableIpv4RouteTracking("LTE-animation-WirelessNetwork2Devices.xml", Seconds(0), Seconds(5), Seconds(0.25));  //For routing visualzation 
	Simulator::Stop(Seconds(10.0));
	Simulator::Run();
	Simulator::Destroy();



	return(0);
}





