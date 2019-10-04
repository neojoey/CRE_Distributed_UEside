/*
 * To do
 * 
 * 1. set the pico enb tx power 
 * 2. check the interference 
 * 3. ue distribution - hotspot & uniform
 */



/* LTE, EPC
 * [remotesForUes] ------p2p------ [PGW] ------p2p----- [eNB] - - wireless - - [UEs]
 *
 * multiple remotes & UEs
 */

#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
//#include "ns3/wifi-module.h"
#include "ns3/athstats-helper.h"
#include "ns3/lte-module.h"
#include "ns3/rr-ff-mac-scheduler.h"

#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/gnuplot.h"


#include <iostream>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <vector>
#include <math.h>
#include <time.h>

#define ANIMATION
#define PICOCELL
#define PHI 3.1415926535

using namespace ns3;

#if 0
template <typename T>

void NewValueTracerIntoStream (std::ofstream *ofs, T oldValue, T newValue) {
	*ofs << Simulator::Now ().GetSeconds () << '\t' << newValue <<std::endl;
}
void NewTimeValueTracerIntoStream (std::ofstream *ofs, Time oldValue, Time newValue){
	*ofs << Simulator::Now ().GetSeconds () << '\t' << newValue.GetSeconds () << std::endl;
}
void DoBulkSendApplicationTcpTrace (Ptr<BulkSendApplication> application, std::string traceFilePrefix, Time traceEnd) {
	using namespace std;
	Ptr<TcpSocketBase> socket = application->GetSocket ()->GetObject<TcpSocketBase> ();

	ofstream *cwndFile = new ofstream ((traceFilePrefix + ".cwnd").c_str());
	socket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&NewValueTracerIntoStream<uint32_t>, cwndFile));
	Simulator::Schedule (traceEnd, &ofstream::close, cwndFile);

	ofstream *rttFile = new ofstream ((traceFilePrefix + ".rtt").c_str());
	socket->TraceConnectWithoutContext ("RTT", MakeBoundCallback (&NewTimeValueTracerIntoStream, rttFile));
	Simulator::Schedule (traceEnd, &ofstream::close, rttFile);
}
void BulkSendApplicationTcpTrace ( //It's not subordinate to BulkSendApplication, just to TCP
		Ptr<BulkSendApplication> application, std::string traceFilePrefix,
		Time traceStart, Time traceEnd) {
	Simulator::Schedule (traceStart + NanoSeconds (1), &DoBulkSendApplicationTcpTrace,
			application, traceFilePrefix, traceEnd);
}
#endif

std::map <uint64_t, uint32_t> handoverCount;

void UpdateHandoverCount ( uint64_t imsi )
{
	std::map <uint64_t, uint32_t>::iterator it = handoverCount.find (imsi);
	if ( it != handoverCount.end () ) {
		it->second++;
	} else {
		handoverCount.insert (std::make_pair ( imsi, 1 ) );
	}
}

void PrintHandoverCount ()
{
	for (std::map <uint64_t, uint32_t>::iterator it = handoverCount.begin (); it != handoverCount.end (); ++it ) 
	{
		std::cout << "IMSI : " << it->first << " H.O Count : " << it->second << std::endl;
	}
}

const std::string CurrentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y%m%d.%H%M", &tstruct);

	return buf;
}

#if 1
void 
NotifyConnectionEstablishedUe (std::string context, 
                               uint64_t imsi, 
                               uint16_t cellid, 
                               uint16_t rnti)
{
  std::cout << context 
            << " UE IMSI " << imsi 
            << ": connected to CellId " << cellid 
            << " with RNTI " << rnti 
            << std::endl;
}

void 
NotifyHandoverStartUe (std::string context, 
                       uint64_t imsi, 
                       uint16_t cellid, 
                       uint16_t rnti, 
                       uint16_t targetCellId)
{
  std::cout << context 
            << " UE IMSI " << imsi 
            << ": previously connected to CellId " << cellid 
            << " with RNTI " << rnti 
            << ", doing handover to CellId " << targetCellId 
            << std::endl;
}

void 
NotifyHandoverEndOkUe (std::string context, 
                       uint64_t imsi, 
                       uint16_t cellid, 
                       uint16_t rnti)
{
  std::cout << context 
            << " UE IMSI " << imsi 
            << ": successful handover to CellId " << cellid 
            << " with RNTI " << rnti 
            << std::endl;
	UpdateHandoverCount ( imsi );
}

void 
NotifyConnectionEstablishedEnb (std::string context, 
                                uint64_t imsi, 
                                uint16_t cellid, 
                                uint16_t rnti)
{
  std::cout << context 
            << " eNB CellId " << cellid 
            << ": successful connection of UE with IMSI " << imsi 
            << " RNTI " << rnti 
            << std::endl;
}

void 
NotifyHandoverStartEnb (std::string context, 
                        uint64_t imsi, 
                        uint16_t cellid, 
                        uint16_t rnti, 
                        uint16_t targetCellId)
{
  std::cout << context 
            << " eNB CellId " << cellid 
            << ": start handover of UE with IMSI " << imsi 
            << " RNTI " << rnti 
            << " to CellId " << targetCellId 
            << std::endl;
}

void 
NotifyHandoverEndOkEnb (std::string context, 
                        uint64_t imsi, 
                        uint16_t cellid, 
                        uint16_t rnti)
{
  std::cout << context 
            << " eNB CellId " << cellid 
            << ": completed handover of UE with IMSI " << imsi 
            << " RNTI " << rnti 
            << std::endl;
}
 
/*
static void 
CourseChange (std::string foo, Ptr<const MobilityModel> mobility)
{
  Vector pos = mobility->GetPosition ();
  Vector vel = mobility->GetVelocity ();
  std::cout << Simulator::Now () << ", model=" << mobility << ", POS: x=" << pos.x << ", y=" << pos.y
            << ", z=" << pos.z << "; VEL:" << vel.x << ", y=" << vel.y
            << ", z=" << vel.z << std::endl;
}
*/

#endif

static ns3::GlobalValue g_macroEnbTxPowerDbm ("macroEnbTxPowerDbm",
																							"Tx power [dBm] used by macro eNBs",
																							ns3::DoubleValue (46.0),
																							ns3::MakeDoubleChecker<double> ());
#if 1
double DegreeToRadian (double degree)
{
	return (degree * PHI/180.0);

}

Vector CalcInitPicoPosition ( int n, double d, Vector center ) 
{
	Vector v;
	double degree = 90 - (360/n/2);
	//std::cout << degree << std::endl;
	v.x = floor (d * cos (DegreeToRadian (degree))) + center.x;
	v.y = floor (d * sin (DegreeToRadian (degree))) + center.y;
	v.z = 0;
	return v;
}

