#include "stdafx.h"
#include "MatrixServerDll.h"

// 创建ADC参数设置帧信息结构体
m_oADCSetFrameStruct* OnCreateInstrADCSetFrame(void)
{
	m_oADCSetFrameStruct* pADCSetFrame = NULL;
	pADCSetFrame = new m_oADCSetFrameStruct;
	InitializeCriticalSection(&pADCSetFrame->m_oSecADCSetFrame);
	pADCSetFrame->m_cpRcvFrameData = NULL;
	pADCSetFrame->m_cpSndFrameData = NULL;
	pADCSetFrame->m_pbyCommandWord = NULL;
	pADCSetFrame->m_oADCSetFrameSocket = INVALID_SOCKET;
	pADCSetFrame->m_pCommandStructSet = NULL;
	pADCSetFrame->m_pCommandStructReturn = NULL;
	pADCSetFrame->m_usPortMove = 0;
	return pADCSetFrame;
}
// 初始化ADC参数设置帧
void OnInitInstrADCSetFrame(m_oADCSetFrameStruct* pADCSetFrame,
	m_oInstrumentCommInfoStruct* pCommInfo, m_oConstVarStruct* pConstVar)
{
	ASSERT(pADCSetFrame != NULL);
	ASSERT(pCommInfo != NULL);
	ASSERT(pConstVar != NULL);
	EnterCriticalSection(&pADCSetFrame->m_oSecADCSetFrame);
	if (pADCSetFrame->m_pCommandStructSet != NULL)
	{
		delete pADCSetFrame->m_pCommandStructSet;
		pADCSetFrame->m_pCommandStructSet = NULL;
	}
	pADCSetFrame->m_pCommandStructSet = new m_oInstrumentCommandStruct;
	// 源地址
	pADCSetFrame->m_pCommandStructSet->m_uiSrcIP = pCommInfo->m_pServerSetupData->m_oXMLIPSetupData.m_uiSrcIP;
	// 目的地址
	pADCSetFrame->m_pCommandStructSet->m_uiAimIP = pCommInfo->m_pServerSetupData->m_oXMLIPSetupData.m_uiAimIP;
	// 目标IP地址端口号
	pADCSetFrame->m_pCommandStructSet->m_usAimPort = pCommInfo->m_pServerSetupData->m_oXMLPortSetupData.m_usAimPort
		+ pCommInfo->m_pServerSetupData->m_oXMLParameterSetupData.m_usNetRcvPortMove;
	// ADC参数设置发送缓冲区帧数设定为仪器个数
	pADCSetFrame->m_uiSndBufferSize = pConstVar->m_iInstrumentNum * pConstVar->m_iSndFrameSize;
	// ADC参数设置应答接收缓冲区帧数设定为仪器个数
	pADCSetFrame->m_uiRcvBufferSize = pConstVar->m_iInstrumentNum * pConstVar->m_iRcvFrameSize;
	// ADC参数设置返回端口
	pADCSetFrame->m_pCommandStructSet->m_usReturnPort = pCommInfo->m_pServerSetupData->m_oXMLPortSetupData.m_usADCSetReturnPort;
	// ADC参数设置接收端口偏移量
	pADCSetFrame->m_usPortMove = pCommInfo->m_pServerSetupData->m_oXMLParameterSetupData.m_usNetRcvPortMove;
	// 重置帧内通讯信息
	// 命令，为1则设置命令应答，为2查询命令应答，为3AD采样数据重发
	pADCSetFrame->m_pCommandStructSet->m_usCommand = pConstVar->m_usSendSetCmd;
	// 重置帧内容解析变量
	ResetInstrFramePacket(pADCSetFrame->m_pCommandStructSet);
	// ADC数据返回地址
	pADCSetFrame->m_pCommandStructSet->m_uiADCDataReturnAddr = pCommInfo->m_pServerSetupData->m_oXMLIPSetupData.m_uiADCDataReturnAddr;
	// ADC数据返回端口
	pADCSetFrame->m_pCommandStructSet->m_usADCDataReturnPort = pCommInfo->m_pServerSetupData->m_oXMLPortSetupData.m_usADCDataReturnPort;
	// 自动数据返回命令，ad_cmd(7)=1，端口递增；=0，端口不变
	pADCSetFrame->m_pCommandStructSet->m_usADCDataReturnCmd = pConstVar->m_usSendADCCmd;
	// 端口递增下限
	pADCSetFrame->m_pCommandStructSet->m_usADCDataReturnPortLimitLow = pCommInfo->m_pServerSetupData->m_oXMLPortSetupData.m_usADCDataReturnPort;
	// 端口递增上限
	pADCSetFrame->m_pCommandStructSet->m_usADCDataReturnPortLimitHigh = pCommInfo->m_pServerSetupData->m_oXMLPortSetupData.m_usADCDataReturnPort;
	// 清空发送帧缓冲区
	if (pADCSetFrame->m_cpSndFrameData != NULL)
	{
		delete[] pADCSetFrame->m_cpSndFrameData;
		pADCSetFrame->m_cpSndFrameData = NULL;
	}
	pADCSetFrame->m_cpSndFrameData = new char[pConstVar->m_iSndFrameSize];
	memset(pADCSetFrame->m_cpSndFrameData, pConstVar->m_cSndFrameBufInit, pConstVar->m_iSndFrameSize);
	// 清空时统设置命令字集合
	if (pADCSetFrame->m_pbyCommandWord != NULL)
	{
		delete[] pADCSetFrame->m_pbyCommandWord;
		pADCSetFrame->m_pbyCommandWord = NULL;
	}
	pADCSetFrame->m_pbyCommandWord = new BYTE[pConstVar->m_iCommandWordMaxNum];
	memset(pADCSetFrame->m_pbyCommandWord, pConstVar->m_cSndFrameBufInit, pConstVar->m_iCommandWordMaxNum);
	// ADC参数设置命令字个数
	pADCSetFrame->m_usCommandWordNum = 0;

	// 重置帧内容解析变量
	if (pADCSetFrame->m_pCommandStructReturn != NULL)
	{
		delete pADCSetFrame->m_pCommandStructReturn;
		pADCSetFrame->m_pCommandStructReturn = NULL;
	}
	pADCSetFrame->m_pCommandStructReturn = new m_oInstrumentCommandStruct;
	ResetInstrFramePacket(pADCSetFrame->m_pCommandStructReturn);
	pADCSetFrame->m_pCommandStructReturn->m_cpADCSet = new char[pConstVar->m_iSndFrameSize];
	// 清空接收帧缓冲区
	if (pADCSetFrame->m_cpRcvFrameData != NULL)
	{
		delete[] pADCSetFrame->m_cpRcvFrameData;
		pADCSetFrame->m_cpRcvFrameData = NULL;
	}
	pADCSetFrame->m_cpRcvFrameData = new char[pConstVar->m_iRcvFrameSize];
	memset(pADCSetFrame->m_cpRcvFrameData, pConstVar->m_cSndFrameBufInit, pConstVar->m_iRcvFrameSize);
	LeaveCriticalSection(&pADCSetFrame->m_oSecADCSetFrame);
}
// 关闭ADC参数设置帧信息结构体
void OnCloseInstrADCSetFrame(m_oADCSetFrameStruct* pADCSetFrame)
{
	ASSERT(pADCSetFrame != NULL);
	EnterCriticalSection(&pADCSetFrame->m_oSecADCSetFrame);
	if (pADCSetFrame->m_cpSndFrameData != NULL)
	{
		delete[] pADCSetFrame->m_cpSndFrameData;
		pADCSetFrame->m_cpSndFrameData = NULL;
	}
	if (pADCSetFrame->m_pbyCommandWord != NULL)
	{
		delete[] pADCSetFrame->m_pbyCommandWord;
		pADCSetFrame->m_pbyCommandWord = NULL;
	}
	if (pADCSetFrame->m_cpRcvFrameData != NULL)
	{
		delete[] pADCSetFrame->m_cpRcvFrameData;
		pADCSetFrame->m_cpRcvFrameData = NULL;
	}
	if (pADCSetFrame->m_pCommandStructSet != NULL)
	{
		delete pADCSetFrame->m_pCommandStructSet;
		pADCSetFrame->m_pCommandStructSet = NULL;
	}
	if (pADCSetFrame->m_pCommandStructReturn != NULL)
	{
		if (pADCSetFrame->m_pCommandStructReturn->m_cpADCSet != NULL)
		{
			delete[] pADCSetFrame->m_pCommandStructReturn->m_cpADCSet;
			pADCSetFrame->m_pCommandStructReturn->m_cpADCSet = NULL;
		}
		delete pADCSetFrame->m_pCommandStructReturn;
		pADCSetFrame->m_pCommandStructReturn = NULL;
	}
	LeaveCriticalSection(&pADCSetFrame->m_oSecADCSetFrame);
}
// 释放ADC参数设置帧信息结构体
void OnFreeInstrADCSetFrame(m_oADCSetFrameStruct* pADCSetFrame)
{
	ASSERT(pADCSetFrame != NULL);
	DeleteCriticalSection(&pADCSetFrame->m_oSecADCSetFrame);
	delete pADCSetFrame;
	pADCSetFrame = NULL;
}
// 创建并设置ADC参数设置端口
void OnCreateAndSetADCSetFrameSocket(m_oADCSetFrameStruct* pADCSetFrame, m_oLogOutPutStruct* pLogOutPut)
{
	ASSERT(pADCSetFrame != NULL);
	EnterCriticalSection(&pADCSetFrame->m_oSecADCSetFrame);
	// 创建套接字
	pADCSetFrame->m_oADCSetFrameSocket = CreateInstrSocket(pADCSetFrame->m_pCommandStructSet->m_usReturnPort + pADCSetFrame->m_usPortMove, 
		pADCSetFrame->m_pCommandStructSet->m_uiSrcIP, pLogOutPut);
	// 设置为广播端口
	SetInstrSocketBroadCast(pADCSetFrame->m_oADCSetFrameSocket, pLogOutPut);
	// 设置发送缓冲区
	SetSndBufferSize(pADCSetFrame->m_oADCSetFrameSocket, pADCSetFrame->m_uiSndBufferSize, pLogOutPut);
	// 设置接收缓冲区大小
	SetRcvBufferSize(pADCSetFrame->m_oADCSetFrameSocket, pADCSetFrame->m_uiRcvBufferSize, pLogOutPut);
	LeaveCriticalSection(&pADCSetFrame->m_oSecADCSetFrame);
	AddMsgToLogOutPutList(pLogOutPut, "OnCreateAndSetADCSetFrameSocket", "创建并设置ADC参数设置帧端口！");
}
// 解析ADC参数设置应答帧
bool ParseInstrADCSetReturnFrame(m_oADCSetFrameStruct* pADCSetFrame, m_oConstVarStruct* pConstVar, m_oLogOutPutStruct* pLogOutPut)
{
	ASSERT(pLogOutPut != NULL);
	ASSERT(pADCSetFrame != NULL);
	ASSERT(pConstVar != NULL);
	bool bReturn = false;
	EnterCriticalSection(&pADCSetFrame->m_oSecADCSetFrame);
	bReturn = ParseInstrFrame(pADCSetFrame->m_pCommandStructReturn, 
		pADCSetFrame->m_cpRcvFrameData, pConstVar, pLogOutPut);
	LeaveCriticalSection(&pADCSetFrame->m_oSecADCSetFrame);
	return bReturn;
}