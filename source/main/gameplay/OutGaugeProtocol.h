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

/** 
	@file   
	@author Thomas Fischer thomas{AT}thomasfischer{DOT}biz
	@date   17th of July 2011
*/

#pragma once

#include "RoRPrerequisites.h"

/// Implements support for OutGauge protocol: http://en.lfsmanual.net/index.php?title=OutSim_/_OutGauge
/// Windows-only
class OutGaugeProtocol : public ZeroedMemoryAllocator
{
public:

	OutGaugeProtocol();
	~OutGaugeProtocol();

	bool Update(float dt);

	inline bool IsActive() { return m_is_active; }

private:

	void Startup();

	bool    m_is_active;
	float   m_delay;
	float   m_timer;
	int     m_id;
	int     m_mode;
	int     m_sockfd;
};