void SetUniformPicoPosition ( NodeContainer &c, Vector center, Vector initPicoPosition, double distPico, int nPico )
{
	Ptr<ListPositionAllocator> positionEnb = CreateObject<ListPositionAllocator> ();
	positionEnb->Add (center);	// for macro Enb
	Vector pos = initPicoPosition;
	double degree = atan2 ( (double)(pos.y-center.y),(double)(pos.x-center.x)) * 180 / PHI; 

	for (uint16_t i = 1; i < c.GetN (); i++ ) // except for macro
	{
		positionEnb->Add (pos);
		degree = degree + 360.0/nPico;
		pos.x = floor (distPico * cos (DegreeToRadian (degree))) + center.x;
		pos.y = floor (distPico * sin (DegreeToRadian (degree))) + center.y;
	}
	MobilityHelper mobilityEnb;
	mobilityEnb.SetPositionAllocator (positionEnb);
	mobilityEnb.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobilityEnb.Install (c);
}

void Sort (double new_array[], int num)
{
	for (int x=0; x<num; x++) {
		for (int y=0; y<num-1; y++) {
			if (new_array[y]>new_array[y+1]) {
				double temp = new_array[y+1];
				new_array[y+1] = new_array[y];
				new_array[y] = temp;
			}
		}
	}
	std::cout << "\nSorted List: \n";
	for (int i = 0; i<num; i++) {
		std::cout << new_array[i] << "\n";
	}
	std::cout << "\n";
	std::cout << "Count: " << num << std::endl;
}
#endif

#if 1
void PlotThroughputCdf ( double data_array1[], double data_array2[], int num, uint16_t absInterval, double simTime, std::string now )
{
	std::stringstream ss1;
	ss1 << absInterval;
	std::string condAbs = ss1.str();
	std::stringstream ss2;
	ss2 << simTime;
	std::string condTime = ss2.str();

	// x Range set to maximum value + 30 automatically
	std::stringstream ss3;
	ss3 << data_array1[num-1]+30;
	std::string maxRange = ss3.str();
	std::string xRange = "set xrange [0: " + maxRange + "]";

	std::string fileNameWithNoExtension = "DistUeBias_Throughput-CDF_i"+condAbs+"_t"+condTime+"_"+now;
	std::string plotFileName = fileNameWithNoExtension + ".plt";
	std::string graphicsFileName = fileNameWithNoExtension + ".png";
	std::string plotTitle = "Throughput CDF";
	std::string dataTitle = "Throughput Data";

	Gnuplot plot (graphicsFileName);
	plot.SetTitle (plotTitle);
	plot.SetTerminal ("png");
	plot.SetLegend ("Throughput (Kbps)", "CDF");
	//plot.AppendExtra ("set xrange [0: 400]");
	plot.AppendExtra (xRange);

	Gnuplot2dDataset dataset;
	dataset.SetTitle (dataTitle);
	dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	for ( int i=0; i<num; i++ ) {
		dataset.Add( data_array1[i], data_array2[i] );
	}

	plot.AddDataset (dataset);
	std::ofstream plotFile (plotFileName.c_str ());
	plot.GenerateOutput (plotFile);
	plotFile.close ();
}

void PlotDelay ( double data_array1[], int num, uint16_t absInterval, double simTime, std::string now)
{
	std::stringstream ss1;
	ss1 << absInterval;
	std::string condAbs = ss1.str();
	std::stringstream ss2;
	ss2 << simTime;
	std::string condTime= ss2.str();

	std::string fileNameWithNoExtension = "DistUeBias_Delay_i"+condAbs+"_t"+condTime+"_"+now;
	std::string graphicsFileName = fileNameWithNoExtension + ".png";
	std::string plotFileName = fileNameWithNoExtension + ".plt";
	std::string plotTitle = "Mean Delay";
	std::string dataTitle = "Delay Data";

	Gnuplot plot (graphicsFileName);
	plot.SetTitle (plotTitle);
	plot.SetTerminal ("png");
	plot.SetLegend ("UEs", "Delay (Sec)");
	plot.AppendExtra ("set xrange [0: 100]");

	Gnuplot2dDataset dataset;
	dataset.SetTitle (dataTitle);
	dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	for ( int i=0; i<num; i++ ) {
		dataset.Add( i, data_array1[i]);
	}

	plot.AddDataset (dataset);
	std::ofstream plotFile (plotFileName.c_str ());
	plot.GenerateOutput (plotFile);
	plotFile.close ();
}

void PlotHOCdf( double data_array1[], double data_array2[], int num, uint16_t absInterval, double simTime, std::string now )
{
	std::stringstream ss1;
	ss1 << absInterval;
	std::string condAbs = ss1.str();
	std::stringstream ss2;
	ss2 << simTime;
	std::string condTime= ss2.str();
	
	// x Range set to maximum value + 30 automatically
	std::stringstream ss3;
	ss3 << data_array1[num-1];
	std::string maxRange = ss3.str();

	std::string xRange = "set xrange [0: " + maxRange + " ]";

	std::string fileNameWithNoExtension = "DistUeBias_HandoverCount-cdf_i"+condAbs+"_t"+condTime+"_"+now;
	std::string plotFileName = fileNameWithNoExtension + ".plt";
	std::string graphicsFileName = fileNameWithNoExtension + ".png";
	std::string plotTitle = "Handover Frequency CDF";
	std::string dataTitle = "Handover Count Data";

	Gnuplot plot (graphicsFileName);
	plot.SetTitle (plotTitle);
	plot.SetTerminal ("png");
	plot.SetLegend ("Handover Frequency", "CDF");
	//plot.AppendExtra ("set xrange [0: 100]");
	plot.AppendExtra (xRange);

	Gnuplot2dDataset dataset;
	dataset.SetTitle (dataTitle);
	dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	for ( int i=0; i<num; i++ ) {
		dataset.Add( data_array1[i], data_array2[i] );
	}

	plot.AddDataset (dataset);
	std::ofstream plotFile (plotFileName.c_str ());
	plot.GenerateOutput (plotFile);
	plotFile.close ();
}


void JainFairnessIndex (double new_array[], int num, double *jainIdx)
{
	//GET TOTAL & CALCULATE Jain Fairness Index
	double total1 = 0;
	double total2 = 0;
	double JFI = 0;
	
	for(int i=0;i<num; i++){
		total1 += new_array[i];
		total2 += pow (new_array[i], 2);
	}
	JFI = pow(total1,2) / (num*total2);
	*jainIdx = JFI;	
	std::cout << "The JainFairnessIndex is " << JFI << std::endl;
}


