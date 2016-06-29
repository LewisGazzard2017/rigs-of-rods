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
#include "Settings.h"
#include "Profiler.h"

#include "Diluculum/LuaState.hpp"

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

#ifdef POWERTRAIN_PROFILING_ENABLED
#   define ROR_PROFILE_RIG_LOADING
#   include "Profiler.h"
// Use root namespace ::
#   define POWERTRAIN_PROFILER_LABEL(NAME)          ("Powertrain | " NAME)
#   define POWERTRAIN_PROFILER_START(NAME)          ::PROFILE_START_RAW(FLEXBODY_PROFILER_LABEL(NAME))
#   define POWERTRAIN_PROFILER_ENTER(NAME)          ::PROFILE_STOP(); FLEXBODY_PROFILER_START(NAME)
#   define POWERTRAIN_PROFILER_EXIT()               ::PROFILE_STOP()
#   define POWERTRAIN_PROFILER_SCOPED()             ::PROFILE_SCOPED()
#else
#   define POWERTRAIN_PROFILER_START(NAME) 
#   define POWERTRAIN_PROFILER_LABEL(NAME) 
#   define POWERTRAIN_PROFILER_EXIT()
#   define POWERTRAIN_PROFILER_SCOPED(NAME)
#   define POWERTRAIN_PROFILER_ENTER(NAME)
#endif

using namespace Ogre;
using namespace Diluculum;

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

void BeamEngine::SetLuaState(Diluculum::LuaState* lua_state, pthread_mutex_t* lua_state_mutex)
{
    m_lua_state = lua_state;
    m_lua_state_mutex = lua_state_mutex;
    m_lua_powertrain = new LuaVariable((*m_lua_state)["powertrain"]);
}

float BeamEngine::getAcc()
{
    POWERTRAIN_PROFILER_SCOPED();
    //return m_curr_acc;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("curr_acc")].value().asNumber();
    return 0.f;
}

float BeamEngine::getClutch()
{
    POWERTRAIN_PROFILER_SCOPED();
    //return m_curr_clutch;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("curr_clutch")].value().asNumber();
}

float BeamEngine::getClutchForce()
{
    POWERTRAIN_PROFILER_SCOPED();
    //return m_conf_clutch_force;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("conf_clutch_force")].value().asNumber();
}

// for hydros acceleration
float BeamEngine::getCrankFactor()
{
    POWERTRAIN_PROFILER_SCOPED();
    // Lua function ClassicPowertrain.calc_crank_factor()
    SCOPED_LOCK();
    LuaValueList results = (*m_lua_powertrain).CallMethod("calc_crank_factor");
    return results.at(0).asNumber();
}

float BeamEngine::getRPM()
{
    POWERTRAIN_PROFILER_SCOPED();
    //return m_curr_engine_rpm;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("curr_engine_rpm")].value().asNumber();
}

float BeamEngine::getSmoke()
{
    POWERTRAIN_PROFILER_SCOPED();
    // Lua function ClassicPowertrain.calc_smoke_factor()
    SCOPED_LOCK();
    LuaValueList results = (*m_lua_powertrain).CallMethod("calc_smoke_factor");
    return results.at(0).asNumber();
}

float BeamEngine::getTorque()
{
    POWERTRAIN_PROFILER_SCOPED();
    SCOPED_LOCK()
    float torque = (*m_lua_powertrain)[LuaValue("curr_clutch_torque")].value().asNumber();
    if (torque >  1000000.0) return  1000000.0;
    if (torque < -1000000.0) return -1000000.0;
    return torque;
}

float BeamEngine::getTurboPSI()
{
    POWERTRAIN_PROFILER_SCOPED();
    // Lua function ClassicPowertrain.update_and_get_turbo_psi()
    SCOPED_LOCK();
    LuaValueList results = (*m_lua_powertrain).CallMethod("update_and_get_turbo_psi");
    return results.at(0).asNumber();
}

int BeamEngine::getAutoMode()
{
    POWERTRAIN_PROFILER_SCOPED();
    //return m_transmission_mode;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("transmission_mode")].value().asInteger();
}

void BeamEngine::setAcc(float val)
{
    POWERTRAIN_PROFILER_SCOPED();
    //m_curr_acc = val;
    SCOPED_LOCK()
    (*m_lua_powertrain)[LuaValue("curr_acc")] = LuaValue(val);
}

