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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Rigs of Rods.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BeamEngine.h"

#include "BeamFactory.h"
#include "Scripting.h"
#include "SoundScriptManager.h"
#include "TorqueCurve.h"

#include "Diluculum/LuaState.hpp"

using namespace Ogre;
using namespace Diluculum;

struct BeamEngineScopedLock
{
    BeamEngineScopedLock(pthread_mutex_t* mtx):
        m_mtx(mtx)
    {
        MUTEX_LOCK(mtx);
    }
    ~BeamEngineScopedLock()
    {
        MUTEX_UNLOCK(m_mtx);
    }
private:
    pthread_mutex_t* m_mtx;
};

#define SCOPED_LOCK() BeamEngineScopedLock beamengine_lock(m_lua_state_mutex);
/*
ENGINE UPDATE TRACE (static):
bool RoRFrameListener::frame Started(const FrameEvent& evt)
    void BeamFactory::calc Physics(float dt)
        trucks[t]->engine->Update Beam Engine(dt, 1);
        [Executed for idle vehicles with engine m_is_engine_running (rig_t::state > DESACTIVATED)]

void Beam::run()
bool Beam::frameStep(Real dt)

void Beam::thread entry()
    void Beam::calc Forces Euler Compute(int doUpdate_int, Real dt, int step, int maxsteps)
        calc Truck Engine(doUpdate, dt);
            Update Beam Engine(doUpdate)
            [Executed for player-driven vehicle, for N steps (N dynamic based on frame-deltatime)]
            [doUpdate is 1 in first iteration, then 0]
*/

BeamEngine::BeamEngine():
    m_lua_state(nullptr),
    m_legacy_torque_curve(nullptr)
{
    m_legacy_torque_curve = new TorqueCurve();
}

BeamEngine::~BeamEngine()
{
    delete m_legacy_torque_curve;
}

float BeamEngine::getAcc()
{
    //return m_curr_acc;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("curr_acc")].value().asNumber();
    return 0.f;
}

float BeamEngine::getClutch()
{
    //return m_curr_clutch;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("curr_clutch")].value().asNumber();
}

float BeamEngine::getClutchForce()
{
    //return m_conf_clutch_force;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("conf_clutch_force")].value().asNumber();
}

// for hydros acceleration
float BeamEngine::getCrankFactor()
{
    // Lua function ClassicPowertrain.calc_crank_factor()
    SCOPED_LOCK();
    LuaValueList results = (*m_lua_state)["powertrain"].CallMethod("calc_crank_factor");
    return results.at(0).asNumber();
}

float BeamEngine::getRPM()
{
    //return m_curr_engine_rpm;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("curr_engine_rpm")].value().asNumber();
}

float BeamEngine::getSmoke()
{
    // Lua function ClassicPowertrain.calc_smoke_factor()
    SCOPED_LOCK();
    LuaValueList results = (*m_lua_state)["powertrain"].CallMethod("calc_smoke_factor");
    return results.at(0).asNumber();
}

float BeamEngine::getTorque()
{
    SCOPED_LOCK()
    float torque = (*m_lua_state)["powertrain"][LuaValue("curr_clutch_torque")].value().asNumber();
    if (torque >  1000000.0) return  1000000.0;
    if (torque < -1000000.0) return -1000000.0;
    return torque;
}

float BeamEngine::getTurboPSI()
{
    // Lua function ClassicPowertrain.update_and_get_turbo_psi()
    SCOPED_LOCK();
    LuaValueList results = (*m_lua_state)["powertrain"].CallMethod("update_and_get_turbo_psi");
    return results.at(0).asNumber();
}

int BeamEngine::getAutoMode()
{
    //return m_transmission_mode;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("curr_engine_rpm")].value().asInteger();
}

void BeamEngine::setAcc(float val)
{
    //m_curr_acc = val;
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"][LuaValue("curr_acc")] = LuaValue(val);
}

void BeamEngine::setAutoMode(int mode)
{
    //m_transmission_mode = mode;
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"][LuaValue("transmission_mode")] = LuaValue(mode);
}

void BeamEngine::setClutch(float clutch)
{
    //m_curr_clutch = clutch;
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"][LuaValue("curr_clutch")] = LuaValue(clutch);
}

