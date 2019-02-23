/*
**
** Copyright (C) 2009 Drew Technologies Inc.
** Author: Joey Oravec <joravec@drewtech.com>
**
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation, either version 3 of the License, or (at
** your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, <http://www.gnu.org/licenses/>.
**
*/

#include <stdio.h>
#include <shim_frontend.h>
#include <tchar.h>

int main(int argc, char * argv[])
{
	unsigned long ptStatus;
	unsigned long DeviceID;
	unsigned long ChannelID;
	unsigned long FilterID;
	PASSTHRU_MSG MaskMsg, PatternMsg, FlowControlMsg;
	PASSTHRU_MSG Msg[2];
	unsigned long numMsgs;

	// Load a J2534 library. It's assumed that the caller will parse the registry,
	// choose one of the installed devices, and pass the appropriate FunctionLibrary
	// string. This example is hardcoded for the CarDAQ+ to keep things simple. 
	ptStatus = PassThruLoadLibrary("c:\\windows\\system32\\cdpls432.dll");

	ptStatus = PassThruOpen(NULL, &DeviceID);

	// Add one line of debug text to the file. The output will be formatted like:
	//  0.019s ** 'Adding an ANSI line to debug log'
	// Notice the separate functions for A (ansi) and W (wide) character strings.
	PassThruWriteToLogA("Adding an ANSI line to debug log");
	PassThruWriteToLogW(_T("Adding a UNICODE line to debug log"));

	ptStatus = PassThruConnect(DeviceID, ISO15765, 0, 500000, &ChannelID);

	SCONFIG Config[9];
	Config[0].Parameter = ISO15765_BS;
	Config[1].Parameter = ISO15765_STMIN;
	Config[2].Parameter = BS_TX;
	Config[3].Parameter = STMIN_TX;
	Config[4].Parameter = DATA_RATE;
	Config[5].Parameter = LOOPBACK;
	Config[6].Parameter = BIT_SAMPLE_POINT;
	Config[7].Parameter = SYNC_JUMP_WIDTH;
	Config[8].Parameter = ISO15765_WFT_MAX;
	SCONFIG_LIST ConfigList;
	ConfigList.NumOfParams = 9;
	ConfigList.ConfigPtr = Config;
	ptStatus = PassThruIoctl(ChannelID, GET_CONFIG, &ConfigList, NULL);

	MaskMsg.ProtocolID = ISO15765;
	MaskMsg.TxFlags = ISO15765_FRAME_PAD;
	MaskMsg.Data[0] = 0xFF;
	MaskMsg.Data[1] = 0xFF;
	MaskMsg.Data[2] = 0xFF;
	MaskMsg.Data[3] = 0xFF;
	MaskMsg.DataSize = 4;
	PatternMsg.ProtocolID = ISO15765;
	PatternMsg.TxFlags = ISO15765_FRAME_PAD;
	PatternMsg.Data[0] = 0x00;
	PatternMsg.Data[1] = 0x00;
	PatternMsg.Data[2] = 0x07;
	PatternMsg.Data[3] = 0xE8;
	PatternMsg.DataSize = 4;
	FlowControlMsg.ProtocolID = ISO15765;
	FlowControlMsg.TxFlags = ISO15765_FRAME_PAD;
	FlowControlMsg.Data[0] = 0x00;
	FlowControlMsg.Data[1] = 0x00;
	FlowControlMsg.Data[2] = 0x07;
	FlowControlMsg.Data[3] = 0xE0;
	FlowControlMsg.DataSize = 4;
	ptStatus = PassThruStartMsgFilter(ChannelID, FLOW_CONTROL_FILTER, &MaskMsg, &PatternMsg, &FlowControlMsg, &FilterID);

	Msg[0].ProtocolID = ISO15765;
	Msg[0].TxFlags = ISO15765_FRAME_PAD;
	Msg[0].Data[0] = 0x00;
	Msg[0].Data[1] = 0x00;
	Msg[0].Data[2] = 0x07;
	Msg[0].Data[3] = 0xDF;
	Msg[0].Data[4] = 0x01;
	Msg[0].Data[5] = 0x00;
	Msg[0].DataSize = 6;
	numMsgs = 1;
	ptStatus = PassThruWriteMsgs(ChannelID, Msg, &numMsgs, 250);

	numMsgs = 2;
	ptStatus = PassThruReadMsgs(ChannelID, Msg, &numMsgs, 250);

	// Unload the J2534 DLL
	ptStatus = PassThruUnloadLibrary();

	// The following calls should return ERR_FAILED because no J2534 DLL is loaded
	ptStatus = PassThruStopMsgFilter(ChannelID, FilterID);
	ptStatus = PassThruDisconnect(ChannelID);
	ptStatus = PassThruClose(DeviceID);

	// Save the debug log to a UTF-8 encoded text file
	PassThruSaveLog("C:\\Users\\Public\\Documents\\logfile.txt");
}