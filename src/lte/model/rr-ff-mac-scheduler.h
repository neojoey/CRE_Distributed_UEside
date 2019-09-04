/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
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
 *
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 */

#ifndef RR_FF_MAC_SCHEDULER_H
#define RR_FF_MAC_SCHEDULER_H

#include <ns3/ff-mac-csched-sap.h>
#include <ns3/ff-mac-sched-sap.h>
#include <ns3/ff-mac-scheduler.h>
#include <vector>
#include <map>
#include <ns3/lte-common.h>
#include <ns3/lte-amc.h>
#include <ns3/nstime.h>
#include <ns3/simulator.h>
#include <ns3/node.h>
#include <ns3/lte-enb-net-device.h>
#include <ns3/lte-ffr-sap.h>
#include <ns3/lte-enb-phy.h>

#define HARQ_PROC_NUM 8
#define HARQ_DL_TIMEOUT 11

namespace ns3 {


typedef std::vector < uint8_t > DlHarqProcessesStatus_t;
typedef std::vector < uint8_t > DlHarqProcessesTimer_t;
typedef std::vector < DlDciListElement_s > DlHarqProcessesDciBuffer_t;
typedef std::vector < std::vector <struct RlcPduListElement_s> > RlcPduList_t; // vector of the LCs and layers per UE
typedef std::vector < RlcPduList_t > DlHarqRlcPduListBuffer_t; // vector of the 8 HARQ processes per UE

typedef std::vector < UlDciListElement_s > UlHarqProcessesDciBuffer_t;
typedef std::vector < uint8_t > UlHarqProcessesStatus_t;




/**
 * \ingroup ff-api
 * \brief Implements the SCHED SAP and CSCHED SAP for a Round Robin scheduler
 *
 * This class implements the interface defined by the FfMacScheduler abstract class
 */

class RrFfMacScheduler : public FfMacScheduler
{
public:
  /**
   * \brief Constructor
   *
   * Creates the MAC Scheduler interface implementation
   */
  RrFfMacScheduler ();

  /**
   * Destructor
   */
  virtual ~RrFfMacScheduler ();

  // inherited from Object
  virtual void DoDispose (void);
  static TypeId GetTypeId (void);

  // inherited from FfMacScheduler
  virtual void SetFfMacCschedSapUser (FfMacCschedSapUser* s);
  virtual void SetFfMacSchedSapUser (FfMacSchedSapUser* s);
  virtual FfMacCschedSapProvider* GetFfMacCschedSapProvider ();
  virtual FfMacSchedSapProvider* GetFfMacSchedSapProvider ();

  // FFR SAPs
  virtual void SetLteFfrSapProvider (LteFfrSapProvider* s);
  virtual LteFfrSapUser* GetLteFfrSapUser ();

  friend class RrSchedulerMemberCschedSapProvider;
  friend class RrSchedulerMemberSchedSapProvider;

  void TransmissionModeConfigurationUpdate (uint16_t rnti, uint8_t txMode);

	//for Abs by joey 2016.04.12
	void SetAbsParameters (uint32_t p, uint16_t i);

	//Triggers the blanking of a subframe.
	void SetAbsTrigger (bool b)
	{
		absFrame = b;
	}

	void AbsFrameCounter ()
	{
		absCount++;
		SetAbsTrigger (false);	//Default value
		//initialises abs trigger and counts subframe for determining pattern
	}

	void SetPhy (Ptr <LteEnbPhy> p)
	{
		// Communication between layers if needed.
		m_phy = p;
	}

	void CheckAbsCounter ()
	{
	// Determining pattern, only for low patterns: 1,2 and 3
		if (absCount%absInterval==0)
		{
			absEndInterval = false;
		}
		if (absEndInterval==true)
		{
			return;
		}
		if (absCount%2 == 0)
		{
			if(absEnabled==true)
			{
				SetAbsTrigger (true);
				absFrames++;
			}
		}
		if (absFrames%absPeriod == 0)
		{
			absFrames = 0;
			absEndInterval = true;
			return;
		}
	}

	void SetMacroNode (bool b)
	{
		//Specifying that this is a macro node
		mNode = b;
		m_phy->SetMnode (b);
	}

	// Used in run file to make the schedulers aware of eachother.
	// Currently only 1 neighbour
	// expanded to multiple pico enbs and a macro enb by joey 2016.04.18
	void SetPicoRr (Ptr <RrFfMacScheduler> n)
	{
		picoRr.push_back (n);
	}
	
