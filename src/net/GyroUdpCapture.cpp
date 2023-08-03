#include "GyroUdpCapture.h"
#include <thread>
#include <time.h>
#ifndef __linux__
#pragma comment(lib,"ws2_32.lib")
#endif

#include "glm/glm.hpp" 
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

GyroUdpCapture::GyroUdpCapture(int fps):
	m_fps(fps)
{
	//获取设备列表
#ifdef __linux__
	if(pcap_findalldevs(&m_Alldevs, errbuf) == -1)
#else
	if(pcap_findalldevs_ex((char*) PCAP_SRC_IF_STRING, NULL, &m_Alldevs, errbuf) == -1)
#endif
	{
		printf("[ERROR]:%s\n", errbuf);
		return;
	}

	//获取设备信息
	pcap_if_t* d; int devCount=0;
	for(d = m_Alldevs; d; d = d->next){
		std::string devname(d->name);
		m_devNames.push_back(devname);
		printf("Dev[%d]:%s\n", devCount++, devname);
		if(d->description)
			m_devDescription.push_back(d->description);
		else
			m_devDescription.push_back("(No description available)");

		if(m_devStructMap.find(devname) == m_devStructMap.end()) m_devStructMap[devname] = d;
	}

	//注册同步数据
	m_dataAsync = std::make_unique<GyroUdpDataAsync>(14, 2000, m_fps);
	m_dataAsync->registerListener(this);
}

GyroUdpCapture::~GyroUdpCapture(){
	this->stopAsync();
	if(m_Alldevs != nullptr) pcap_freealldevs(m_Alldevs);
}

