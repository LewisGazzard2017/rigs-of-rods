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
	@date   11/2015
	@author Petr Ohlidal
*/

#pragma once

#include "RigDef_Prerequisites.h"
#include "ForwardDeclarations.h"

#include <boost/shared_ptr.hpp>

namespace RoR
{

class LuaSubsystem
{
public:
    static void BeginRigSetup(rig_t* rig);

    static void SetupRigClassicPowertrain(
        rig_t* rig,
        boost::shared_ptr<RigDef::Engine> engine_def,
        boost::shared_ptr<RigDef::Engoption> engoption,
        boost::shared_ptr<RigDef::Engturbo> engturbo,
        int vehicle_number,
        TorqueCurve* torque_curve
        );

    static void FinishRigSetup(rig_t* rig);
};

} // namespace RoR