void Median_Q1Q3 (double new_array[], int num, double *median, double *q1, double *q3)
{
	int mid;
	//CALCULATE THE MEDIAN (middle number)
	if (num % 2 != 0){// is the # of elements odd?
		int temp = ((num+1)/2)-1;
		//std::cout << "The median is " << new_array[temp] << std::endl;
		mid = temp + 1;
	}
	else {// then it's even! :)
		//double median = (new_array[(num/2)-1] + new_array[num/2]) / 2;
		//std::cout << "The median is "<< new_array[(num/2)-1] << " and " << new_array[num/2] << std::endl;
		//std::cout << "The median is "<< median << std::endl;
		mid = num/2;
	}
	
	if (mid % 2 != 0) {// is the # of elements odd?
		int tmp_Q1 = ((mid+1)/2)-1;
		std::cout << "The Q1 is " << new_array[tmp_Q1] << std::endl;
		*q1 = new_array[tmp_Q1];
		
		if(num % 2 != 0) { // is the # of elements odd?
			std::cout << "The median is " << new_array[((num+1)/2)-1] << std::endl;
			*median = new_array[((num+1)/2)-1] ;
		}
		else { // even
			std::cout << "The median is "<< (new_array[(num/2)-1] + new_array[num/2]) / 2 << std::endl;
			*median = (new_array[(num/2)-1] + new_array[num/2]) / 2;
		}
		int third = (tmp_Q1 + 1) + mid; 
		std::cout << "The Q3 is " << new_array[third-1] << std::endl;
		*q3 = new_array[third-1];
	}
	else {// then it's even! :)
		double Q1 = (new_array[(mid/2)-1] + new_array[mid/2]) / 2;
		//std::cout << "The Q1 is " << new_array[(mid/2)-1] << " and " << new_array[mid/2] << std::endl;
		std::cout << "The Q1 is " << Q1 << std::endl;
		*q1 = Q1;
				
		if(num % 2 != 0) { // is the # of elements odd?
			std::cout << "The median is " << new_array[((num+1)/2)-1] << std::endl;
			*median = new_array[((num+1)/2)-1]; 
		}
		else { // even
			std::cout << "The median is "<< (new_array[(num/2)-1] + new_array[num/2]) / 2 << std::endl;
			*median = (new_array[(num/2)-1] + new_array[num/2]) / 2;
		}
			
		int third = mid/2 + mid;
		double Q3 = (new_array[third-1] + new_array[third]) / 2;
		//std::cout << "The Q3 is " << new_array[third - 1] << " and " << new_array[third] << std::endl;
		std::cout << "The Q3 is " << Q3 << std::endl;
		*q3 = Q3;
	}
}

void Mean (double new_array[], int num, double * mean)
{
	//GET TOTAL & CALCULATE MEAN
	double total = 0;
	//double mean = 0;
	for(int i=0;i<num; i++){
		total += new_array[i];
	}
	*mean = total / (double) num;
	std::cout << "The mean is " << *mean << std::endl;
}

void StdDev (double new_array[], int num, double *positiveStdDev, double *negativeStdDev)
{
	double mean = 0;
	double positive_stdDev = 0;
	double negative_stdDev = 0;
	int j = 0;
	int k = 0;;

	Mean (new_array, num, &mean);
	for(int i=0;i<num; i++){ 
		if (new_array[i] >= mean) {
			positive_stdDev += pow(new_array[i] - mean, 2);
			j++;
		} else {
			negative_stdDev += pow(new_array[i] - mean, 2);
			k++;
		}
	}
	positive_stdDev /= j;
	negative_stdDev /= k;
	positive_stdDev = sqrt(positive_stdDev);
	negative_stdDev = sqrt(negative_stdDev);
	*positiveStdDev = positive_stdDev;
	*negativeStdDev = negative_stdDev;
	std::cout << "The positive stdDev is " << positive_stdDev << std::endl;
	std::cout << "The negative stdDev is " << negative_stdDev << std::endl;
}

void Minimum (double new_array[], int num, double *minimum)
{
	std::cout << "The minimum is " << new_array[0] << std::endl;
	*minimum = new_array[0];
}

void Maximum (double new_array[], int num, double *maximum)
{
	std::cout << "The maximum is " << new_array[num-1] << std::endl;
	*maximum = new_array[num-1];
}



/*
void PlotHO( double data_array1[], int num, uint16_t absInterval, double simTime)
{
	std::stringstream ss1;
	ss1 << absInterval;
	std::string condAbs = ss1.str();
	std::stringstream ss2;
	ss2 << simTime;
	std::string condTime= ss2.str();

	std::string fileNameWithNoExtension = "hoCount_i"+condAbs+"_t"+condTime;
	std::string graphicsFileName = fileNameWithNoExtension + ".png";
	std::string plotFileName = fileNameWithNoExtension + ".plt";
	std::string plotTitle = "Handover Frequency";
	std::string dataTitle = "Handover Count";

	Gnuplot plot (graphicsFileName);
	plot.SetTitle (plotTitle);
	plot.SetTerminal ("png");
	plot.SetLegend ("UEs", "Counts");
	plot.AppendExtra ("set xrange [0: 100]");

	Gnuplot2dDataset dataset;
	dataset.SetTitle (dataTitle);
	dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	for ( int i=0; i<num; i++ ) {
		dataset.Add( i, data_array1[i]);
	}

	plot.AddDataset (dataset);
	std::ofstream plotFile (plotFileName.c_str ());
	plot.GenerateOutput (plotFile);
	plotFile.close ();
}
*/

#endif

NS_LOG_COMPONENT_DEFINE ("Simple");

