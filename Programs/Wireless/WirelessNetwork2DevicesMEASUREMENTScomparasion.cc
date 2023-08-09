
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

NS_LOG_COMPONENT_DEFINE("HelloSimulator");

using namespace ns3;
// Global variable to keep track of received packets
uint32_t g_packetCount = 0;



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
	double  txPowerDbm = 23.0; //dBm
	
	
	
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
	//YansWifiChannelHelper channel = wifiPhy; 
	//YansWifiChannelHelper channel;
	

	YansWifiPhyHelper physicalLayer;
	WifiMacHelper mac;
	WifiHelper wifi;
	
	//Reference of config:  https://www.nsnam.org/docs/models/html/wifi-user.html
	wifi.SetStandard(WIFI_STANDARD_80211a); //This change the Bandwith parameter. 

      
	Ssid ssid = Ssid("ns-3-ssid");// 802.11 service set identifier (SSID)
	
	
    physicalLayer.Set ("TxPowerLevels", UintegerValue(1));
    physicalLayer.Set("TxPowerStart", DoubleValue(txPowerDbm)); // Set the start and end transmission power
    physicalLayer.Set("TxPowerEnd", DoubleValue(txPowerDbm));
	physicalLayer.SetChannel(channel.Create());
	channel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");

 
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
	 */
    
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(1.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install(nodes);


	Ptr<MobilityModel> mobilityModelA = nodes.Get(0)->GetObject<MobilityModel>();
	Ptr<MobilityModel> mobilityModelB = nodes.Get(1)->GetObject<MobilityModel>();
	
	
	//Get the constant distance of the model. 
	float x1 = mobilityModelA->GetPosition().x - mobilityModelB->GetPosition().x; 
	float y1 = mobilityModelA->GetPosition().y - mobilityModelB->GetPosition().y;
	float d = sqrt(pow(x1,2) + pow(y1,2));
	
	
	//PropagationLossModel model();
	// The reference loss of 46.6777 dB was calculated using Friis propagation loss model at 5.15 GHz.
	Ptr<PropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel>();
		
	
	//Chanel capacity calculation
	double rxPowerDbm = lossModel->CalcRxPower(txPowerDbm,mobilityModelA,mobilityModelB);
	double pathLoss = txPowerDbm -rxPowerDbm; 
	double txPowerW = pow(10,txPowerDbm/10); //converting to W
	double noise = pow(10,-90/10); //Converting -90db to W (noise value). 
	double SNR = txPowerW/noise;  //This is in W
	double chCapacity = 20*log2(1 + SNR); //This is in Mhz
	
	
	
	
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
	

	// Install OnOff application on node 0

	/*
	OnOffHelper onOff("ns3::TcpSocketFactory", Address(InetSocketAddress(apInterface.GetAddress(0), SERVER)));
	onOff.SetAttribute("PacketSize", UintegerValue(1024));
	onOff.SetAttribute("MaxPackets", UintegerValue(1));
	onOff.SetAttribute("Interval", TimeValue(Seconds(1)));


	ApplicationContainer apps = onOff.Install(wifiStaNode.Get(0));
	apps.Start(Seconds(1.0));
	apps.Stop(Seconds(10.0)); // Set the duration of the OnOff application

	*/

	/*
	 *comunication parameters
*/
	//Server node configuration


	UdpEchoServerHelper server(SERVER);
	ApplicationContainer serverApps = server.Install(nodes.Get(0)); //Server is runing in the station device.
	serverApps.Start(Seconds(0.0));
	serverApps.Stop(Seconds(10.0));

	//Client  configuration
	UdpEchoClientHelper echoClient(staInterface.GetAddress(0),SERVER); //Interface to deliver the package 10.1.1.0.
	
	echoClient.SetAttribute("MaxPackets", UintegerValue(1));
	echoClient.SetAttribute("Interval", TimeValue(Seconds(1)));
	echoClient.SetAttribute("PacketSize", UintegerValue(1));


	//Clien node configuration
	ApplicationContainer clientApps = echoClient.Install(nodes.Get(1)); //Client is runing in the Acces point device.
	clientApps.Start(Seconds(1.0));
	clientApps.Stop(Seconds(10.0));	

	




	/*
	 *Flow Monitor initialization
	 */
	Ptr<FlowMonitor> monitor;
	FlowMonitorHelper flowMonitor;
	monitor = flowMonitor.InstallAll();



	/*
	 *NetSim animation
	 */
    AnimationInterface anim("WirelessNetwork2Devices.xml"); //For netanim visualization
	anim.EnablePacketMetadata(true); 



	Simulator::Stop(Seconds(10.0));
	Simulator::Run();

	
	/*
	 *Flow Monitor printing results
	 *Note that is just works with Ipv4 protocol.
     */

	monitor->SerializeToXmlFile("Wireless2DevicesMEASURMENTS.xml", true, true);  //File for netanim use


	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowMonitor.GetClassifier()); //Gets a pointer to the Ipv4 flow information.
	
	std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats(); //Gets a pointer to the statistics of the Flow monitor.
	std::map<FlowId, FlowMonitor::FlowStats>::const_iterator statsPointer ;


	for (statsPointer  = stats.begin(); statsPointer  != stats.end(); ++statsPointer ){

		Ipv4FlowClassifier::FiveTuple monitor = classifier->FindFlow(statsPointer ->first);

		//Monitor of the communication between the 2 end devices.

		if ((monitor.sourceAddress== "10.1.1.2" && monitor.destinationAddress == "10.1.1.1")){

			//Getting stats from the flow monitor
			float PDR = 100*(statsPointer->second.rxPackets)/(statsPointer->second.txPackets); //Defined as 100*(Received packages/ transmitted packages).
			float transmitionTime = statsPointer->second.timeLastRxPacket.GetSeconds() - statsPointer ->second.timeFirstTxPacket.GetSeconds(); //Difference between first an last package.
			float packetSize = statsPointer -> second.rxBytes; //number of bytes received (size of the package)
			double conv = 1.0/125000.0; //Conversion constant bytes per second to Mbps.


			std::cout << "\n ----------------------------- \n";
				
			std::cout << "Flow "  << " (" << monitor.sourceAddress << " -> " << monitor.destinationAddress << ")\n";
			
			std::cout << "\n -----------Parameters -------- \n";
			std::cout << "Distance :  " << d << " m \n";
			std::cout << "Tx Power:  " <<txPowerDbm << " dBm \n";	
						
						
						
			
			std::cout << "\n -----------Theorical results-------- \n";
			
			
			std::cout << "Rx Power:  " <<rxPowerDbm << " dBm \n";
			std::cout << "path loss:  " <<pathLoss  << "dBm \n";
			std::cout << "Channel capacity:  " << chCapacity << " Mbps\n";
			
			
			std::cout << "\n --- Simulation results ----- \n";
			
			std::cout << "Throughput: " << conv*(packetSize)/(transmitionTime)<< " Mbps\n";
			std::cout << "Latency: " << (statsPointer->second.delaySum)/(statsPointer->second.rxPackets)  << " \n"; //Divided by number of packages because is the mean value. 
			std::cout << "PDR: " << PDR  << "% \n";
			std::cout << "Tx Packages:   " << statsPointer ->second.txPackets << "\n";
			std::cout << "Rx Packages:   " << statsPointer ->second.rxPackets<< "\n";
			std::cout << "Lost packages:   " << statsPointer ->second.lostPackets << "\n";

			}
		}



	
	
	Simulator::Destroy();





	return(0);
}