void GyroUdpCapture::StartListen(std::string devName, std::string idName){
	/* 打开设备 */
#ifdef __linux__
	if((m_Adhandle = pcap_open_live(devName.c_str(), BUFSIZ, 1, 0, errbuf)) == NULL)
#else
	if((m_Adhandle = pcap_open(devName.c_str(),						// name of the device
										65536,						// portion of the packet to capture
																	// 65536 guarantees that the whole packet will be captured on all the link layers
										PCAP_OPENFLAG_PROMISCUOUS,  // promiscuous mode
										1000,						// read timeout
										NULL,						// authentication on the remote machine
										errbuf						// error buffer
									)) == NULL)
#endif
	{
		printf("\nUnable to open the adapter. %s is not supported by WinPcap\n", devName.c_str());
		return;
	}
	printf("Success to open the adapter: %s \n", devName.c_str());

	/*开启同步线程*/
	this->startAsync();

	//设备结构体
	pcap_if_t* d;	
	d = m_devStructMap[devName];
	do 
	{
		/* Check the link layer. We support only Ethernet for simplicity. */
		if(pcap_datalink(m_Adhandle) != DLT_EN10MB){
			printf("[ERROR] This program works only on Ethernet networks.\n");
			break;
		}

		u_int netmask = 0;
		#ifndef __linux__
		if(d->addresses != NULL)
			/* Retrieve the mask of the first address of the interface */
			netmask = ((struct sockaddr_in*) (d->addresses->netmask))->sin_addr.S_un.S_addr;
		else
			/* If the interface is without addresses we suppose to be in a C class network */
			netmask = 0xffffff;
		#endif // !__linux__
		

		/* compile the filter */
		char packet_filter[] = "udp and ip";
		struct bpf_program fcode;
		if(pcap_compile(m_Adhandle, &fcode, packet_filter, 1, netmask) < 0){
			printf("[ERROR] Unable to compile the packet filter. Check the syntax.\n");
			break;
		}

		/* set the filter */
		if(pcap_setfilter(m_Adhandle, &fcode) < 0){
			printf("[ERROR] Error setting the filter.");
			break;
		}

		printf("Start listening......");
		/* open a file */
		time_t t = time(NULL);
		char dateTime[32] = { NULL };
		strftime(dateTime, sizeof(dateTime), "%Y%m%d_%H%M%S", localtime(&t));
		std::string filename(dateTime);
		filename = "./" + filename + "_" + idName + "_" + std::to_string(m_fps) + "HZ" + ".csv";
		std::ofstream outfile;
		outfile.open(filename, std::ios::binary | std::ios::app | std::ios::in | std::ios::out);

		/* Retrieve the packets */
		int res = 0;
		struct pcap_pkthdr* header;
		const u_char* pkt_data;
		//time_t local_tv_sec;
		struct tm ltime;
		//char timestr[16];
		char timestamp[255];
		forceStop = false;
		volatile bool isfirstPackage = true;
		static volatile long firstPackageTimeStamp = 1664522082;
		while((res = pcap_next_ex(m_Adhandle, &header, &pkt_data)) >= 0){
			if(forceStop) break;

			if(res == 0) continue;

			if(header->len != 147 && header->len != 173) continue;

			//local_tv_sec = header->ts.tv_sec;
			//localtime_s(&ltime, &local_tv_sec);
			//strftime(timestr, sizeof timestr, "%H:%M:%S", &ltime);
			if(isfirstPackage){
				firstPackageTimeStamp = header->ts.tv_sec;
				isfirstPackage = false;
			}

			//local_tv_sec = header->ts.tv_sec;
			//localtime_s(&ltime, &local_tv_sec);
			//strftime(timestr, sizeof timestr, "%H:%M:%S", &ltime);
			sprintf(timestamp, "%ld.%06ld", header->ts.tv_sec - firstPackageTimeStamp, header->ts.tv_usec);
			//printf("[%s.%ld] len:%d\n", timestr, header->ts.tv_usec, header->len);

			/* retireve the position of the ip header */
			ip_header* ih;
			ih = (ip_header*) (pkt_data + 14); //length of ethernet header

			/* retireve the position of the udp header */
			u_int ip_len;
			udp_header* uh;
			ip_len = (ih->ver_ihl & 0xf) * 4;
			uh = (udp_header*) ((u_char*) ih + ip_len);

			/* convert from network byte order to host byte order */
			// u_short sport, dport;
			// sport = ntohs(uh->sport);
			// dport = ntohs(uh->dport);

			/* print ip addresses and udp ports */
			//printf("%d.%d.%d.%d.%d -> %d.%d.%d.%d.%d\n",
			//	ih->saddr.byte1,ih->saddr.byte2,ih->saddr.byte3,ih->saddr.byte4,sport,
			//	ih->daddr.byte1,ih->daddr.byte2,ih->daddr.byte3,ih->daddr.byte4,dport);

			//浮点数
			const float* pFloatsBegin = (const float*) &pkt_data[48];

			if (m_Listener != nullptr)
			{
				if(header->len == 147) m_Listener->reciveTimeCode(pkt_data[142], pkt_data[143], pkt_data[144], pkt_data[145]);
				m_Listener->reciveQuat(pFloatsBegin[3], pFloatsBegin[4], pFloatsBegin[5], pFloatsBegin[6]);
				m_Listener->recive3AxisParams( std::stof(timestamp),
												pFloatsBegin[0],	pFloatsBegin[1],	pFloatsBegin[2],
												pFloatsBegin[8],	pFloatsBegin[9],	pFloatsBegin[10],
												pFloatsBegin[11],	pFloatsBegin[12],	pFloatsBegin[13]);
				m_Listener->reciveError(pFloatsBegin[7]);
				m_Listener->saveALine(outfile, timestamp);
				m_dataAsync->product(std::stof(timestamp), pFloatsBegin);
			}
		}
		outfile.close();
	} while (0);

	this->stopAsync();

	pcap_close(m_Adhandle);

	printf("Listening stopped.\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void GyroUdpCapture::Stop(){
	forceStop = true;
}

void GyroUdpCapture::RegisterListener(IGyroUdpListener* listener){
	m_Listener = listener;
}

void GyroUdpCapture::AsyncData(const float timestamp, const float* data){
	if(m_Listener != nullptr){
		glm::quat q4 = glm::quat(data[3], data[4], data[5], data[6]);
		glm::mat4 model = glm::toMat4(q4);
		float eulerX = 0.0f, eulerY = 0.0f, eulerZ = 0.0f;
		glm::extractEulerAngleXYZ(model, eulerX, eulerY, eulerZ);

		m_Listener->recive3AxisParamsStable(timestamp,
			data[0], data[1], data[2],
			data[8], data[9], data[10],
			data[11], data[12], data[13]);
		//m_Listener->recive3AxisParamsStable(timestamp,
		//									eulerX,		eulerY,		eulerZ,
		//									data[8],	data[9],	data[10],
		//									data[11],	data[12],	data[13]);
		//printf("timestamp: %.6f\n", interval);
	}
}

void GyroUdpCapture::startAsync(){
	m_dataAsyncFuture = std::async(&GyroUdpDataAsync::startAsync, m_dataAsync.get());
}

void GyroUdpCapture::stopAsync(){
	if(m_dataAsyncFuture.valid() && m_dataAsyncFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready){
		m_dataAsync->stopAsync();
		m_dataAsyncFuture.wait();
	}
}