	void SetMacroRr (Ptr <RrFfMacScheduler> n)
	{
		macroRr = n;
	}

	// used in the macro base station to start abs.
	void InitiateAbs ()
	{
		if (absEnabled ==false)
		{
			absStart = Simulator:: Now(); //used to calculate abs specific throughput if necessary 
		}
		absEnabled = true;
	}

	// used by pico to recommend abs deployment to increase throughput.
	void InvokeAbs ()
	{
		//ask macro for abs
		if (mNode == false)
		{
			macroRr->InitiateAbs ();
			absEnabled = true;
			//std::cout << "abs invoked..." << std::endl;
		}
	}
	// abs update sent from pico station.
	void AbsUpdateSend (bool b)
	{
		absEnabled = b;
#if 1
		this->AbsUpdateRecv (b);
		//std::cout << "update recv " << std::endl;
#else
		std::list <Ptr<RrFfMacScheduler> >::const_iterator it;
		for (it = picoRr.begin (); it!= picoRr.end (); it++)
		{
			(*it)->AbsUpdateRecv (b);
			std::cout << "dddddddd = " << picoRr.size () <<  std::endl;
		}
#endif
	}

	// ABS config 
	void AbsUpdateRecv (bool b)
	{
		absEnabled = b;
		//std::cout << "update recved ... " << std::endl;
	}
  //managing victims
	void VictimListEdit (uint16_t rnti, bool add)
	{
		if (add==true)
		{
			std::list <uint16_t>::iterator it;
			bool check = false;
			for (it = victimList.begin();it !=victimList.end();it++)
			{
				if (*it == rnti)
				{
					check = true;
				}
			}
			if (check == true){return;}
			std::vector <ThroughputInfo>::iterator itThr;
			for (itThr = ThroughputInfo_s.begin (); itThr != ThroughputInfo_s.end(); itThr++)
			{
				if ((rnti==(*itThr).rnti)&&(absEnabled==true))
				{
					(*itThr).absStart = Simulator:: Now ();
				}

			}

			victimList.push_back (rnti);
		}
		else
		{
			std::list <uint16_t>::iterator it;
			bool check = false;
			for (it = victimList.begin();it !=victimList.end();it++)
			{
				if (*it == rnti)
				{
					check = true;
				}
			}
			if (check == false){return;}
			
			std::vector <ThroughputInfo>::iterator itThr;
			victimList.remove (rnti);
		}
	}

	bool IsVictim (uint16_t rnti)
	{
		std::list <uint16_t>::iterator it;
		bool check = false;
		for (it = victimList.begin();it !=victimList.end();it++)
		{
			if (*it == rnti)
			{
				check = true;
				return check;
			}
		}
		return check;
	}

	// managing throughput calculations to facilitate future algorithms
	void UpdateThroughputCalc (uint16_t r, double tr)
	{
	std::vector <ThroughputInfo>::iterator itTh;
	bool rntiFound = false;
	double sum = 0;


	for (itTh = ThroughputInfo_s.begin();itTh != ThroughputInfo_s.end();itTh++)
	{
		if ((*itTh).rnti == r)
		{
			rntiFound = true;
			(*itTh).throughputStream.push_back (tr);

			std::vector <double>::iterator i;
			uint16_t rn = (*itTh).rnti;
			if ((absFrame==true) && ( IsVictim( rn ) ))
			{
				(*itTh).throughputAbs.push_back (tr);
			}
			if ((*itTh).throughputStream.size ()>30)
			{
				(*itTh).throughputStream.erase ((*itTh).throughputStream.begin ());
			}
			sum = 0;
			for (i = (*itTh).throughputStream.begin (); i != (*itTh).throughputStream.end();i++)
			{
				sum += *i;
			}
			Time timeDiff = Simulator::Now () - (*itTh).startTime;
			uint64_t timeDiffVal = timeDiff.GetMilliSeconds ();
			(*itTh).averageThroughput = sum/double(timeDiffVal);
			sum = 0;
			for (i = (*itTh).throughputAbs.begin (); i != (*itTh).throughputAbs.end();i++)
			{
				sum += *i;
			}
			if ((absEnabled==true)&&(IsVictim ((*itTh).rnti)))
			{
				Time timeDiffAbs = Simulator::Now () - (*itTh).absStart;
				uint64_t timeDiffAbsVal = timeDiffAbs.GetMilliSeconds ();
				(*itTh).averageThroughputAbs = sum/double(timeDiffAbsVal);
			}
			else
			{
				(*itTh).averageThroughputAbs = 0;
			}
			sum = 0;
			//std::cout << "RNTI: " << int((*itTh).rnti) << " has average throughput: " << double((*itTh).averageThroughput) << " Abs Throughput test: " << double ((*itTh).averageThroughputAbs) << std::endl;
			(*itTh).lastTbSize = tr;
		}
	}

	if (rntiFound==false)
	{
		ThroughputInfo t;
		t.rnti = r;
		t.averageThroughput = tr;
		t.lastTbSize = tr;
		t.throughputStream.push_back (tr);
		if (absEnabled==true)
		{
			t.absStart = Simulator::Now ();
		}
		if ((absFrame==true)&&(IsVictim(r)==true))
		{
			t.throughputAbs.push_back (tr);
			t.averageThroughputAbs = tr;
		}
		ThroughputInfo_s.push_back (t);
		t.startTime = Simulator::Now();
	}
}

private:
  //
  // Implementation of the CSCHED API primitives
  // (See 4.1 for description of the primitives)
  //