void BeamEngine::setRPM(float rpm)
{
    //m_curr_engine_rpm = rpm;
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"][LuaValue("curr_engine_rpm")] = LuaValue(rpm);
}

void BeamEngine::setSpin(float rpm)
{
    //m_cur_wheel_revolutions = rpm;
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"][LuaValue("cur_wheel_revolutions")] = LuaValue(rpm);
}

void BeamEngine::toggleAutoMode()
{
    // Lua function ClassicPowertrain.toggle_auto_transmission_mode()
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("toggle_auto_transmission_mode");
}

void BeamEngine::toggleContact()
{
    // Lua function ClassicPowertrain.toggle_starter_contact()
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("toggle_starter_contact");
}

void BeamEngine::offstart()
{
    // Lua function ClassicPowertrain.offstart()
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("offstart");
}

int BeamEngine::getGear()
{
    //return m_curr_gear;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("curr_gear")].value().asNumber();
}

int BeamEngine::getGearRange()
{
    //return m_curr_gear_range;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("curr_gear_range")].value().asNumber();
}

int BeamEngine::getAutoShift()
{
    //return (int)m_autoselect;
    SCOPED_LOCK()
    return static_cast<int>((*m_lua_state)["powertrain"][LuaValue("autoselect")].value().asNumber());
}

bool BeamEngine::hasContact()
{
    //return m_starter_has_contact;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("starter_has_contact")].value().asBoolean();
}

bool BeamEngine::hasTurbo()
{
    //return m_conf_engine_has_turbo
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("conf_engine_has_turbo")].value().asBoolean();
}

bool BeamEngine::isRunning()
{
    //return m_is_engine_running;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("is_engine_running")].value().asBoolean();
}

char BeamEngine::getType()
{
    //return m_conf_engine_type;
    SCOPED_LOCK()
    std::string type_str = (*m_lua_state)["powertrain"][LuaValue("conf_engine_type")].value().asString();
    if (type_str == "c") return 'c';
    if (type_str == "e") return 'e';
    return 't';
}

float BeamEngine::getEngineTorque()
{
    //return m_conf_engine_torque;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("conf_engine_torque")].value().asNumber();
}

float BeamEngine::getIdleRPM()
{
    //return m_conf_engine_idle_rpm;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("conf_engine_idle_rpm")].value().asNumber();
}

float BeamEngine::getMaxRPM()
{
    //return m_conf_engine_max_rpm;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("conf_engine_max_rpm")].value().asNumber();
}

float BeamEngine::getMinRPM()
{
    //return m_conf_engine_min_rpm;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("conf_engine_min_rpm")].value().asNumber();
}

// Num. forward gears
int BeamEngine::getNumGears()
{
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("conf_num_gears")].value().asNumber();
}

int BeamEngine::getNumGearsRanges()
{
    //return getNumGears() / 6 + 1; };
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("conf_num_gear_ranges")].value().asNumber();
}

float BeamEngine::GetTurboInertiaFactor()
{
    // return m_conf_turbo_inertia_factor;
    SCOPED_LOCK()
    return (*m_lua_state)["powertrain"][LuaValue("conf_turbo_inertia_factor")].value().asNumber();
}

void BeamEngine::BeamEngineShift(int shift_change_relative)
{
    // Lua function ClassicPowertrain.shift()
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("shift", LuaValue(shift_change_relative));
}

// this is mainly for smoke...
void BeamEngine::netForceSettings(float rpm, float force, float clutch, int gear, bool _running, bool _contact, char _automode)
{
    // Lua: ClassicPowertrain.network_set_state(self, rpm, force, clutch, gear_index, is_running, has_contact, auto_mode)

    SCOPED_LOCK()
    LuaValueList params;
    params.push_back(LuaValue(rpm));       // rpm
    params.push_back(LuaValue(force));     // force
    params.push_back(LuaValue(clutch));    // clutch
    params.push_back(LuaValue(gear));      // gear_index
    params.push_back(LuaValue(_running));  // is_running
    params.push_back(LuaValue(_contact));  // has_contact
    params.push_back(LuaValue(_automode)); // auto_mode

    (*m_lua_state)["powertrain"].CallMethod("network_set_state", params);
}

