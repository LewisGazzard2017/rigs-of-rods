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

#include "LuaSubsystem.h"

#include "rig_t.h"
#include "RigDef_File.h"
#include "Settings.h"
#include "SoundScriptManager.h"
#include "TorqueCurve.h"

#include "lua.hpp"
#include "Diluculum/LuaState.hpp"
#include "Diluculum/LuaVariable.hpp"
#include "Diluculum/LuaWrappers.hpp"

using namespace RoR;
using namespace Diluculum;

LuaValueList LUA_LogMessage(const LuaValueList& args)
{
    if (args.size() == 1)
    {
        LOG(args[0].asString());
    }
    return LuaValueList();
}
DILUCULUM_WRAP_FUNCTION(LUA_LogMessage)

LuaValueList LUA_EvaluateLegacyTorqueCurve(const LuaValueList& args)
{
    LuaValueList returnee;
    if (args.size() == 2)
    {
        LuaUserData tc_userdata = args[0].asUserData();
        TorqueCurve* torque_curve = *static_cast<TorqueCurve**>(tc_userdata.getData());
        float rpm_ratio = args[1].asNumber();

        float torque = torque_curve->getEngineTorque(rpm_ratio);
        returnee.push_back(LuaValue(torque));
    }
    return returnee;
}
DILUCULUM_WRAP_FUNCTION(LUA_EvaluateLegacyTorqueCurve)

LuaValueList LUA_SoundScriptModulate(const LuaValueList& args)
{
    if (args.size() == 3)
    {
        int vehicle_index     = args[0].asInteger();
        int modulation_source = args[1].asInteger();
        float value           = args[2].asNumber();

        SoundScriptManager::getSingleton().modulate(vehicle_index, SS_MOD_GEARBOX, value);
    }
    return LuaValueList();
}
DILUCULUM_WRAP_FUNCTION(LUA_SoundScriptModulate)

LuaValueList LUA_SoundScriptTrigger(const LuaValueList& args)
{
    if (args.size() == 3)
    {
        int vehicle_index  = args[0].asInteger();
        int trigger_source = args[1].asInteger();
        int trigger_type   = args[2].asInteger();

        if (trigger_type == 1)
        {
            SoundScriptManager::getSingleton().trigOnce(vehicle_index, trigger_source);
        }
        else if (trigger_type == 2)
        {
            SoundScriptManager::getSingleton().trigToggle(vehicle_index, trigger_source);
        }
        else if (trigger_type == 3)
        {
            SoundScriptManager::getSingleton().trigStart(vehicle_index, trigger_source);
        }
        else if (trigger_type == 4)
        {
            SoundScriptManager::getSingleton().trigStop(vehicle_index, trigger_source);
        }
    }
    return LuaValueList();
}
DILUCULUM_WRAP_FUNCTION(LUA_SoundScriptTrigger)

int LuaPanicFunction(lua_State* L)
{
    const char* err_msg = nullptr;
    int error_msg_index = lua_gettop(L);
    if( error_msg_index != 0 && lua_isstring(L, error_msg_index))
    {
        err_msg = lua_tostring(L, error_msg_index);
    }
    if (err_msg == nullptr)
    {
        const char* msg = "Lua panic occurred - no error message available.";
        LOG("===========================================================");
        LOG(msg);
        LOG("===========================================================");
        throw std::runtime_error(msg);
        return 0;
    }
    std::string err_str("Lua panic ocurred, message: ");
    err_str += err_msg;
    LOG("===========================================================");
    LOG(err_str);
    LOG("===========================================================");
    throw std::runtime_error(err_str);
    return 0;
}

// static
void LuaSubsystem::BeginRigSetup(rig_t* rig)
{
    rig->lua_state_machine = new Diluculum::LuaState();
    lua_atpanic(rig->lua_state_machine->getState(), LuaPanicFunction);

    // Setup system API
    LuaValue module_ror = EmptyTable;
    module_ror[LuaValue("log_message")]                  = DILUCULUM_WRAPPER_FUNCTION(LUA_LogMessage);
    module_ror[LuaValue("evaluate_legacy_torque_curve")] = DILUCULUM_WRAPPER_FUNCTION(LUA_EvaluateLegacyTorqueCurve);
    module_ror[LuaValue("sound_script_modulate")]        = DILUCULUM_WRAPPER_FUNCTION(LUA_SoundScriptModulate);
    module_ror[LuaValue("sound_script_trigger")]         = DILUCULUM_WRAPPER_FUNCTION(LUA_SoundScriptTrigger);

    module_ror[LuaValue("scripts_path")] = SSETTING("Simulation Scripts Path", "");

    (*rig->lua_state_machine)["RoR"] = module_ror;
}