  void DoCschedCellConfigReq (const struct FfMacCschedSapProvider::CschedCellConfigReqParameters& params);

  void DoCschedUeConfigReq (const struct FfMacCschedSapProvider::CschedUeConfigReqParameters& params);

  void DoCschedLcConfigReq (const struct FfMacCschedSapProvider::CschedLcConfigReqParameters& params);

  void DoCschedLcReleaseReq (const struct FfMacCschedSapProvider::CschedLcReleaseReqParameters& params);

  void DoCschedUeReleaseReq (const struct FfMacCschedSapProvider::CschedUeReleaseReqParameters& params);

  //
  // Implementation of the SCHED API primitives
  // (See 4.2 for description of the primitives)
  //

  void DoSchedDlRlcBufferReq (const struct FfMacSchedSapProvider::SchedDlRlcBufferReqParameters& params);

  void DoSchedDlPagingBufferReq (const struct FfMacSchedSapProvider::SchedDlPagingBufferReqParameters& params);

  void DoSchedDlMacBufferReq (const struct FfMacSchedSapProvider::SchedDlMacBufferReqParameters& params);

  void DoSchedDlTriggerReq (const struct FfMacSchedSapProvider::SchedDlTriggerReqParameters& params);

  void DoSchedDlRachInfoReq (const struct FfMacSchedSapProvider::SchedDlRachInfoReqParameters& params);

  void DoSchedDlCqiInfoReq (const struct FfMacSchedSapProvider::SchedDlCqiInfoReqParameters& params);

  void DoSchedUlTriggerReq (const struct FfMacSchedSapProvider::SchedUlTriggerReqParameters& params);

  void DoSchedUlNoiseInterferenceReq (const struct FfMacSchedSapProvider::SchedUlNoiseInterferenceReqParameters& params);

  void DoSchedUlSrInfoReq (const struct FfMacSchedSapProvider::SchedUlSrInfoReqParameters& params);

  void DoSchedUlMacCtrlInfoReq (const struct FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params);

  void DoSchedUlCqiInfoReq (const struct FfMacSchedSapProvider::SchedUlCqiInfoReqParameters& params);


  int GetRbgSize (int dlbandwidth);

  static bool SortRlcBufferReq (FfMacSchedSapProvider::SchedDlRlcBufferReqParameters i,FfMacSchedSapProvider::SchedDlRlcBufferReqParameters j);

  void RefreshDlCqiMaps (void);
  void RefreshUlCqiMaps (void);

  void UpdateDlRlcBufferInfo (uint16_t rnti, uint8_t lcid, uint16_t size);
  void UpdateUlRlcBufferInfo (uint16_t rnti, uint16_t size);

  /**
  * \brief Update and return a new process Id for the RNTI specified
  *
  * \param rnti the RNTI of the UE to be updated
  * \return the process id  value
  */
  uint8_t UpdateHarqProcessId (uint16_t rnti);

  /**
  * \brief Return the availability of free process for the RNTI specified
  *
  * \param rnti the RNTI of the UE to be updated
  * \return the process id  value
  */
  uint8_t HarqProcessAvailability (uint16_t rnti);

  /**
  * \brief Refresh HARQ processes according to the timers
  *
  */
  void RefreshHarqProcesses ();

  Ptr<LteAmc> m_amc;

  /*
   * Vectors of UE's RLC info
  */
  std::list <FfMacSchedSapProvider::SchedDlRlcBufferReqParameters> m_rlcBufferReq;