void BeamEngine::setstarter(int v)
{
    // Lua function ClassicPowertrain.enable_starter()
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("enable_starter", LuaValue(v == 1));
}

// quick start
void BeamEngine::start()
{
    // Lua function ClassicPowertrain.start()
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("start");
}

void BeamEngine::stop()
{
    // Lua function ClassicPowertrain.stop()
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("stop");
}

// low level gear changing
void BeamEngine::setGear(int v)
{
    //m_curr_gear = v;
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"][LuaValue("curr_gear")] = LuaValue(v);
}

void BeamEngine::setGearRange(int v)
{
    //m_curr_gear_range = v;
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"][LuaValue("curr_gear_range")] = LuaValue(v);
}

// high level controls
void BeamEngine::autoSetAcc(float val)
{
    // Lua function ClassicPowertrain.auto_set_acc(self, val)
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("auto_set_acc", LuaValue(val));
}

void BeamEngine::BeamEngineShiftTo(int new_gear)
{
    // Lua function ClassicPowertrain.shift_to(self, new_gear)
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("shift_to", LuaValue(new_gear));
}

void BeamEngine::autoShiftSet(int mode)
{
    // Lua function ClassicPowertrain.auto_shift_set(self, mode)
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("auto_shift_set", LuaValue(mode));
}

void BeamEngine::autoShiftUp()
{
    // Lua function ClassicPowertrain.auto_shift_up()
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("auto_shift_up");
}

void BeamEngine::autoShiftDown()
{
    // Lua function ClassicPowertrain.auto_shift_down()
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("auto_shift_down");
}

void BeamEngine::setManualClutch(float val)
{
    // Lua function ClassicPowertrain.set_manual_clutch(self, val)
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"].CallMethod("set_manual_clutch", LuaValue(val));
}

void BeamEngine::SetHydroPump(float value)
{
    //m_engine_hydropump = value;
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"][LuaValue("engine_hydropump")] = LuaValue(value);
}

void BeamEngine::SetPrime(int prime)
{
    //m_prime = prime;
    SCOPED_LOCK()
    (*m_lua_state)["powertrain"][LuaValue("prime")] = LuaValue(prime);
}

void BeamEngine::UpdateBeamEngine(float deltatime_seconds, bool do_update)
{
    
    // Lua: ClassicPowertrain.update_beam_engine(
    //          self, delta_time_sec, do_update, node0_velocity, hdir_velocity, wheel0_radius, vehicle_brake_force, vehicle_brake_ratio)
    
    SCOPED_LOCK()
    int vehicle_index =  static_cast<int>((*m_lua_state)["powertrain"][LuaValue("vehicle_index")].value().asNumber());
    Beam* vehicle = BeamFactory::getSingleton().getTruck(vehicle_index);
    Vector3 node0_velocity    = vehicle->nodes[0].Velocity;
    float node0_velocity_len  = node0_velocity.length();
    float wheel0_radius       = vehicle->wheels[0].radius;
    float vehicle_brake_force = vehicle->brakeforce;
    float vehicle_brake_ratio = vehicle->brake;
    LuaValue hdir_velocity; // defaults to Lua type "nil" (= NULL)
    if (vehicle->cameranodepos[0] >= 0 && vehicle->cameranodedir[0] >=0)
    {
        Vector3 hdir = (vehicle->nodes[vehicle->cameranodepos[0]].RelPosition - vehicle->nodes[vehicle->cameranodedir[0]].RelPosition).normalisedCopy();
        hdir_velocity = hdir.dotProduct(node0_velocity); // Lua type "number"
    }

    LuaValueList params;
    params.push_back(LuaValue(deltatime_seconds));
    params.push_back(LuaValue(do_update));
    params.push_back(LuaValue(node0_velocity_len));
    params.push_back(hdir_velocity);
    params.push_back(LuaValue(wheel0_radius));
    params.push_back(LuaValue(vehicle_brake_force));
    params.push_back(LuaValue(vehicle_brake_ratio));

    //LOG("C++ ENTER update_beam_engine()");
    (*m_lua_state)["powertrain"].CallMethod("update_beam_engine", params);
    //LOG("C++ EXIT  update_beam_engine()");
}