int main (int argc, char *argv[])
{
	LogComponentEnable ("Simple", LOG_LEVEL_INFO);

	//std::string scenarioType = "UNIFORM";	// UNIFORM & HOTSPOT
	std::string scenarioType = "HOTSPOT";	// UNIFORM & HOTSPOT

	double simTime = 60.0;
	int nUes = 100;
	int packetSize = 300; //bytes
	int rndSeed = 1;
	int rndRun= 1;
#ifdef PICOCELL
#if 0
	Vector cellCenter = Vector( 512.0, 512.0, 0.0 );
	double cellRadius = 512.0;
#else
	Vector cellCenter = Vector( 256.0, 256.0, 0.0 );
	double cellRadius = 256.0;
#endif
	double distancePico = cellRadius*3/4;
	uint32_t nPicoEnb = 6;	//one of value 3, 6, 9, 12
	double picoTx = 30;
	double picoVariance = 2.0;
	double picoRadius = cellRadius*1/4;
	//double picoRadius = cellRadius*2/5;
//	Vector tmpInitialPosition = Vector (0, distancePico, 0);	// Pico eNBs locate at 5/8 distance from macro eNB




#endif

	//generates a random data rate per each user
	// 10Mhz & 10% guard band
	// maximum occupied bandwidth = 9Mhz
	// 9MHz/180kHz = 50 RBs
	// 1RB = 12*7 symbols/0.5ms
	// 50 RBs = 50 * 168 symbols/1ms = 8.4 * 10^6 symbols/sec
	int nRB = 50;

	double macroTx = 46;


	//double meanTrafficVolume = capacity * offeredLoad * applicationDuration / nUes;
	//double meanPacketInterval = applicationDuration / (meanTrafficVolume / 8 / packetSize); // sec

	/* for a further development
	double maxDelay = 0.04; // 40ms -> sec
	double viloationProb = 0.02; // delay violation probability
	*/


	//double distance = 50.0;

#if 1
	//Abs parameters
	uint16_t absPeriod = 1;
	//uint16_t absInterval = 8;
	uint16_t absInterval = 2;
#endif 

#if 1
	//Bias CRE values
	double rpB = 0;		//rsrp -> dBm
	double rqB = 0;	//rsrq -> dB
	//double rqB = rpB;	//rsrq -> dB
#endif
	
	double offeredLoad = 0.5;

	CommandLine cmd;
	cmd.AddValue ("nUes", "number of Ues", nUes);
	cmd.AddValue ("Load", "offered Load to Netwok", offeredLoad);
	cmd.AddValue ("nPicoEnb", "number of Pico eNB", nPicoEnb);
	cmd.AddValue ("scenarioType", "scenario Type(UNIFORM/HOTSPOT)", scenarioType);
	cmd.AddValue ("picoVariance", "variance of the # of UEs for Pico HOTSPOT scenario", picoVariance);
	cmd.AddValue ("absInterval", "cycle of ABS", absInterval);
	cmd.AddValue ("rpB", "bias value of CRE ", rpB);
	cmd.AddValue ("simTime", "simulation Time ", simTime);
	cmd.AddValue ("rndSeed", "Random Seed", rndSeed);
	cmd.AddValue ("rndRun", "Random substream run", rndRun);
	cmd.Parse (argc, argv);


	double applicationStart = 0.5;
	//double applicationEnd = 2.5;
	double applicationEnd = simTime;
	double simulationEnd = applicationEnd + 1.0;
#if 1
	double applicationDuration = applicationEnd - applicationStart;
	double capacity = 168 * 1000 * 4 * nRB; //Mbps; 168 symbols/1ms * 4 bits(16QAM) * # of RBs

	//std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA :: " << nUes << std::endl;
#endif


	//Trace
	std::string traceFilePrefix = "trace";
	std::string traceFilePrefixLte;

	//AMC
	//Config::SetDefault ("ns3::LteAmc::AmcModel", EnumValue (LteAmc::PiroEW2010));
	Config::SetDefault ("ns3::LteAmc::AmcModel", EnumValue (LteAmc::MiErrorModel));
	Config::SetDefault ("ns3::LteAmc::Ber", DoubleValue (0.000005));
	Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (160));

	//Auto Neighbour Relation
	Config::SetDefault ("ns3::LteHelper::AnrEnabled", BooleanValue (true));
	
	// Error Free model
	//Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (true));
	Config::SetDefault ("ns3::LteSpectrumPhy::CtrlErrorModelEnabled", BooleanValue(false));
	//Config::SetDefault ("ns3::LteSpectrumPhy::DataErrorModelEnabled", BooleanValue(false));

	//HANDOVER_LEAVING_TIMEOUT (default 500)
	//Config::SetDefault ("ns3::LteEnbRrc::HandoverLeavingTimeoutDuration", TimeValue (MilliSeconds (800)));

	//RA preambles default 50 to 10
	//Config::SetDefault ("ns3::LteEnbMac::NumberOfRaPreambles",UintegerValue (10));
	//Config::SetDefault ("ns3::LteEnbMac::NumberOfRaPreambles",UintegerValue (30));
	Config::SetDefault ("ns3::LteEnbMac::NumberOfRaPreambles",UintegerValue (50));
	Config::SetDefault ("ns3::LteEnbMac::PreambleTransMax",UintegerValue (50));

	Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (packetSize)); //Default: 536B
	//Enough buffer Size
	//Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (640000));
	//Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (640000));
	Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (12800000));
	Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (12800000));

	// !!!!!!!!!! Should set DropTailQueue::MaxPackets value (default:100) !!!!!!!!!!!!!!!!!!
	// DropTailQueue is used on the p2p-links in EPC between SgwPgw node and any remote host
	// It causes an problem when remote hosts create more than 100 packets at the same time.
	// So, it can be problem when increasing the number of UE nodes
	Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (1000));
	//Config::SetDefault ("ns3::RrFfMacScheduler::HarqEnabled", BooleanValue (false));
	Config::SetDefault ("ns3::RrFfMacScheduler::HarqEnabled", BooleanValue (true));
	
	//RLC AM, BufferBloat
	Config::SetDefault ("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue (ns3::LteEnbRrc::RLC_AM_ALWAYS));


#if 1
	//Random Seed & Run for random variables
	ns3::RngSeedManager::SetSeed(rndSeed);
	ns3::RngSeedManager::SetRun(rndRun);
#endif

	double meanDataRate = capacity * offeredLoad / nUes;

	// print out simulation start time
	float m_timeTotal, m_timeStart, m_timeEnd;
	std::string m_currentTime;
	std::cout << CurrentDateTime() << " : Simulation Runs..............." << std::endl;
	m_currentTime = CurrentDateTime ();
	m_timeStart = clock ();
	
#ifdef ANIMATION
	std::stringstream ssRnd;
	ssRnd << rndRun;
	std::string rndNum = ssRnd.str();
	std::stringstream ssSeed;
	ssSeed << rndSeed;
	std::string seedNum = ssSeed.str();
	std::string animFile = "SimpleAnim_SEED"+seedNum+"_RND"+rndNum+"_"+m_currentTime+".xml";
#endif


//	std::cout << "nUes= " << nUes << ", offeredLoad= " << offeredLoad << ", nPicoEnb= " << nPicoEnb << ", scenariotype = " << scenarioType << std::endl;
	std::cout << "nUes= " << nUes << ", simTime = " << simTime <<  ", offeredLoad= " << offeredLoad << ", nPicoEnb= " << nPicoEnb << ", scenariotype = " << scenarioType << ", picoVariance = " << picoVariance << ", absInterval = " << absInterval << ", rpB = " << rpB << std::endl;
	

	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	NodeContainer remotesForUes, enbNodes, ueNodes;
	remotesForUes.Create (nUes);
	
	//Create macro & pico eNB
	enbNodes.Create (nPicoEnb+1);
	ueNodes.Create (nUes);

	//Set LteHelper with EPC
	Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
	Ptr<PointToPointEpcHelper> epc = CreateObject<PointToPointEpcHelper> ();
	lteHelper->SetEpcHelper (epc);
	lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
	lteHelper->SetHandoverAlgorithmType ("ns3::A3RsrpHandoverAlgorithm");   // setting Handover algorithm
	//lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis", DoubleValue (3.0)); 	// larger than > 1 dB
	//lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis", DoubleValue (2.0)); 	// larger than > 1 dB
	//lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger", TimeValue (MilliSeconds (500)));	//  during 500ms
	//lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger", TimeValue (MilliSeconds (1024)));	//  during 50ms

	Ptr<Node> pgw = epc->GetPgwNode ();

	//Internet
	InternetStackHelper internet;
	internet.Install (remotesForUes);
	internet.Install (ueNodes);

	//wired configuration
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute ("DataRate", StringValue("1Gbps"));
	pointToPoint.SetChannelAttribute ("Delay", StringValue("1ms"));

	//Temporal containers
	NetDeviceContainer p2pDevs;

	//Address
	Ipv4AddressHelper ipv4;
	char network[256];

	//remotesForUes - pgw
	for (uint32_t i = 0; i < remotesForUes.GetN (); i++ )
	{
		p2pDevs = pointToPoint.Install (remotesForUes.Get (i), pgw);

		sprintf (network, "1.1.%d.0", i+1 );
		ipv4.SetBase (network, "255.255.255.0");
		ipv4.Assign (p2pDevs);
	}

	//Set macro/pico eNBs' mobility & position
	Vector initPicoPos = CalcInitPicoPosition ( nPicoEnb, distancePico, cellCenter );
	SetUniformPicoPosition ( enbNodes, cellCenter, initPicoPos, distancePico, nPicoEnb );

	//for eliminating ns3-NetAnim warning msg.
	Ptr<ListPositionAllocator> tmpPos = CreateObject<ListPositionAllocator> ();
	tmpPos->Add (cellCenter);
	MobilityHelper mobilityPgw;
	mobilityPgw.SetPositionAllocator (tmpPos);
	mobilityPgw.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobilityPgw.Install (pgw); 


	// for checking Pico position  allocation
	for (uint16_t k=0; k<enbNodes.GetN (); k++ ) {
		Ptr<Node> nI = enbNodes.Get (k);
		Vector pos = nI->GetObject <MobilityModel> ()->GetPosition ();
		std::cout << "eNB #" << k << " pos( " << pos.x << ", " << pos.y << ", " << pos.z << " )" << std::endl;
	}

	
	//Mobility
	/*
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	for (uint16_t i = 0; i < nUes; i++ )
	{
		positionAlloc->Add (Vector(distance * i, 0, 0));
	}
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobility.SetPositionAllocator (positionAlloc);
	*/

	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	positionAlloc->Add (Vector (-100,100,0));

	MobilityHelper mobilityRemotes;
	mobilityRemotes.SetPositionAllocator (positionAlloc);
	mobilityRemotes.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobilityRemotes.Install (remotesForUes);


	if ( scenarioType == "UNIFORM" )
	{
		MobilityHelper mobilityUe;
		char stringX[8],stringY[8],bounds[256];
		sprintf (stringX, "%f", cellCenter.x); 
		sprintf (stringY, "%f", cellCenter.y); 
		sprintf (bounds, "ns3::UniformRandomVariable[Min=0|Max=%f]", cellRadius); 
		mobilityUe.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
																		"X", StringValue (stringX),
																		"Y", StringValue (stringY),
																		"Rho", StringValue (bounds));
		#if 1
			mobilityUe.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
		#else

		sprintf (bounds, "0|%f|0|%f", (cellCenter.x+cellRadius), (cellCenter.y+cellRadius));
		mobilityUe.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
																"Mode", StringValue ("Time"),
																"Time", StringValue ("0.01s"),
																"Bounds", StringValue (bounds));
		#endif
		/*
		mobilityUe.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
																		"X", StringValue ("0.0"),
																		"Y", StringValue ("100.0"),
																		"Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=200]"));
	
		mobilityUe.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
																"Mode", StringValue ("Time"),
																"Time", StringValue ("2s"),
																"Bounds", StringValue ("-2000|2000|-1000|3000"));
		*/	
		mobilityUe.Install (ueNodes);
	} else if ( scenarioType == "HOTSPOT" ) // 2/3 ues are around pico eNBs and the remains are uniformly distributed over the cell
	{
		MobilityHelper mobilityUe;
		double meanPicoUes = round (nUes * 2/3 / nPicoEnb);
		//double meanPicoUes = round (nUes * 1/2 / nPicoEnb);		// 2016.12.21 temporary
		uint32_t nUesPerPico[nPicoEnb];
		int totalPicoUes;

		Ptr<NormalRandomVariable> x = CreateObject<NormalRandomVariable> ();
		x->SetAttribute ("Mean", DoubleValue (meanPicoUes));
		x->SetAttribute ("Variance", DoubleValue (picoVariance));
		//x->SetAttribute ("Bound", DoubleValue ( meanPicoUes/2 ));

		for (uint32_t i = 0; i< nPicoEnb; i++) 
		{
			nUesPerPico[i] = (int) x->GetValue ();
			totalPicoUes = totalPicoUes + nUesPerPico[i];
			std::cout << "# of UEs at Pico[" << i << "] = " << nUesPerPico[i] << std::endl;
		}

		// 2/3 ues makes HOTSPOT
		uint32_t ueCount=0;
		for (uint32_t i = 0; i< nPicoEnb; i++) 
		{
			Ptr<Node> pico =	enbNodes.Get (i+1);	//enbNodes(0) = macro eNB
			Vector center = pico->GetObject <MobilityModel> ()->GetPosition ();
			
			char stringX[8],stringY[8],bounds[256];
			sprintf (stringX, "%f", center.x); 
			sprintf (stringY, "%f", center.y); 
			sprintf (bounds, "ns3::UniformRandomVariable[Min=0|Max=%f]", picoRadius); 
			mobilityUe.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
																			"X", StringValue (stringX),
																			"Y", StringValue (stringY),
																			"Rho", StringValue (bounds));
		#if 0
			mobilityUe.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
		#else

			sprintf (bounds, "0|%f|0|%f", (center.x+picoRadius), (center.y+picoRadius));
			mobilityUe.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
																	"Mode", StringValue ("Time"),
																	"Time", StringValue ("0.01s"),
																	"Bounds", StringValue (bounds));
		#endif	
			for (uint32_t j = 0; j < nUesPerPico[i]; j++)
			{
				mobilityUe.Install (ueNodes.Get (ueCount));
				ueCount++;
				std::cout << ueCount << std::endl;
			}

		}

		// 1/3 ues are uniformly distributed over the cell
		char stringX[8],stringY[8],bounds[256];
		sprintf (stringX, "%f", cellCenter.x); 
		sprintf (stringY, "%f", cellCenter.y); 
		sprintf (bounds, "ns3::UniformRandomVariable[Min=0|Max=%f]", cellRadius); 
		mobilityUe.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
																		"X", StringValue (stringX),
																		"Y", StringValue (stringY),
																		"Rho", StringValue (bounds));
		#if 0
			mobilityUe.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
		#else

		sprintf (bounds, "0|%f|0|%f", (cellCenter.x+cellRadius), (cellCenter.y+cellRadius));
		mobilityUe.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
																"Mode", StringValue ("Time"),
																"Time", StringValue ("0.01s"),
																"Bounds", StringValue (bounds));
		#endif
		for (int i = ueCount; i < nUes; i++)
		{
			mobilityUe.Install (ueNodes.Get (i));
		}

