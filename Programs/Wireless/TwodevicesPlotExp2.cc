#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
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
#include <unistd.h>
#include <fstream>
using namespace ns3;

/*
    File: WirelessTopology.cpp
    Author: Miguel Verdi
*/
class HighwayExperiment 
{
public:
  
  	bool Configure (int argc, char **argv);
	void openFile ();
	void runAll();

	uint32_t g_packetCount = 0; // Global variable to keep track of received packets
	int SERVER;  //Aplication port of the server 
	std::string dataRate;
	uint32_t packetSize; // In bytes
	uint32_t maxBytes;
	int maxPackages;
	std::string filename = "Experiment_2.csv";
	double  txPowerDbm; //dBm
	double simTime; //sec
	float senderStart;//sec
	float receiverStart;//sec
	float senderStop;//sec
	float receiverStop;//sec
	float vel1; //m/s
	float vel2; //m/s
	int channelWidth; //MHz
	std::string phyMode;
	void runExperiment(float gap);



};




int
main(int argc, char **argv)
{
    
	HighwayExperiment highwayExperiment;

	if (!highwayExperiment.Configure (argc, argv)){
    	NS_FATAL_ERROR ("Configuration failed. Aborted.");
	}
	

	highwayExperiment.Configure (argc, argv);

	float gap = 10.0; 
	highwayExperiment.vel1 = 30.0;
	highwayExperiment.vel2 = 35.0;
    std::ofstream outputFile(highwayExperiment.filename, std::ofstream::trunc); 	//std::ofstream outputFile(highwayExperiment.filename);

    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not create or open the file for writing." << std::endl;
        return 1;  // Exit with an error code
    }

	for(int i = 0; i < 9; i++){
		highwayExperiment.runExperiment(gap);
		gap = gap + 10;
	}

	

	return(0);
}

bool 
HighwayExperiment::Configure (int argc, char **argv)
{
	
    CommandLine cmd (__FILE__);
 	cmd.Parse (argc, argv);
	SERVER = 9;  //Aplication port of the server 
	dataRate = "80Mbps";
	packetSize = 500; // In bytes
	txPowerDbm = 35.0; //dBm
	channelWidth = 40; //MHz
	maxPackages = 100000; 
	maxBytes = packetSize*maxPackages;
	simTime = 5.0; //sec
	
	senderStart = 0.1;//sec
	receiverStart = 0.0;//sec

	senderStop = simTime;//sec
	receiverStop = simTime + 0.1;//sec
	
	phyMode = "VhtMcs6";


 	return true;

}


