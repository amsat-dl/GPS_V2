#include <stdio.h>

// UBX-CFG-TP5 message
#define GPSBASEQRG 2000000
#define GPSQRG (GPSBASEQRG + 0)

unsigned char gps_config_data[] = 
{
	0,		// time pulse selection: 0=TP, 1=TP2
	1,		// message version
	0,		// reserved
	0,
	50,		// Antenne cable delay in ns
	0,
	0,		// RF group delay
	0,
	
	(unsigned char)(GPSQRG & 0xff),
	(unsigned char)((GPSQRG>>8)&0xff),
	(unsigned char)((GPSQRG>>16)&0xff),
	(unsigned char)((GPSQRG>>24)&0xff),
	// ATTENTION !
	// DO NOT shift these bytes, they are accessed by function: clear_GPS_shift() !!!
	// to avoid a beat ("Schwebung") between the UBLOX internal clock and the TIMEPULSE output
	// we need a small difference between the wanted clock and the real clock
	(unsigned char)(GPSQRG & 0xff),
	(unsigned char)((GPSQRG>>8)&0xff),
	(unsigned char)((GPSQRG>>16)&0xff),
	(unsigned char)((GPSQRG>>24)&0xff),
	
	0,		// pulse len ratio = 0%
	0,
	0,
	0x80,
	0,		// pulse len ratio if locked: 50%
	0,		// "islength" bit is set to 0, so this value is the duty cycle
	0,
	0x80,
	
	0,		// user conf. time pulse delay
	0,
	0,
	0,
	0xef,	// flages, see below
	0,
	0,
	0
};

/*
 * Flags in above message:
 * bit 0 ... active ...			1=enable time pulse output
 * bit 1 ... lockGnssFreq ...	1=lock if GNSS is valid, otherwise use local clock
 * bit 2 ... lockedOtherSet ...	1=use the LOCKED parameters if locked
 * bit 3 ... isFreq ...			1=interpret above parameters as frequency
 * bit 4 ... isLength ...		0=interpret above parameters as duty cycle
 * bit 5 ... alignToTow ...		1=align to top of a second, this is always set by default in FTL products
 * bit 6 ... polarity ...		1 (we don't care)
 * bit 7-10. gridUtcGnss ...	1=use GPS time
 * bit11-13. syncMode ...		0=if locked use locked parameters, never switch back to unlocked parameters even if not locked 
 */


// activate 10 MHz output in ublox module
// only every 2 seconds and only if not done already
// do this 100 times max.

void send_UBX_string(unsigned char class, unsigned char id, unsigned short len, unsigned char *payload)
{
	unsigned char txbuf[50];
	int idx = 0;
	
	txbuf[idx++] = 0xb5;
	txbuf[idx++] = 0x62;
	txbuf[idx++] = class;
	txbuf[idx++] = id;
	txbuf[idx++] = (unsigned char)(len & 0xff);
	txbuf[idx++] = (unsigned char)(len >> 8);
	for(int i=0; i<len; i++)
	{
		txbuf[idx++] = payload[i];
	}
	
	// calc chksum
	unsigned char CK_A = 0, CK_B = 0;
	for(int i=2; i<idx; i++)
	{
		CK_A = CK_A + txbuf[i];
		CK_B = CK_B + CK_A;
	}
	txbuf[idx++] = CK_A;
	txbuf[idx++] = CK_B;
	
	if(idx >= 50)
		return; // send_UBX_string: idx too long
	
	for(int i=0; i<idx; i++)
		printf("0x%02X, ",txbuf[i]);
	
	printf("\n");
}

void main()
{
	send_UBX_string(6,0x31,sizeof(gps_config_data),gps_config_data);
}