#if 0
		int nUesPerPico = round(nPicoUes/nPicoEnb);
		for (uint32_t i = (nUes-nPicoUes); i < nUes; i++)
		{
			for (uint32_t j = 0; j < nUesPerPico; j++) 
			{

			}
		}
#endif
	}

	//Vector posEnb = Vector(500,500,0);

#if 0
	//Set eNB mobility & position
	Ptr<ListPositionAllocator> positionEnb = CreateObject<ListPositionAllocator> ();
	positionEnb->Add (Vector (500,500,0));


	MobilityHelper mobilityEnb;
	mobilityEnb.SetPositionAllocator (positionEnb);
	mobilityEnb.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobilityEnb.Install (enbNodes.Get (0));
	
	mobilityEnb.Install (pgw); //for eliminating ns3-NetAnim warning msg.
/*
	Ptr<Node> tmpEnb = enbNodes.Get(0);
	Vector posEnb = Vector(500,500,0);
	tmpEnb->GetObject <MobilityModel> ()->SetPosition( posEnb );
*/
#endif

#if 1
	// for checking positions
	for (uint16_t k=0; k<ueNodes.GetN (); k++)
	{
		Ptr<Node> nI = ueNodes.Get (k);
		Vector pos = nI->GetObject <MobilityModel> ()->GetPosition ();
		std::cout << "UE " << k << " @ " << "( " <<pos.x << ", " << pos.y << ", " << pos.z << " )" << std::endl;
	}

	Ptr<Node> tmp = enbNodes.Get(0);
	Vector pos = tmp->GetObject <MobilityModel> ()->GetPosition ();
	std::cout << "eNB " <<  " @ " << "( " <<pos.x << ", " << pos.y << ", " << pos.z << " )" << std::endl;