void BeamEngine::setAutoMode(int mode)
{
    POWERTRAIN_PROFILER_SCOPED();
    //m_transmission_mode = mode;
    SCOPED_LOCK()
    (*m_lua_powertrain)[LuaValue("transmission_mode")] = LuaValue(mode);
}

void BeamEngine::setClutch(float clutch)
{
    POWERTRAIN_PROFILER_SCOPED();
    //m_curr_clutch = clutch;
    SCOPED_LOCK()
    (*m_lua_powertrain)[LuaValue("curr_clutch")] = LuaValue(clutch);
}

void BeamEngine::setRPM(float rpm)
{
    POWERTRAIN_PROFILER_SCOPED();
    //m_curr_engine_rpm = rpm;
    SCOPED_LOCK()
    (*m_lua_powertrain)[LuaValue("curr_engine_rpm")] = LuaValue(rpm);
}

void BeamEngine::setSpin(float rpm)
{
    POWERTRAIN_PROFILER_SCOPED();
    //m_cur_wheel_revolutions = rpm;
    SCOPED_LOCK()
    (*m_lua_powertrain)[LuaValue("cur_wheel_revolutions")] = LuaValue(rpm);
}

void BeamEngine::toggleAutoMode()
{
    POWERTRAIN_PROFILER_SCOPED();
    // Lua function ClassicPowertrain.toggle_auto_transmission_mode()
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("toggle_auto_transmission_mode");
}

void BeamEngine::toggleContact()
{
    POWERTRAIN_PROFILER_SCOPED();
    // Lua function ClassicPowertrain.toggle_starter_contact()
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("toggle_starter_contact");
}

void BeamEngine::offstart()
{
    POWERTRAIN_PROFILER_SCOPED();
    // Lua function ClassicPowertrain.offstart()
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("offstart");
}

int BeamEngine::getGear()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return m_curr_gear;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("curr_gear")].value().asNumber();
}

int BeamEngine::getGearRange()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return m_curr_gear_range;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("curr_gear_range")].value().asNumber();
}

int BeamEngine::getAutoShift()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return (int)m_autoselect;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("autoselect")].value().asInteger();
}

bool BeamEngine::hasContact()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return m_starter_has_contact;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("starter_has_contact")].value().asBoolean();
}

bool BeamEngine::hasTurbo()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return m_conf_engine_has_turbo
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("conf_engine_has_turbo")].value().asBoolean();
}

bool BeamEngine::isRunning()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return m_is_engine_running;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("is_engine_running")].value().asBoolean();
}

char BeamEngine::getType()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return m_conf_engine_type;
    SCOPED_LOCK()
    std::string type_str = (*m_lua_powertrain)[LuaValue("conf_engine_type")].value().asString();
    if (type_str == "c") return 'c';
    if (type_str == "e") return 'e';
    return 't';
}

float BeamEngine::getEngineTorque()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return m_conf_engine_torque;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("conf_engine_torque")].value().asNumber();
}

float BeamEngine::getIdleRPM()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return m_conf_engine_idle_rpm;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("conf_engine_idle_rpm")].value().asNumber();
}

float BeamEngine::getMaxRPM()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return m_conf_engine_max_rpm;
    SCOPED_LOCK()

    lua_State* L = m_lua_state->getState();
    lua_getglobal(L, "powertrain");
    lua_pushstring(L, "conf_engine_max_rpm");
    lua_gettable(L, -1);
    lua_Number lua_num = lua_tonumber(L, 0);
    return static_cast<float>(lua_num);
    //return (*m_lua_powertrain)[LuaValue("conf_engine_max_rpm")].value().asNumber();
}

float BeamEngine::getMinRPM()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return m_conf_engine_min_rpm;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("conf_engine_min_rpm")].value().asNumber();
}

// Num. forward gears
int BeamEngine::getNumGears()
{
    POWERTRAIN_PROFILER_SCOPED()
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("conf_num_gears")].value().asNumber();
}

int BeamEngine::getNumGearsRanges()
{
    POWERTRAIN_PROFILER_SCOPED()
    //return getNumGears() / 6 + 1; };
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("conf_num_gear_ranges")].value().asNumber();
}

float BeamEngine::GetTurboInertiaFactor()
{
    POWERTRAIN_PROFILER_SCOPED()
    // return m_conf_turbo_inertia_factor;
    SCOPED_LOCK()
    return (*m_lua_powertrain)[LuaValue("conf_turbo_inertia_factor")].value().asNumber();
}

void BeamEngine::BeamEngineShift(int shift_change_relative)
{
    POWERTRAIN_PROFILER_SCOPED()
    // Lua function ClassicPowertrain.shift()
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("shift", LuaValue(shift_change_relative));
}

