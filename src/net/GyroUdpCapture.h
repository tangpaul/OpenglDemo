#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <future>
#include <memory>

#include "pcap.h"

#include "IGyroUdpListener.h"
#include "GyroUdpDataAsync.h"

/* IPv4 address */
typedef struct ip_address{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

/* IPv4 header */
typedef struct ip_header{
	u_char  ver_ihl;        // Version (4 bits) + Internet header length (4 bits)
	u_char  tos;            // Type of service
	u_short tlen;           // Total length
	u_short identification; // Identification
	u_short flags_fo;       // Flags (3 bits) + Fragment offset (13 bits)
	u_char  ttl;            // Time to live
	u_char  proto;          // Protocol
	u_short crc;            // Header checksum
	ip_address  saddr;      // Source address
	ip_address  daddr;      // Destination address
	u_int   op_pad;         // Option + Padding
}ip_header;

/* UDP header*/
typedef struct udp_header{
	u_short sport;          // Source port
	u_short dport;          // Destination port
	u_short len;            // Datagram length
	u_short crc;            // Checksum
}udp_header;

class GyroUdpCapture : public IGyroUdpDataAsyncListener{
public:
	GyroUdpCapture(int fps = 100);
	~GyroUdpCapture();

	void StartListen(std::string devName, std::string idName = "");
	void Stop();

	void RegisterListener(IGyroUdpListener* listener);

	const std::vector<std::string>& GetDevNames(){ return m_devNames; }
	const std::vector<std::string>& GetDevDescription(){ return m_devDescription; }

	virtual void AsyncData(const float timestamp, const float* data) override;

private:
	void startAsync();
	void stopAsync();

private:
	char errbuf[PCAP_ERRBUF_SIZE];				//������Ϣ
	pcap_if_t* m_Alldevs	= nullptr;			//����������
	pcap_t* m_Adhandle		= nullptr;			//��ǰhandle��������

	std::vector<std::string> m_devNames;			//�豸��¼
	std::vector<std::string> m_devDescription;		//�豸����
	std::unordered_map<std::string, pcap_if_t*>	m_devStructMap;		//�豸���ƽṹ���ϣ��

	volatile bool forceStop = false;			//ǿ��ֹͣ��־λ

	IGyroUdpListener* m_Listener = nullptr;		//���ݼ���

	std::future<void>	m_dataAsyncFuture;			//����ͬ���߳�
	std::unique_ptr<GyroUdpDataAsync> m_dataAsync;	//����ͬ��

	const int m_fps	= 100;
};