#endif


	lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::LogDistancePropagationLossModel"));
	lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
	lteHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (nRB));
	

	NetDeviceContainer enbDev;
	NetDeviceContainer ueDevs;
	enbDev = lteHelper->InstallEnbDevice (enbNodes);
	ueDevs = lteHelper->InstallUeDevice (ueNodes);

	//Address
	Ipv4InterfaceContainer ueIfs = epc->AssignUeIpv4Address (ueDevs);

	//User Association
	//lteHelper->Attach (ueDevs, enbDev.Get(0));
	// automatic attachment of  a set of UE devices to a suitable cell using Idle mode initial cell selection procedure
	lteHelper->Attach (ueDevs);	

	// Add X2 interface
	//lteHelper->AddX2Interface (enbNodes);	

#if 0
	for (uint16_t i = 1; i < enbNodes.GetN (); i++ )
	{
		lteHelper->AddX2Interface (enbNodes.Get (0), enbNodes.Get (i));		//X2interface between a macro and pico enbs
	}
#else
	for (uint16_t i = 0; i < enbNodes.GetN (); i++ )
	{
		for (uint16_t j = i+1; j < enbNodes.GetN (); j++ ) {
			lteHelper->AddX2Interface (enbNodes.Get (i), enbNodes.Get (j));		//X2interface between a macro and pico enbs
		}
	}
#endif
	//Setting Tx power of different nodes in the HetNet
	for (uint16_t i = 0; i < enbNodes.GetN (); i++ )
	{
		if ( i == 0 ) 
		{
			enbNodes.Get (i)->GetDevice (0)->GetObject <LteEnbNetDevice> ()->GetPhy ()->SetTxPower (macroTx);
		}
		else
		{
			enbNodes.Get (i)->GetDevice (0)->GetObject <LteEnbNetDevice> ()->GetPhy ()->SetTxPower (picoTx);
		}
	}

	//Routing
	Ipv4StaticRoutingHelper routing;
	Ptr<Ipv4StaticRouting> routingEntity;
	for (uint32_t i = 0; i < remotesForUes.GetN (); i++ )
	{
		// remotesForUes -> ueNodes
		routingEntity = routing.GetStaticRouting (remotesForUes.Get (i)->GetObject<Ipv4> ());
		routingEntity->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1); 
		//ueNodes -> remotesForUes
		routingEntity = routing.GetStaticRouting (ueNodes.Get (i)->GetObject<Ipv4> ());
		routingEntity->SetDefaultRoute (epc->GetUeDefaultGatewayAddress (), 1);
	}

#if 1
	// Setting bias CRE values
	Ptr <BiasHelper> b = CreateObject <BiasHelper> ();
	b->SetBias (rpB, rqB);

	//ueNodes.Get (0)->GetDevice (0)->GetObject <LteUeNetDevice> ()->GetPhy ()->SetBiasHelper (b);
	

	for (uint16_t ueIt = 0; ueIt<ueNodes.GetN (); ueIt++)
	{
		//Ptr <Node> n0 = ueNodes.Get (ueIt);
		//n0->GetDevice (0)->GetObject <LteUeNetDevice> ()->GetPhy ()->SetBiasHelper (b);
		ueNodes.Get (ueIt)->GetDevice (1)->GetObject <LteUeNetDevice> ()->GetPhy ()->SetBiasHelper (b);
		//std::cout << "Device ## " << ueNodes.Get (ueIt)->GetNDevices () << std::endl;
	}

	// i=0 is macro
	/*
	for (uint8_t enbIt = 1; enbIt < enbNodes.GetN (); enbIt++)
	{
		b->biasList.push_back ( enbNodes.Get(enbIt)->GetDevice (0)->GetObject <LteEnbNetDevice> ()->GetCellId ());
		//std::cout << "Cell ID : " << enbNodes.Get(enbIt)->GetDevice (0)->GetObject <LteEnbNetDevice> ()->GetCellId () << std::endl;
	}
	*/

#endif


#if 1
  //ABS related
	// Getting Ptrs to rrFfMacScheduler to initialise for ABS
	Ptr<FfMacScheduler> rrFfMacScheduler = enbNodes.Get (0)->GetDevice (0)->GetObject <LteEnbNetDevice> ()->GetFfMacSchedule (); 
	
	// Get access to physical layer in scheduler
	rrFfMacScheduler->GetObject <RrFfMacScheduler> ()->SetPhy (enbNodes.Get (0)->GetDevice (0)->GetObject <LteEnbNetDevice> ()->GetPhy ());
	
	//setting parameters for macro
	rrFfMacScheduler->GetObject <RrFfMacScheduler> ()->SetAbsParameters(absPeriod,absInterval);
	rrFfMacScheduler->GetObject <RrFfMacScheduler> ()->SetMacroNode(true);
	
	
	// i=0 is macro
	for (uint8_t i = 1; i < enbNodes.GetN (); i++)
	{
		Ptr<FfMacScheduler> rrFfMacSchedulerPico = enbNodes.Get (i)->GetDevice (0)->GetObject <LteEnbNetDevice> ()->GetFfMacSchedule (); 
		rrFfMacSchedulerPico->GetObject <RrFfMacScheduler> ()->SetPhy (enbNodes.Get (i)->GetDevice (0)->GetObject <LteEnbNetDevice> ()->GetPhy ());
		//setting parameters for pico
		rrFfMacSchedulerPico->GetObject <RrFfMacScheduler> ()->SetMacroNode (false);
		rrFfMacSchedulerPico->GetObject <RrFfMacScheduler> ()->SetAbsParameters (absPeriod,absInterval);
		rrFfMacSchedulerPico->GetObject <RrFfMacScheduler> ()->SetMacroRr (rrFfMacScheduler->GetObject <RrFfMacScheduler> ());
	
		//setting neighbour links for communication
		rrFfMacScheduler->GetObject <RrFfMacScheduler> ()->SetPicoRr (enbNodes.Get (i)->GetObject <RrFfMacScheduler> ());
	}