  /*
  * Map of UE's DL CQI P01 received
  */
  std::map <uint16_t,uint8_t> m_p10CqiRxed;
  /*
  * Map of UE's timers on DL CQI P01 received
  */
  std::map <uint16_t,uint32_t> m_p10CqiTimers;

  /*
  * Map of previous allocated UE per RBG
  * (used to retrieve info from UL-CQI)
  */
  std::map <uint16_t, std::vector <uint16_t> > m_allocationMaps;

  /*
  * Map of UEs' UL-CQI per RBG
  */
  std::map <uint16_t, std::vector <double> > m_ueCqi;
  /*
  * Map of UEs' timers on UL-CQI per RBG
  */
  std::map <uint16_t, uint32_t> m_ueCqiTimers;



  /*
  * Map of UE's buffer status reports received
  */
  std::map <uint16_t,uint32_t> m_ceBsrRxed;

  // MAC SAPs
  FfMacCschedSapUser* m_cschedSapUser;
  FfMacSchedSapUser* m_schedSapUser;
  FfMacCschedSapProvider* m_cschedSapProvider;
  FfMacSchedSapProvider* m_schedSapProvider;

  // FFR SAPs
  LteFfrSapUser* m_ffrSapUser;
  LteFfrSapProvider* m_ffrSapProvider;

  // Internal parameters
  FfMacCschedSapProvider::CschedCellConfigReqParameters m_cschedCellConfig;

  uint16_t m_nextRntiDl; // RNTI of the next user to be served next scheduling in DL
  uint16_t m_nextRntiUl; // RNTI of the next user to be served next scheduling in UL

  uint32_t m_cqiTimersThreshold; // # of TTIs for which a CQI canbe considered valid

  std::map <uint16_t,uint8_t> m_uesTxMode; // txMode of the UEs
  


  // HARQ attributes
  /**
  * m_harqOn when false inhibit te HARQ mechanisms (by default active)
  */
  bool m_harqOn;
  std::map <uint16_t, uint8_t> m_dlHarqCurrentProcessId;
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` trasmission count
  std::map <uint16_t, DlHarqProcessesStatus_t> m_dlHarqProcessesStatus;
  std::map <uint16_t, DlHarqProcessesTimer_t> m_dlHarqProcessesTimer;
  std::map <uint16_t, DlHarqProcessesDciBuffer_t> m_dlHarqProcessesDciBuffer;
  std::map <uint16_t, DlHarqRlcPduListBuffer_t> m_dlHarqProcessesRlcPduListBuffer;
  std::vector <DlInfoListElement_s> m_dlInfoListBuffered; // HARQ retx buffered

  std::map <uint16_t, uint8_t> m_ulHarqCurrentProcessId;
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` trasmission count
  std::map <uint16_t, UlHarqProcessesStatus_t> m_ulHarqProcessesStatus;
  std::map <uint16_t, UlHarqProcessesDciBuffer_t> m_ulHarqProcessesDciBuffer;


  // RACH attributes
  std::vector <struct RachListElement_s> m_rachList;
  std::vector <uint16_t> m_rachAllocationMap;
  uint8_t m_ulGrantMcs; // MCS for UL grant (default 0)

  bool absFrame;
	uint16_t absCount;
	uint16_t absPeriod;
	uint16_t absFrames;
	uint16_t absInterval;
	bool absEndInterval;
	bool mNode;
	std::list <Ptr<RrFfMacScheduler> > picoRr; //pointer to ICIC neighbour, can be made into list to facilitate tighter grid
	Ptr <RrFfMacScheduler> macroRr; //pointer to ICIC neighbour, can be made into list to facilitate tighter grid
	bool absEnabled;
	std::list <uint16_t> victimList;
	uint8_t m_victimTreshold;
											  
	std::map <uint16_t, uint8_t> cqiNormal;  //mapping RNTI to CQI value.
	std::map <uint16_t, uint8_t> cqiProtected;
													  
	Ptr <LteEnbPhy> m_phy;

	struct ThroughputInfo
	{
		uint16_t rnti;
		double averageThroughput;
		double averageThroughputAbs;
		std::vector <double> throughputStream;
		std::vector <double> throughputAbs;
		Time startTime;
		Time absStart;
		double lastTbSize;
		//bool absActive;
	};  
		
	double averageThroughput;
	double averageThroughputAbs;
	std::vector <double> tbStream;
	Time absStart;
		
	std::vector <ThroughputInfo> ThroughputInfo_s;
};

} // namespace ns3

#endif /* RR_FF_MAC_SCHEDULER_H */
