#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/onoff-application.h"
#include "ns3/netanim-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/aodv-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/olsr-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/v4traceroute-helper.h"

/*
    File: WirelessTopology.cpp
    Author: Miguel Verdi
    Description: This C++ program  uses ns3 library to simulate the communication between
    m adhoc devices, in a wireless topology as shown.

    Topology: 
  0
  |
  1 -- 2
  |    |
  3 -- 4 -- 5
       |    | 
       6 -- 7

  It does a trace route between node 0 and 7, previous IP and mac addressing and an on-off sending aplication that returns measurments.
*/

using namespace ns3;
uint32_t g_packetCount = 0; // Global variable to keep track of received packets


int
main()
{
	/*
	 * Constants and parameters inicialization.
	 */
	NS_LOG_COMPONENT_DEFINE("WirelessNetworknDevices");	
	
	
	const int SERVER = 9;  //Aplication port of the server 
	const int nNodes = 8; //Every node must be conected 
	const int CONECTIONS = 9; //The number of conections has to match the number of IPs and links in topology. 
	char IP[CONECTIONS][10] = { "10.1.1.0", "10.1.2.0", "10.1.3.0",
							"10.1.4.0", "10.1.5.0", "10.1.6.0",
							"10.1.7.0","10.1.8.0","10.1.9.0"};

	const int TOPOLOGY[CONECTIONS][2] = {
		{0,1},
		{1,2},{1,3},
		{2,4},{3,4},
		{4,5},{4,6},
		{6,7},{5,7}};

	char MASK[] = "255.255.255.0";
    
	/*
	 * Physical layer configuration.
	 */
	std::string dataRate = "1Mps"; 
	std::string packetSize = "10"; //In bytes 
	double  txPowerDbm = 23.0; //dBm
	double simTime = 20.0; //sec
	std::string phyMode("DsssRate1Mbps");
	
    	Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue(phyMode));

	/*
	 * Node configuration.
	 */
   	NodeContainer nodes; 
	NodeContainer c[CONECTIONS]; //Holds nodes in pairs.
  	nodes.Create(nNodes); //Holds all the nodes in the simulation

	/*
	 * Comunication wireless protocol configuration.
	 */
   	NetDeviceContainer devices[CONECTIONS];  
	YansWifiPhyHelper wifiPhy[CONECTIONS];
	WifiHelper wifi[CONECTIONS];
	YansWifiChannelHelper wifiChannel[CONECTIONS];
	WifiMacHelper wifiMac;

	wifiMac.SetType("ns3::AdhocWifiMac");

	/*
	 * Instalation of the configuration in each link of pair of nodes.
	 */
	for (int link = 0; link < CONECTIONS; link++) {

		c[link].Add(nodes.Get(TOPOLOGY[link][0])); //Adds each node based in the topology with some link
        	c[link].Add(nodes.Get(TOPOLOGY[link][1]));

		wifiChannel[link].SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel"); //Channel configuration
		wifiChannel[link].AddPropagationLoss("ns3::LogDistancePropagationLossModel",
	                               "Exponent", DoubleValue(3.0));

		wifi[link].SetStandard(WIFI_STANDARD_80211b); //Wifi standar
		wifi[link].SetRemoteStationManager("ns3::ConstantRateWifiManager",
	                            	 "DataMode",StringValue(phyMode),
	                             	 "ControlMode",StringValue(phyMode));	

		wifiPhy[link].SetChannel(wifiChannel[link].Create()); //Physical layer
		wifiPhy[link].SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
		wifiPhy[link].Set("TxPowerStart", DoubleValue(txPowerDbm));
   		wifiPhy[link].Set("TxPowerEnd", DoubleValue(txPowerDbm));

		devices[link] = wifi[link].Install(wifiPhy[link], wifiMac, c[link]); //Creating each conection
    	}

	/*
	 * Rouiting protocols (can change olsr,aodv,dsdv)
	 */
	OlsrHelper olsr;
	AodvHelper aodv;
        DsdvHelper dsdv;

	Ipv4StaticRoutingHelper staticRouting;
	Ipv4ListRoutingHelper list;
        InternetStackHelper stack;
	
	
	list.Add(aodv, 100);
	list.Add(staticRouting, 10);
	stack.SetRoutingHelper(list);
	stack.InstallAll();


        /*
	 * IP addressing. 
	 */
	Ipv4AddressHelper address[CONECTIONS];
	Ipv4InterfaceContainer interface[CONECTIONS];

	for (int link = 0; link < CONECTIONS; link++) {
        address[link].SetBase(IP[link], MASK); //Adressing two nodes to the same network.
		interface[link] = address[link].Assign(devices[link]); 
  	  }
	
 	/*
	 * Node mobility. 
	 */
	MobilityHelper mobility;
	mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",DoubleValue(10.0),
                                  "MinY", DoubleValue(10.0),
                                  "DeltaX",DoubleValue(5.0),
                                  "DeltaY",DoubleValue(2.0),
                                  "GridWidth",UintegerValue(5),
                                  "LayoutType",StringValue("RowFirst"));
	
   	mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(-100, 100, -100, 100)),
                              "Speed", StringValue("ns3::ConstantRandomVariable[Constant=5.0]"));
	mobility.InstallAll();
	

	/*
	 * Tracing simulation. 
	 */
	V4TraceRouteHelper traceroute("10.1.9.2"); // Objetive of the trace 
   	traceroute.SetAttribute("Verbose", BooleanValue(true));
    	ApplicationContainer tracing = traceroute.Install(nodes.Get(0)); //Node sending the packages
   	tracing.Start(Seconds(0));
   	tracing.Stop(Seconds(simTime));
  

	/*
	 * App protocol. 
  	 */
	OnOffHelper onOffHelper ("ns3::TcpSocketFactory", Address(InetSocketAddress("10.1.1.1",SERVER))); //Address of the node and por of node generating trafic.

	onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]")); //Time the app is on. 
	onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]")); //Time the app is off. 
	onOffHelper.SetAttribute ("DataRate",StringValue ("3Mbps")); //Sending rate. 
	onOffHelper.SetAttribute ("PacketSize",UintegerValue(10000));  //Packet size


	// Install application on source node
	ApplicationContainer appContainer = onOffHelper.Install(nodes.Get(0)); //Node that is sending the package. 
	appContainer.Start(Seconds(0.0));
	appContainer.Stop(Seconds(simTime)); 


	// Create  sink to receive  packets
	PacketSinkHelper sink ("ns3::TcpSocketFactory",Address(InetSocketAddress("10.1.1.1",SERVER))); //Addrress of the node you are reciving from
	ApplicationContainer sinkContainer = sink.Install(nodes.Get(7)); //Node that is reciving
	sinkContainer.Start(Seconds(1.0));
        sinkContainer.Stop(Seconds(simTime));


	/*
	 * Flow Monitor initialization
	 */
	Ptr<FlowMonitor> monitor;
	FlowMonitorHelper flowMonitor;
	monitor = flowMonitor.InstallAll();

	//For netanim visualization
	AnimationInterface anim("WirelessNetworknDevices.xml"); 

	Simulator::Stop(Seconds(simTime));
	Simulator::Run();

	/*
	 *Flow Monitor measurments
	 */
	monitor->SerializeToXmlFile("Wireless2DevicesMEASURMENTS.xml", true, true);  //File for netanim use
	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowMonitor.GetClassifier()); //Gets a pointer to the Ipv4 flow information.
	std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats(); //Gets a pointer to the statistics of the Flow monitor.
	std::map<FlowId, FlowMonitor::FlowStats>::const_iterator statsPointer ;


	for (statsPointer  = stats.begin(); statsPointer  != stats.end(); ++statsPointer ){
		Ipv4FlowClassifier::FiveTuple monitor = classifier->FindFlow(statsPointer ->first);
		//Getting stats from the flow monitor
		float PDR = 100*(statsPointer->second.rxPackets)/(statsPointer->second.txPackets); //Defined as 100*(Received packages/ transmitted packages).
		float transmitionTime = statsPointer->second.timeLastRxPacket.GetSeconds() - statsPointer ->second.timeFirstTxPacket.GetSeconds(); //Difference between first an last package.
		float packetSize = statsPointer -> second.rxBytes; //number of bytes received (size of the package)
		std::cout << "\n ----------------------------- \n";		
		std::cout << "Flow "  << " (" << monitor.sourceAddress << " -> " << monitor.destinationAddress << ")\n";
		std::cout << "\n --- Simulation results ----- \n";
		std::cout << "Throughput: " << 8*(packetSize)/(transmitionTime)/100000<< " Mbps\n";
		std::cout << "Latency: " << (statsPointer->second.delaySum)/(statsPointer->second.rxPackets)  << " \n"; //Divided by number of packages because is the mean value. 
		std::cout << "PDR: " << PDR  << "% \n";
		std::cout << "Tx Packages:   " << statsPointer ->second.txPackets << "\n";
		std::cout << "Rx Packages:   " << statsPointer ->second.rxPackets<< "\n";
		std::cout << "Lost packages:   " << statsPointer ->second.lostPackets << "\n";
		}
	
	Simulator::Destroy();
	return(0);
}