void 
HighwayExperiment::runExperiment (float gap){
	int nNodes = 2;
	int CONECTIONS(nNodes - 1);
	NodeContainer nodes; 
	NodeContainer c[CONECTIONS]; 
	nodes.Create(nNodes); 

	NetDeviceContainer devices[CONECTIONS];  
	YansWifiPhyHelper wifiPhy[CONECTIONS];
	WifiHelper wifi[CONECTIONS];
	YansWifiChannelHelper wifiChannel[CONECTIONS];
	WifiMacHelper wifiMac;
	wifiMac.SetType("ns3::AdhocWifiMac");


	for (int link = 0; link < CONECTIONS; link++) {

		c[link].Add(nodes.Get(link)); //Adds each node based in the topology with some link
		c[link].Add(nodes.Get(link + 1));

		wifiChannel[link].SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel"); //Channel configuration
		wifiChannel[link].AddPropagationLoss("ns3::LogDistancePropagationLossModel", "Exponent", DoubleValue(3.0));

		wifi[link].SetStandard(WIFI_STANDARD_80211ac); //Wifi standar
		wifi[link].SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",StringValue(phyMode),"ControlMode",StringValue(phyMode));	

		wifiPhy[link].SetChannel(wifiChannel[link].Create()); //Physical layer
		wifiPhy[link].SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
		wifiPhy[link].Set("TxPowerStart", DoubleValue(txPowerDbm));
		wifiPhy[link].Set("TxPowerEnd", DoubleValue(txPowerDbm));

		devices[link] = wifi[link].Install(wifiPhy[link], wifiMac, c[link]); //Creating each conection
	}
	//int channelWidth = 40; 
	Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue (channelWidth));
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


	Ipv4AddressHelper address[CONECTIONS];
	Ipv4InterfaceContainer interface[CONECTIONS];
	ns3::Ipv4Address ipv4Address;


	for (int link = 0; link < CONECTIONS; link++) {
		address[link].SetBase("10.1.1.0", "255.255.255.0"); //Adressing two nodes to the same network.
		interface[link] = address[link].Assign(devices[link]); 
	}


	MobilityHelper mobility;
	mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();



	positionAlloc->Add(Vector(0.0,0.0, 0.0)); 
	positionAlloc->Add(Vector(gap,0.0, 0.0)); 
	


	mobility.SetPositionAllocator(positionAlloc);
	mobility.Install (nodes);


	
	

	Ptr<ConstantVelocityMobilityModel> mob1 = nodes.Get(0) -> GetObject<ConstantVelocityMobilityModel>();
	mob1->SetVelocity(Vector(vel1, 0, 0));
	
	Ptr<ConstantVelocityMobilityModel> mob2 = nodes.Get(1) -> GetObject<ConstantVelocityMobilityModel>();
	mob2->SetVelocity(Vector(vel2, 0, 0));


	OnOffHelper onOffHelper("ns3::UdpSocketFactory", Address());
	onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));





	onOffHelper.SetAttribute("DataRate", StringValue(dataRate));
	onOffHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
	onOffHelper.SetAttribute("MaxBytes", UintegerValue(maxBytes));	


	AddressValue remoteAddress(InetSocketAddress(interface[CONECTIONS - 1].GetAddress(1), SERVER));  //Objetive of the last node
	onOffHelper.SetAttribute("Remote", remoteAddress);
	ApplicationContainer senderApp = onOffHelper.Install(nodes.Get(0)); //Sender is the first node


	senderApp.Start(Seconds(senderStart));
	senderApp.Stop(Seconds(senderStop));


	PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", Address());
	sinkHelper.SetAttribute("Local", AddressValue(InetSocketAddress(Ipv4Address::GetAny(), SERVER)));
	ApplicationContainer receiverApp = sinkHelper.Install(nodes.Get(nNodes - 1)); //Reciver is the las node

	receiverApp.Start(Seconds(receiverStart));
	receiverApp.Stop(Seconds(receiverStop));



	Ptr<FlowMonitor> monitor;
	FlowMonitorHelper flowMonitor;
	monitor = flowMonitor.InstallAll();

	Simulator::Stop(Seconds(simTime));
	Simulator::Run();
	monitor->SerializeToXmlFile("MEASURMENTS.xml", true, true);  //File for netanim use



	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowMonitor.GetClassifier()); //Gets a pointer to the Ipv4 flow information.

	std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats(); //Gets a pointer to the statistics of the Flow monitor.
	std::map<FlowId, FlowMonitor::FlowStats>::const_iterator statsPointer ;

	std::ofstream outputFile(filename, std::ofstream::app);
	for (statsPointer  = stats.begin(); statsPointer  != stats.end(); ++statsPointer ){

	Ipv4FlowClassifier::FiveTuple monitor = classifier->FindFlow(statsPointer ->first);
	//Getting stats from the flow monitor
		if ((monitor.sourceAddress== interface[0].GetAddress(0) && monitor.destinationAddress == interface[CONECTIONS - 1].GetAddress(1))){
			std::cout<<  "================= Flow sim results ================= "  << std::endl;
			
			float PDR = 100*(statsPointer->second.rxPackets)/(statsPointer->second.txPackets); //Defined as 100*(Received packages/ transmitted packages).
			float transmitionTime = statsPointer->second.timeLastRxPacket.GetSeconds() - statsPointer ->second.timeFirstTxPacket.GetSeconds(); //Difference between first an last package.
			float totRecPackets = statsPointer -> second.rxPackets; 
			float throughput = (8*totRecPackets*packetSize)/(transmitionTime*1000000);

			std::cout << "\n ----------------------------- \n";		
			std::cout << "Flow "  << " (" << monitor.sourceAddress << " -> " << monitor.destinationAddress << ")\n";
			std::cout << "\n --- Simulation results ----- \n";
			std::cout << "Throughput: " << (8*totRecPackets*packetSize)/(transmitionTime*1000000) << " Mbps\n";
			std::cout << "Latency: " << (statsPointer->second.delaySum)/(statsPointer->second.rxPackets)  << " \n"; //Divided by number of packages because is the mean value. 
			std::cout << "PDR: " << PDR  << "% \n";
			std::cout << "Tx Packages:   " << statsPointer ->second.txPackets << "\n";
			std::cout << "Rx Packages:   " << statsPointer ->second.rxPackets<< "\n";
			std::cout << "Lost packages:   " << statsPointer ->second.lostPackets << "\n";
			std::cout << "Distance:   " << gap << "\n";
			std::cout << "vel 1:   " << vel1 << " vel 2:  " << vel2  << "\n";

			outputFile << gap << ","<< throughput << ","<< PDR << std::endl;
			outputFile.close();
		}
	}




	Simulator::Destroy();


		

	}

    
	