#endif	

	//Application - traffic of Poisson dist. with OnOffModel in ns3
	//	NS_LOG_INFO ("Create Application");

	ApplicationContainer sourceApp, ueSinkApp;
	int port = 20;

	for (uint32_t i = 0; i< remotesForUes.GetN (); i++ )
	{

#if 0
		// Simple traffic for test
		
		PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress (ueIfs.GetAddress (i), port));
		ueSinkApp.Add (sink.Install (ueNodes.Get (i)));

		UdpClientHelper client (ueIfs.GetAddress (i), port);
		client.SetAttribute ("Interval", TimeValue (MilliSeconds(1000)));
		client.SetAttribute ("MaxPackets", UintegerValue (1000000));
	
		sourceApp.Add (client.Install (remotesForUes));
	
#endif

#if 1
		// Poisson traffic with OnOffmodel
		OnOffHelper onoff ("ns3::UdpSocketFactory", InetSocketAddress (ueIfs.GetAddress (i), port));

		// On
		char stringDurationOn[300];
		double durationOn = (double)packetSize * 8.0 / (double)(capacity / nUes);
		sprintf (stringDurationOn, "ns3::ConstantRandomVariable[Constant=" "%64.63f" "]", durationOn );

		onoff.SetAttribute ("OnTime", StringValue( stringDurationOn ));

		/* 
		 * Off
		 * dataRate of UE -> Exponential Random
		 * meanOffDuration of UE -> Exponential random with the dataRate of UE
		*/
		
		char stringDurationOff[300];
		Ptr<ExponentialRandomVariable> x = CreateObject<ExponentialRandomVariable> ();

		//x->SetAttribute ("Mean", DoubleValue (packetSize * 8.0 / meanDataRate - durationOn));
		x->SetAttribute ("Mean", DoubleValue (meanDataRate));
		x->SetAttribute ("Bound", DoubleValue (0));

		double dataRate = x->GetValue();
		//double dataRate = ns3::ExponentialRandomVariable->GetValue (meanDataRate, 0);
		
		// Make more similar to Poisson (inter-arrival)
		double meanDurationOff = packetSize * 8.0 / dataRate - durationOn;

		// Need a modification !!
		if ( meanDurationOff <= 0 )
		{
			meanDurationOff = packetSize * 8.0 / dataRate;
		}
		sprintf(stringDurationOff, "ns3::ExponentialRandomVariable[Mean=" "%64.63f" "]", meanDurationOff );

		//std::cout << i << ": " << dataRate << std::endl;

		onoff.SetAttribute ("OffTime", StringValue (stringDurationOff));
		onoff.SetAttribute ("PacketSize", UintegerValue (packetSize));
		onoff.SetAttribute ("DataRate", DataRateValue (capacity / nUes));
		onoff.SetAttribute ("MaxBytes", UintegerValue (dataRate*applicationDuration/8));
		
		sourceApp.Add (onoff.Install (remotesForUes.Get (i)));

		ApplicationContainer tmpApp;
		tmpApp.Add ( sourceApp.Get (sourceApp.GetN () - 1));
		tmpApp.Start ( Seconds (applicationStart));
	
		//TCP Tracing
		/*
		if( i == 0 && i != remotesForUes.GetN()-1 )
		{
			traceFilePrefixLte = traceFilePrefix + "-ueNodes";
			BulkSendApplicationTcpTrace (sourceApp.Get(sourceApp.GetN ()-1)->GetObject<BulkSendApplication> (), traceFilePrefixLte, Seconds (applicationStart +0.2), Seconds (simulationEnd));
		}
		*/
		PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress (ueIfs.GetAddress (i), port));
		ueSinkApp.Add (sink.Install (ueNodes.Get (i)));
#endif
	}

	sourceApp.Start (Seconds (applicationStart));
	sourceApp.Stop (Seconds (applicationEnd));
	ueSinkApp.Start (Seconds (0));
	ueSinkApp.Stop (Seconds (applicationEnd));

	FlowMonitorHelper flowmon;
	Ptr <FlowMonitor> monitor;
	monitor = flowmon.Install ( remotesForUes );
	monitor = flowmon.Install ( ueNodes );
	//monitor = flowmon.GetMonitor ();

	Simulator::Stop (Seconds (simulationEnd + 1.0));

	//KPI enable
	lteHelper->EnableMacTraces ();
	lteHelper->EnableRlcTraces ();
	lteHelper->EnablePdcpTraces ();


#if 1
//Mobility trace
//  /*  
//    AsciiTraceHelper ascii;
//      MobilityHelper::EnableAsciiAll (ascii.CreateFileStream ("mobility-trace.mob"));
//        */
//          // connect custom trace sinks for RRC connection establishment and handover notification
//
	Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
										MakeCallback (&NotifyConnectionEstablishedEnb));
	Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
										MakeCallback (&NotifyConnectionEstablishedUe));
	Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
										MakeCallback (&NotifyHandoverStartEnb));
	Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
										MakeCallback (&NotifyHandoverStartUe));
	Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
										MakeCallback (&NotifyHandoverEndOkEnb));
	Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
										MakeCallback (&NotifyHandoverEndOkUe));
	//Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/NewUeContext", MakeCallback (&NotifyNewUeContext));
	//Config::Connect ("/NodeList/4/DeviceList/0/LteUePhy/ReportCurrentCellRsrpSinr", MakeCallback (&SinrLevels));

	//Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
	//									MakeCallback (&CourseChange));
	//Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/LteRlc/TxPDU",
	//                 MakeCallback (&NotifyPduReceived));

#endif

#ifdef ANIMATION
	//NetAnim
	AnimationInterface anim(animFile);
#endif

	Simulator::Run ();

	//Print delay test
	Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();

#if 0
	std::vector<double> delayStat;
	delayStat = rlcStats->GetDlDelayStats( 1, 3);
	std::cout << "IMSI:1 LCID:3 " << delayStat[0] << " / " << delayStat[1] << " / " << delayStat[2] << std::endl;
