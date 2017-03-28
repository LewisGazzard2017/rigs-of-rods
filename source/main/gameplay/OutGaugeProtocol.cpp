/*
	This source file is part of Rigs of Rods
	Copyright 2005-2012 Pierre-Michel Ricordel
	Copyright 2007-2012 Thomas Fischer
	Copyright 2013-2015 Petr Ohlidal

	For more information, see http://www.rigsofrods.com/

	Rigs of Rods is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 3, as
	published by the Free Software Foundation.

	Rigs of Rods is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

#include "OutGaugeProtocol.h"

#include "BeamEngine.h"
#include "BeamFactory.h"
#include "RoRVersion.h"
#include "Settings.h"

#include <Ogre.h>

#ifdef WIN32
#include <Ws2tcpip.h>
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#else
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif //WIN32

#ifdef USE_SOCKETW
#include "SocketW.h"
#endif //USE_SOCKETW

// from LFS/doc/insim.txt
enum
{
	OG_SHIFT      = 1,           // key
	OG_CTRL       = 2,           // key
	OG_TURBO      = 8192,        // show turbo gauge
	OG_KM         = 16384,       // if not set - user prefers MILES
	OG_BAR        = 32768,       // if not set - user prefers PSI
};

enum
{
	DL_SHIFT      = BITMASK(1),  // bit 0	- shift light
	DL_FULLBEAM   = BITMASK(2),	 // bit 1	- full beam
	DL_HANDBRAKE  = BITMASK(3),	 // bit 2	- handbrake
	DL_PITSPEED   = BITMASK(4),	 // bit 3	- pit speed limiter
	DL_TC         = BITMASK(5),	 // bit 4	- TC active or switched off
	DL_SIGNAL_L   = BITMASK(6),	 // bit 5	- left turn signal
	DL_SIGNAL_R   = BITMASK(7),	 // bit 6	- right turn signal
	DL_SIGNAL_ANY = BITMASK(8),  // bit 7	- shared turn signal
	DL_OILWARN    = BITMASK(9),  // bit 8	- oil pressure warning
	DL_BATTERY    = BITMASK(10), // bit 9	- battery warning
	DL_ABS        = BITMASK(11), // bit 10	- ABS active or switched off
	DL_SPARE      = BITMASK(12), // bit 11
	DL_NUM        = BITMASK(13)  // bit 14  - end
};

PACK (struct OutGaugePack
{
	unsigned int   Time;         // time in milliseconds (to check order)
	char           Car[4];       // Car name
	unsigned short Flags;        // Info (see OG_x below)
	unsigned char  Gear;         // Reverse:0, Neutral:1, First:2...
	unsigned char  PLID;         // Unique ID of viewed player (0 = none)
	float		   Speed;		 // M/S
	float		   RPM;		     // RPM
	float		   Turbo;		 // BAR
	float		   EngTemp;		 // C
	float		   Fuel;		 // 0 to 1
	float		   OilPressure;  // BAR
	float		   OilTemp;		 // C
	unsigned int   DashLights;   // Dash lights available (see DL_x below)
	unsigned int   ShowLights;   // Dash lights currently switched on
	float		   Throttle;     // 0 to 1
	float		   Brake;        // 0 to 1
	float		   Clutch;       // 0 to 1
	char           Display1[16]; // Usually Fuel
	char           Display2[16]; // Usually Settings
	int			   ID;           // optional - only if OutGauge ID is specified
});

using namespace Ogre;

OutGaugeProtocol::OutGaugeProtocol(void) : 
	  m_delay(0.1f)
	, m_id(0)
	, m_mode(0)
	, m_sockfd(-1)
	, m_timer(0)
	, m_is_active(false)
{
	m_delay *= FSETTING("OutGauge Delay", 10);
	m_mode   = ISETTING("OutGauge Mode", 0);
	m_id     = ISETTING("OutGauge ID", 0);

	if ( m_mode > 0 )
	{
		Startup();
	}
}

OutGaugeProtocol::~OutGaugeProtocol(void)
{
	if ( m_sockfd != 0 )
	{
#if WIN32
		closesocket( m_sockfd );
#else
		close( m_sockfd );
#endif
		m_sockfd = 0;
	}
}

void OutGaugeProtocol::Startup()
{
#ifdef WIN32
	SWBaseSocket::SWBaseError error;

	// get some settings
	String ipstr = SSETTING("OutGauge IP", "192.168.1.100");
	int port     = ISETTING("OutGauge Port", 1337);
	
	// startup winsock
	WSADATA wsd;
	if ( WSAStartup(MAKEWORD(2, 2), &wsd) != 0 )
	{
		LOG("error starting up winsock");
		return;
	}

	// open a new socket
	if ( (m_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
	{
		LOG(String("error creating socket for OutGauge: ").append(strerror(errno)));
		return;
	}

	// get the IP of the remote side, this function is compatible with windows 2000
	hostent *remoteHost = gethostbyname(ipstr.c_str());
	char *ip = inet_ntoa(*(struct in_addr *)*remoteHost->h_addr_list);

	// init socket data
	struct sockaddr_in sendaddr;
	memset(&sendaddr, 0, sizeof(sendaddr));
	sendaddr.sin_family      = AF_INET;
	sendaddr.sin_addr.s_addr = inet_addr(ip);
	sendaddr.sin_port        = htons(port);

	// connect
	if ( connect(m_sockfd, (struct sockaddr *) &sendaddr, sizeof(sendaddr)) == SOCKET_ERROR )
	{
		LOG(String("error connecting socket for OutGauge: ").append(strerror(errno)));
		return;
	}

	LOG("OutGauge connected successfully");
	m_is_active = true;
#else
	// TODO: fix linux
#endif //WIN32
}

bool OutGaugeProtocol::Update(float dt)
{
#ifdef WIN32
	if ( !m_is_active )
	{
		return false;
	}

	// below the set delay?
	m_timer += dt;
	if ( m_timer < m_delay )
	{
		return true;
	}
	m_timer = 0;

	// send a package
	OutGaugePack gd;
	memset(&gd, 0, sizeof(gd));

	// set some common things
	gd.Time  = Root::getSingleton().getTimer()->getMilliseconds();
	gd.ID    = m_id;
	gd.Flags = 0 | OG_KM;
	sprintf(gd.Car, "RoR");

	Beam *truck = BeamFactory::getSingleton().getCurrentTruck();
	if ( !truck )
	{
		// not in a truck?
		sprintf(gd.Display2, "not in vehicle");
	} 
	else if ( truck && !truck->engine )
	{
		// no engine?
		sprintf(gd.Display2, "no engine");
	} 
	else if ( truck && truck->engine )
	{
		// truck and engine valid
		if ( truck->engine->hasTurbo() )
		{
			gd.Flags |= OG_TURBO;
		}
		gd.Gear        = std::max(0, truck->engine->getGear() + 1); // we only support one reverse gear
		gd.PLID        = 0;
		gd.Speed       = fabs(truck->WheelSpeed);
		gd.RPM         = truck->engine->getRPM();
		gd.Turbo       = truck->engine->getTurboPSI() * 0.0689475729f;
		gd.EngTemp     = 0; // TODO
		gd.Fuel        = 0; // TODO
		gd.OilPressure = 0; // TODO
		gd.OilTemp     = 0; // TODO

		gd.DashLights  = 0;
		gd.DashLights |= DL_HANDBRAKE;
		gd.DashLights |= DL_BATTERY;
		gd.DashLights |= DL_SIGNAL_L;
		gd.DashLights |= DL_SIGNAL_R;
		gd.DashLights |= DL_SIGNAL_ANY;
		if (truck->tc_present)   gd.DashLights |= DL_TC;
		if (truck->alb_present)  gd.DashLights |= DL_ABS;

		gd.ShowLights = 0;
		if (truck->parkingbrake)   gd.ShowLights |= DL_HANDBRAKE;
		if (truck->lights)         gd.ShowLights |= DL_FULLBEAM;
		if (truck->engine->hasContact() && !truck->engine->isRunning()) gd.ShowLights |=  DL_BATTERY;
		if (truck->left_blink_on)  gd.ShowLights |= DL_SIGNAL_L;
		if (truck->right_blink_on) gd.ShowLights |= DL_SIGNAL_R;
		if (truck->warn_blink_on)  gd.ShowLights |= DL_SIGNAL_ANY;
		if (truck->tc_mode)        gd.ShowLights |= DL_TC;
		if (truck->alb_mode)       gd.ShowLights |= DL_ABS;

		gd.Throttle = truck->engine->getAcc();
		gd.Brake    = truck->brake / truck->brakeforce;
		gd.Clutch   = 1 - truck->engine->getClutch(); // 0-1

		strncpy(gd.Display1, truck->realtruckname.c_str(), 15);
		if ( truck->realtruckname.length() > 15 )
		{
			strncpy(gd.Display2, truck->realtruckname.c_str() + 15, 15);
		}
	}
	// send the package
	send(m_sockfd, (const char*)&gd, sizeof(gd), NULL);

	return true;
#else
	// TODO: fix linux
	return false;
#endif //WIN32
}
