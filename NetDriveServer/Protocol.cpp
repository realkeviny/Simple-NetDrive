#include "Protocol.h"

PDU* makePDU(unsigned MsgLen)
{
	unsigned PDULen = sizeof(PDU) + MsgLen;
	PDU* pdu = (PDU*)malloc(PDULen);
	if (nullptr == pdu)
	{
		exit(EXIT_FAILURE);
	}
	memset(pdu, 0, PDULen);
	pdu->PDULen = PDULen;
	pdu->MsgLen = MsgLen;
	return pdu;
}