// this is mainly for smoke...
void BeamEngine::netForceSettings(float rpm, float force, float clutch, int gear, bool _running, bool _contact, char _automode)
{
    POWERTRAIN_PROFILER_SCOPED()
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

    (*m_lua_powertrain).CallMethod("network_set_state", params);
}

void BeamEngine::setstarter(int v)
{
    POWERTRAIN_PROFILER_SCOPED()
    // Lua function ClassicPowertrain.enable_starter()
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("enable_starter", LuaValue(v == 1));
}

// quick start
void BeamEngine::start()
{
    POWERTRAIN_PROFILER_SCOPED()
    // Lua function ClassicPowertrain.start()
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("start");
}

void BeamEngine::stop()
{
    POWERTRAIN_PROFILER_SCOPED()
    // Lua function ClassicPowertrain.stop()
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("stop");
}

// low level gear changing
void BeamEngine::setGear(int v)
{
    POWERTRAIN_PROFILER_SCOPED()
    //m_curr_gear = v;
    SCOPED_LOCK()
    (*m_lua_powertrain)[LuaValue("curr_gear")] = LuaValue(v);
}

void BeamEngine::setGearRange(int v)
{
    POWERTRAIN_PROFILER_SCOPED()
    //m_curr_gear_range = v;
    SCOPED_LOCK()
    (*m_lua_powertrain)[LuaValue("curr_gear_range")] = LuaValue(v);
}

// high level controls
void BeamEngine::autoSetAcc(float val)
{
    POWERTRAIN_PROFILER_SCOPED()
    // Lua function ClassicPowertrain.auto_set_acc(self, val)
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("auto_set_acc", LuaValue(val));
}

void BeamEngine::BeamEngineShiftTo(int new_gear)
{
    POWERTRAIN_PROFILER_SCOPED()
    // Lua function ClassicPowertrain.shift_to(self, new_gear)
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("shift_to", LuaValue(new_gear));
}

void BeamEngine::autoShiftSet(int mode)
{
    POWERTRAIN_PROFILER_SCOPED()
    // Lua function ClassicPowertrain.auto_shift_set(self, mode)
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("auto_shift_set", LuaValue(mode));
}

void BeamEngine::autoShiftUp()
{
    POWERTRAIN_PROFILER_SCOPED()
    // Lua function ClassicPowertrain.auto_shift_up()
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("auto_shift_up");
}

void BeamEngine::autoShiftDown()
{
    POWERTRAIN_PROFILER_SCOPED()
    // Lua function ClassicPowertrain.auto_shift_down()
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("auto_shift_down");
}

void BeamEngine::setManualClutch(float val)
{
    POWERTRAIN_PROFILER_SCOPED()
    // Lua function ClassicPowertrain.set_manual_clutch(self, val)
    SCOPED_LOCK()
    (*m_lua_powertrain).CallMethod("set_manual_clutch", LuaValue(val));
}

void BeamEngine::SetHydroPump(float value)
{
    POWERTRAIN_PROFILER_SCOPED()
    //m_engine_hydropump = value;
    SCOPED_LOCK()
    (*m_lua_powertrain)[LuaValue("engine_hydropump")] = LuaValue(value);
}

void BeamEngine::SetPrime(int prime)
{
    POWERTRAIN_PROFILER_SCOPED()
    //m_prime = prime;
    SCOPED_LOCK()
    (*m_lua_powertrain)[LuaValue("prime")] = LuaValue(prime);
}

void BeamEngine::UpdateBeamEngine(float deltatime_seconds, bool do_update)
{
    POWERTRAIN_PROFILER_SCOPED()
    
    // Lua: ClassicPowertrain.update_beam_engine(
    //          self, delta_time_sec, do_update, node0_velocity, hdir_velocity, wheel0_radius, vehicle_brake_force, vehicle_brake_ratio)
    
    SCOPED_LOCK()
    int vehicle_index =  static_cast<int>((*m_lua_powertrain)[LuaValue("vehicle_index")].value().asNumber());
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
    (*m_lua_powertrain).CallMethod("update_beam_engine", params);
    //LOG("C++ EXIT  update_beam_engine()");
}

// static
void BeamEngine::DumpPowertrainProfilerHtml()
{
    // Print stats
    std::string out_path = SSETTING("Profiler output dir", "") + "BeamEngine.html";
    ::Profiler::DumpHtml(out_path.c_str());
}

