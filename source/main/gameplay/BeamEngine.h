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

#pragma once

#include "RoRPrerequisites.h"

#include "pthread.h"

/**
* Represents a virtual engine of a vehicle (not "engine" as in "physics engine").
*/
class BeamEngine : public ZeroedMemoryAllocator
{
public:

    enum shiftmodes {AUTOMATIC, SEMIAUTO, MANUAL, MANUAL_STICK, MANUAL_RANGES};
    enum autoswitch {REAR, NEUTRAL, DRIVE, TWO, ONE, MANUALMODE};

    BeamEngine();
    ~BeamEngine();

    // Wrapper interface

    inline void SetLuaState(Diluculum::LuaState* lua_state, pthread_mutex_t* lua_state_mutex)
    {
        m_lua_state = lua_state;
        m_lua_state_mutex = lua_state_mutex;
    }

    // Lua script interface

    float getAcc();
    float getClutch();
    float getClutchForce();
    float getCrankFactor();
    float getRPM();
    float getSmoke();
    float getTorque();
    float getTurboPSI();
    int getAutoMode();
    int getGear();
    int getGearRange();
    bool hasContact();
    bool hasTurbo();
    bool isRunning();
    char getType();
    float getEngineTorque(); // Access from TorqueCurve
    float getIdleRPM();
    float getMaxRPM();
    float getMinRPM();
    int getAutoShift();
    int getNumGears();
    int getNumGearsRanges();
    float GetTurboInertiaFactor();
    inline TorqueCurve* getTorqueCurve() const { return m_legacy_torque_curve; }

    void setAcc(float val);
    void setAutoMode(int mode); // Spawner-only access
    void setClutch(float clutch);
    /// Set current engine RPM.
    void setRPM(float rpm);
    /// Set current wheel spinning speed.
    void setSpin(float rpm);
    /// low level gear changing
    void setGear(int v);
    /// low level gear changing
    void setGearRange(int v);
    void autoSetAcc(float val);
    void autoShiftDown();
    void autoShiftSet(int mode);
    void autoShiftUp();
    void setManualClutch(float val);
    void SetHydroPump(float value);
    void SetPrime(int prime);

    void toggleAutoMode();
    void toggleContact();

    /// Quick start of vehicle engine. Plays sounds.
    void offstart();

    /// Quick engine start. Plays sounds.
    void start();

    /// stall engine
    void stop();

    /// Controls vehicle starter. No side effects.
    /// @param v 1 to run starter, 0 to stop it.
    void setstarter(int v);

    /// Changes gear by a relative offset. Plays sounds.
    /// @param shift_change_relative 1 = shift up by 1, -1 = shift down by 1
    void BeamEngineShift(int shift_change_relative);

    /// Changes gear to given value. Plays sounds.
    /// @see BeamEngine::shift
    void BeamEngineShiftTo(int val);

    // Sets current engine state; Needed mainly for smoke.
    // @param rpm Current engine RPM
    // @param force Current acceleration force
    // @param clutch 
    // @param gear Current gear {-1 = reverse, 0 = neutral, 1...21 = forward}
    // @param m_is_engine_running
    // @param m_starter_has_contact
    // @param m_transmission_mode
    void netForceSettings(float rpm, float force, float clutch, int gear, bool m_is_engine_running, bool m_starter_has_contact, char m_transmission_mode);

    void ScaleEngineTorque(float scale) { } // ScaleTruck() == DEADCODE

    void UpdateBeamEngine(float deltatime_seconds, bool do_update);

private:
    Diluculum::LuaState* m_lua_state;
    TorqueCurve*         m_legacy_torque_curve;
    pthread_mutex_t*     m_lua_state_mutex;

};