#endif

	//Flow monitor
	std::string flowFile = "flowen_SEED"+seedNum+"_RND"+rndNum+"_"+m_currentTime+".xml";
	monitor->SerializeToXmlFile(flowFile, true, true);
	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
	std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

	double throughputArray[nUes];
	double delayArray[nUes];
	double percentageThroughput[nUes];
	double hoArray[nUes];
	double percentageHO[nUes];

	for (uint16_t i = 0; i < nUes; ++i ) {
		std::map <uint64_t, uint32_t>::iterator it = handoverCount.find ( i+1 );
		if ( it != handoverCount.end () ) {
			hoArray[i] = it->second;
		} else {
			hoArray[i] = 0;
		}
	}

	int tmpIdx=0;

	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
	{
 		Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);
  	NS_LOG_UNCOND("Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << " Dst Addr " << t.destinationAddress);
#if 0
  	NS_LOG_UNCOND("Tx Packets = " << iter->second.txPackets);
    NS_LOG_UNCOND("Tx Bytes = " << iter->second.txBytes);
    NS_LOG_UNCOND("Sum jitter = " << iter->second.jitterSum);
    //NS_LOG_UNCOND("Delay Sum = " << iter->second.delaySum);
    NS_LOG_UNCOND("Mean Delay = " << iter->second.delaySum/iter->second.rxPackets); NS_LOG_UNCOND("Lost Packet = " << iter->second.lostPackets); NS_LOG_UNCOND("Rx Bytes = " << iter->second.rxBytes);
    NS_LOG_UNCOND("Rx Packets = " << iter->second.rxPackets);
    NS_LOG_UNCOND("Throughput: " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024  << " Kbps");
 		NS_LOG_UNCOND("Packet loss %= " << ((iter->second.txPackets-iter->second.rxPackets)*1.0)/iter->second.txPackets);
#endif
		if (iter->second.rxPackets > 0 && iter->second.delaySum > 0)
		{
    	NS_LOG_UNCOND("Mean Delay = " << iter->second.delaySum/iter->second.rxPackets);
			ns3::Time tmpTime = iter->second.delaySum/iter->second.rxPackets;
			delayArray[tmpIdx] = tmpTime.GetDouble ();
			//delayArray[tmpIdx] = iter->second.delaySum.GetSeconds()/(double)iter->second.rxPackets;
			if ( isnan (delayArray[tmpIdx]) )
				delayArray[tmpIdx] = 0;
		} else {
    	NS_LOG_UNCOND("Mean Delay = " << "0");
			delayArray[tmpIdx] = 0;
		}
		if (iter->second.rxBytes > 0)
		{
#if 1
    	//NS_LOG_UNCOND("Throughput: " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024  << " Kbps");
			throughputArray[tmpIdx] = iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024;
			NS_LOG_UNCOND("Throughput:: " << throughputArray[tmpIdx] << " Kbps");
			if ( isnan (throughputArray[tmpIdx]) ) 
				throughputArray[tmpIdx] = 0;

		} else {
			NS_LOG_UNCOND("Throughput:: " << "0" << " Kbps");
			throughputArray[tmpIdx] = 0;
#endif
		}
		tmpIdx++;
 		NS_LOG_UNCOND("Packet loss %= " << ((iter->second.txPackets-iter->second.rxPackets)*1.0)/iter->second.txPackets);
  }
	
	//Throughput
#if 0
	for (uint32_t i = 0; i < remotesForUes.GetN (); i++) 
	{
		Ptr<PacketSink> sink = ueSinkApp.Get (i)->GetObject<PacketSink> ();
		throughputArray[i] = sink->GetTotalRx () * 8 / (applicationEnd - applicationStart) / 1000;
		std::cout << "UE" << i << '(' << ueIfs.GetAddress(i) << "):" << sink->GetTotalRx () * 8 / (applicationEnd - applicationStart) / 1000000 << "Mbps" << std::endl;
	}
#endif

	std::cout << "Throughput Percentage " << std::endl;
	for (int i=0; i<nUes; i++ ) {
		if (i == 0){
			percentageThroughput[i] = 1.0/nUes;
			//std::cout << percentage[i] << std::endl;
			printf ("%d :: %f \n", i, percentageThroughput[i] );
		} else {
			percentageThroughput[i] = percentageThroughput[i-1]+(1.0/nUes);
			printf ("%d :: %f \n", i, percentageThroughput[i] );
		}
	}
	std::cout << " " << std::endl;

	std::cout << "Handover Percentage " << std::endl;
	for (int i=0; i<nUes; i++ ) {
		if (i == 0){
			percentageHO[i] = 1.0/nUes;
			//std::cout << percentage[i] << std::endl;
			printf ("%d :: %f \n", i, percentageHO[i] );
		} else {
			percentageHO[i] = percentageHO[i-1]+(1.0/nUes);
			printf ("%d :: %f \n", i, percentageHO[i] );
		}
	}
	
	PrintHandoverCount ();
	Sort (throughputArray, nUes);
	Sort (delayArray, nUes);
	Sort (hoArray, nUes);

	double mean = 0;
	double positiveStdDev = 0;
	double negativeStdDev = 0;
	double minimum = 0;
	double maximum = 0;
	double median = 0;
	double q1 = 0;
	double q3 = 0;
	double jainIdx = 0;


	PlotThroughputCdf (throughputArray, percentageThroughput , nUes, absInterval, simTime, m_currentTime);
	PlotDelay (delayArray, nUes, absInterval, simTime, m_currentTime);
	PlotHOCdf(hoArray, percentageHO, nUes, absInterval, simTime, m_currentTime);
	//PlotDelay (delayArray, nUes, absInterval, rpB);
		
	Mean (throughputArray, nUes, &mean);
	StdDev (throughputArray, nUes, &positiveStdDev, &negativeStdDev);
	Median_Q1Q3 (throughputArray, nUes, &median, &q1, &q3);
	Minimum (throughputArray, nUes, &minimum);
	Maximum (throughputArray, nUes, &maximum);
	JainFairnessIndex (throughputArray, nUes, &jainIdx);

	std::ostringstream os;
	os << "DistUeBias_Box_Jainfiarness"+m_currentTime+".txt";
	std::ofstream of (os.str ().c_str (), std::ios::out | std::ios::app);
	of << minimum << " " << q1 << " " << median << " " << q3 << " " << maximum << " JainFair: " << jainIdx << "\n";
	of.close ();


	//std::string outputFileName = "throughput-cdf.dat";

#if 0
	//Throughput
	for (uint32_t i = 0; i < remotesForUes.GetN (); i++) 
	{
		Ptr<PacketSink> sink = ueSinkApp.Get (i)->GetObject<PacketSink> ();
		std::cout << "UE" << i << '(' << ueIfs.GetAddress(i) << "):" << sink->GetTotalRx () * 8 / (applicationEnd - applicationStart) / 1000000 << "Mbps" << std::endl;
	}
#endif

	Simulator::Destroy ();

	m_timeEnd = clock ();
	m_timeTotal=(m_timeEnd - m_timeStart)/(double) CLOCKS_PER_SEC; 
	std::cout << CurrentDateTime () << " : Simulation Ends..............." << std::endl;
	std::cout << "\n*** Simulation time: " << m_timeTotal << "s\n\n"; 

	return 0;
}