// Static
void LuaSubsystem::SetupRigClassicPowertrain(
    rig_t* rig,
    boost::shared_ptr<RigDef::Engine> engine_def,
    boost::shared_ptr<RigDef::Engoption> engoption,
    boost::shared_ptr<RigDef::Engturbo> engturbo,
    int vehicle_number,
    TorqueCurve* torque_curve
    )
{
    LuaValue lua_truckfile = EmptyTable;

    if (engine_def)
    {
        LuaValue lua_engine = EmptyTable;
        lua_engine[LuaValue("shift_down_rpm")]     = LuaValue(engine_def->shift_down_rpm);
        lua_engine[LuaValue("shift_up_rpm")]       = LuaValue(engine_def->shift_up_rpm);
        lua_engine[LuaValue("torque")]             = LuaValue(engine_def->torque);
        lua_engine[LuaValue("global_gear_ratio")]  = LuaValue(engine_def->global_gear_ratio);
        lua_engine[LuaValue("reverse_gear_ratio")] = LuaValue(engine_def->reverse_gear_ratio);
        lua_engine[LuaValue("neutral_gear_ratio")] = LuaValue(engine_def->neutral_gear_ratio);

        LuaValue lua_gear_ratios = EmptyTable;
        unsigned const num_gears = engine_def->gear_ratios.size();
        for (unsigned i = 0; i < num_gears; ++i)
        {
            lua_gear_ratios[LuaValue(i)] = LuaValue(engine_def->gear_ratios[i]);
        }
        lua_engine[LuaValue("forward_gears")] = lua_gear_ratios;

        lua_truckfile[LuaValue("engine")] = lua_engine;
    }

    if (engoption)
    {
        LuaValue lua_engoption = EmptyTable;
        char type[5];
        sprintf(type, "%c", (char)engoption->type);

        lua_engoption[LuaValue("type")]                      = LuaValue(type);
        lua_engoption[LuaValue("inertia")]                   = LuaValue(engoption->inertia);
        lua_engoption[LuaValue("clutch_force")]              = LuaValue(engoption->clutch_force);
        lua_engoption[LuaValue("_clutch_force_use_default")] = LuaValue(engoption->_clutch_force_use_default);
        lua_engoption[LuaValue("shift_time")]                = LuaValue(engoption->shift_time);
        lua_engoption[LuaValue("clutch_time")]               = LuaValue(engoption->clutch_time);
        lua_engoption[LuaValue("post_shift_time")]           = LuaValue(engoption->post_shift_time);
        lua_engoption[LuaValue("idle_rpm")]                  = LuaValue(engoption->idle_rpm);
        lua_engoption[LuaValue("_idle_rpm_use_default")]     = LuaValue(engoption->_idle_rpm_use_default);
        lua_engoption[LuaValue("stall_rpm")]                 = LuaValue(engoption->stall_rpm);
        lua_engoption[LuaValue("max_idle_mixture")]          = LuaValue(engoption->max_idle_mixture);
        lua_engoption[LuaValue("min_idle_mixture")]          = LuaValue(engoption->min_idle_mixture);

        lua_truckfile[LuaValue("engoption")] = lua_engoption;
    }

    if (engturbo)
    {
        LuaValue lua_engturbo = EmptyTable;
        lua_engturbo[LuaValue("version")]                 = LuaValue(engturbo->version);
        if (engturbo->version == 1)
        {
            lua_engturbo[LuaValue("inertia_factor")]      = LuaValue(engturbo->tinertiaFactor);
            lua_engturbo[LuaValue("num_turbos")]          = LuaValue(engturbo->nturbos);
            lua_engturbo[LuaValue("additional_torque")]   = LuaValue(engturbo->param1);
            lua_engturbo[LuaValue("engine_rpm_op")]       = LuaValue(engturbo->param2);
        }
        else
        {
            lua_engturbo[LuaValue("inertia_factor")]      = LuaValue(engturbo->tinertiaFactor);
            lua_engturbo[LuaValue("num_turbos")]          = LuaValue(engturbo->nturbos);
            lua_engturbo[LuaValue("max_psi")]             = LuaValue(engturbo->param1);
            lua_engturbo[LuaValue("engine_rpm_op")]       = LuaValue(engturbo->param2);
            lua_engturbo[LuaValue("has_bov")]             = LuaValue(engturbo->param3 == 1);
            lua_engturbo[LuaValue("bov_min_psi")]         = LuaValue(engturbo->param4);
            lua_engturbo[LuaValue("has_wastegate")]       = LuaValue(engturbo->param5 == 1);
            lua_engturbo[LuaValue("wastegate_max_psi")]   = LuaValue(engturbo->param6);
            lua_engturbo[LuaValue("wastegate_threshold")] = LuaValue(engturbo->param7);
            lua_engturbo[LuaValue("has_antilag")]         = LuaValue(engturbo->param8 == 1);
            lua_engturbo[LuaValue("antilag_chance")]      = LuaValue(engturbo->param9);
            lua_engturbo[LuaValue("antilag_min_rpm")]     = LuaValue(engturbo->param10);
            lua_engturbo[LuaValue("antilag_power")]       = LuaValue(engturbo->param11);
        }
        lua_truckfile[LuaValue("engturbo")] = lua_engturbo;
    }

    LuaState* const L = rig->lua_state_machine;
    (*L)["truckfile"] = lua_truckfile;

    LuaValue lua_rig = EmptyTable;
    lua_rig[LuaValue("number")] = LuaValue(vehicle_number);
    if (torque_curve != nullptr)
    {
        Diluculum::LuaUserData tc_userdata(sizeof(void*));
        TorqueCurve** tc_userdata_ptr = static_cast<TorqueCurve**>(tc_userdata.getData());
        *tc_userdata_ptr = torque_curve;
        lua_rig[LuaValue("legacy_torque_curve")] = tc_userdata;
    }
    (*L)["rig"] = lua_rig;
}

// static
void LuaSubsystem::FinishRigSetup(rig_t* rig)
{
    LuaState* const L = rig->lua_state_machine;
    std::string scripts_path = SSETTING("Simulation Scripts Path", "");
    L->doFile(scripts_path + "/init_rig.lua");
}
