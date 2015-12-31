
-- ========================== THE CLASSIC POWERTRAIN ===========================
--
-- This script implements RoR's classic engine simulation, controlled by
-- sections "engine", "engoption" and, since 0.4.5, "engturbo".

-- ================================== LICENSE ==================================

-- This source file is part of Rigs of Rods
-- Copyright 2005-2012 Pierre-Michel Ricordel
-- Copyright 2007-2012 Thomas Fischer
-- Copyright 2013-2015 Petr Ohlidal
--
-- For more information, see http://www.rigsofrods.com/
--
-- Rigs of Rods is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License version 3, as
-- published by the Free Software Foundation.
--
-- Rigs of Rods is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with Rigs of Rods.  If not, see <http://www.gnu.org/licenses/>.

-- =============================================================================

RoR.import("util/deque.lua")

ClassicPowertrain = {} -- Namespace
ClassicPowertrain.__index = ClassicPowertrain -- Metatable

-- Constants
ClassicPowertrain.MAX_NUM_TURBOS = 4

-- Enumerations
ClassicPowertrain.ShiftMode = {
    AUTOMATIC = 0,
    SEMIAUTO = 1,
    MANUAL = 2,
    MANUAL_STICK = 3,
    MANUAL_RANGES = 4
}

ClassicPowertrain.AutoSwitch = {
    REAR = 0,
    NEUTRAL = 1,
    DRIVE = 2,
    TWO = 3,
    ONE = 4,
    MANUALMODE = 5
}

ClassicPowertrain.RAD_PER_SEC_TO_RPM = 9.5492965855137 -- Convert radian/second to RPM (60/2*PI)

local DBG_log_message = function() end


-- Creates an engine state object.
-- Equals legacy C++ class BeamEngine with it's defaults
-- Original params: float engine_min_rpm, float engine_max_rpm, float torque, std::vector<float> gears, float dratio, int m_vehicle_index
function ClassicPowertrain.new(engine_min_rpm, engine_max_rpm, eng_torque, gear_list, num_forward_gears, diff_ratio, vehicle_index)

    local self = {} -- The state object = empty table

    -- Gearbox --
    -------------

    -- estimated wheel revolutions based on current vehicle speed along the long axis
	self.ref_wheel_revolutions = 0.0
    -- measured wheel revolutions
	self.cur_wheel_revolutions = 0.0
    -- Current gear {-1 = reverse, 0 = neutral, 1...21 = forward}
    self.curr_gear = 0
	self.curr_gear_range = 0
    -- Number of forward gears
	self.conf_num_gears = num_forward_gears
    -- Number indexed gears: [0] = reverse, [1] = neutral, [2+] - forward
	self.conf_gear_ratios = gear_list

	self.conf_num_gear_ranges = (num_forward_gears / 6) + 1

	-- Vehicle --
    -------------

    -- current velocity of the vehicle
	self.abs_velocity = 0.0
    -- current velocity of the vehicle along the long axis
	self.rel_velocity = 0.0
    self.vehicle_index = vehicle_index

	-- Clutch --
    ------------

	self.conf_clutch_force = 10000.0
	self.conf_clutch_time = 0.2
	self.curr_clutch = 0
	self.curr_clutch_torque = 0

	-- Engine --
    ------------

    local eng_braking_torque = (-eng_torque / 5.0)

    self.legacy_torque_curve          = nil
    self.conf_engine_diff_ratio       = diff_ratio
    self.conf_engine_torque           = eng_torque - eng_braking_torque
    self.conf_engine_has_air          = true
    self.conf_engine_has_turbo        = false
    self.engine_hydropump             = 0.0
    self.conf_engine_idle_rpm         = math.min(engine_min_rpm, 800.0)
    self.conf_engine_inertia          = 10.0
    self.conf_engine_max_idle_mixture = 0.2
    self.conf_engine_max_rpm          = math.abs(engine_max_rpm)
    self.conf_engine_min_idle_mixture = 0.0
    self.conf_engine_min_rpm          = math.abs(engine_min_rpm)
    -- 't' = truck (default), 'c' = car, 'e' = electric car
    self.conf_engine_type             = "t"
    self.conf_engine_braking_torque   = eng_braking_torque
    self.conf_engine_stall_rpm        = 300.0
    self.conf_post_shift_time         = 0.2
    self.post_shift_clock             = 0.0
    self.shift_clock                  = 0.0
    self.conf_shift_time              = 0.5
    self.is_post_shifting             = false
    self.is_engine_running            = false
    -- Ongoing shifting: 1 = shift up by 1, -1 = shift down by 1
    self.curr_gear_change_relative    = 0
    self.air_pressure                 = 0.0
    self.auto_curr_acc                = 0.0
    self.transmission_mode            = ClassicPowertrain.ShiftMode.AUTOMATIC
    self.autoselect                   = ClassicPowertrain.AutoSwitch.DRIVE
    self.starter_has_contact          = false
    self.starter_is_running           = false
    self.curr_acc                     = 0.0
    self.prime                        = 0
    self.curr_engine_rpm              = 0.0
	self.is_shifting                  = false
	self.conf_turbo_inertia_factor    = 1

    -- Auto transmission --
    -----------------------

    self.autotrans_curr_shift_behavior    = 0.0
    self.autotrans_up_shift_delay_counter = 0
	-- m_conf_autotrans_full_rpm_range = (m_conf_engine_max_rpm - m_conf_engine_min_rpm);
    self.conf_autotrans_full_rpm_range    = self.conf_engine_max_rpm - self.conf_engine_min_rpm
	self.autotrans_rpm_buffer             = Deque.new()
	self.autotrans_acc_buffer             = Deque.new()
	self.autotrans_brake_buffer           = Deque.new()


    -- Turbo --
    -----------

    self.conf_num_turbos                 = 1
    self.conf_turbo_max_rpm              = 200000.0
    self.conf_turbo_engine_rpm_operation = 0.0
    self.conf_turbo_version              = 1
    self.conf_turbo_min_bov_psi          = 11
    self.conf_turbo_wg_max_psi           = 20
    self.conf_turbo_has_wastegate        = false
    self.conf_turbo_has_bov              = false
    self.conf_turbo_has_flutter          = false
    self.conf_turbo_wg_threshold_p       = 0
    self.conf_turbo_wg_threshold_n       = 0
    self.conf_turbo_has_antilag          = false
    self.conf_turbo_antilag_chance_rand  = 0.9975
    self.conf_turbo_antilag_min_rpm      = 3000
    self.conf_turbo_antilag_power_factor = 170

    self.conf_turbo_max_psi              = 0
    self.conf_turbo_inertia_factor       = 1
    self.conf_turbo_addi_torque          = {}
    self.turbo_curr_rpm                  = { [0] = 0 } -- C++ like indexing from 0
    self.turbo_torque                    = 0
    self.turbo_inertia                   = 0
    self.turbo_psi                       = 0
    self.turbo_cur_bov_rpm               = {}
    self.turbo_bov_torque                = 0

	-- Effects --
    -------------

    -- Sound effect only
	self.air_pressure = 0

	-- Assign methods + constants manually
	-- Metatable is not enough when invoking methods within methods invoked from C++.
	-- ## Should be fixed now

	--for key, value in pairs(ClassicPowertrain) do
	--	self[key] = value
	--end

	--self.sound_script_trigger_start  = ClassicPowertrain.sound_script_trigger_start
	--self.sound_script_trigger_stop   = ClassicPowertrain.sound_script_trigger_stop
	--self.sound_script_trigger_once   = ClassicPowertrain.sound_script_trigger_once
	--self.sound_script_trigger_toggle = ClassicPowertrain.sound_script_trigger_toggle
	--self.sound_script_modulate       = ClassicPowertrain.sound_script_modulate

	setmetatable(self, ClassicPowertrain)
    return self

end

function ClassicPowertrain.set_legacy_torque_curve(self, tc)
	self.legacy_torque_curve = tc
end

-- Helper for setting turbos
function ClassicPowertrain.set_turbo_common_options(self, turbo_version, inertia_factor, num_turbos, engine_rpm_op)

    if self.conf_engine_has_turbo == false then
        self.conf_engine_has_turbo = true
    end

    if num_turbos > ClassicPowertrain.MAX_NUM_TURBOS then
        self.conf_num_turbos = ClassicPowertrain.MAX_NUM_TURBOS
        RoR.log_message("Lua | ClassicPowertrain | Turbo: no more than " .. ClassicPowertrain.MAX_NUM_TURBOS .. " allowed")
    else
        self.conf_num_turbos = num_turbos
    end

    self.conf_turbo_version = turbo_version;
	self.conf_turbo_inertia_factor = inertia_factor

    if engine_rpm_op ~= 9999 then
        self.conf_turbo_engine_rpm_operation = engine_rpm_op
    end

end

-- Equals legacy `BeamEngine::setTurboOptions` with turbo-type 1

function ClassicPowertrain.set_turbo_version1_options(self, inertia_factor, num_turbos, additional_torque, engine_rpm_op)

    ClassicPowertrain.set_turbo_common_options(self, inertia_factor, num_turbos, engine_rpm_op)

    -- Distribute additional torque
    local i
    for i = 0, num_turbos do
        self.conf_turbo_addi_torque[i] = additional_torque / m_conf_num_turbos
    end

end

-- Equals legacy `BeamEngine::setTurboOptions` with turbo-type 2. This is the recommended turbo type.

function ClassicPowertrain.set_turbo_version2_options(self, engturbo_def)

    ClassicPowertrain.set_turbo_common_options(self, engturbo_def.inertia_factor, engturbo_def.num_turbos, engturbo_def.engine_rpm_op)


    self.conf_turbo_max_psi = engturbo_def.max_psi
    self.conf_turbo_max_rpm = engturbo_def.max_psi * 10000
    self.conf_turbo_has_bov = engturbo_def.has_bov
    self.conf_turbo_has_wastegate = engturbo_def.has_wastegate
    self.conf_turbo_has_antilag = engturbo_def.has_antilag

    if engturbo_def.bov_min_psi ~= 9999 then
        self.conf_turbo_min_bov_psi = engturbo_def.bov_min_psi
    end

    if engturbo_def.wastegate_max_psi ~= 9999 then
        self.conf_turbo_wg_max_psi = engturbo_def.wastegate_max_psi * 10000
    end

    if engturbo_def.wastegate_threshold ~= 9999 then
        self.conf_turbo_wg_threshold_n = 1 - engturbo_def.wastegate_threshold
        self.conf_turbo_wg_threshold_p = 1 + engturbo_def.wastegate_threshold
    end

    if engturbo_def.antilag_chance ~= 9999 then
        self.conf_turbo_antilag_chance_rand = engturbo_def.antilag_chance
    end

	if engturbo_def.antilag_min_rpm ~= 9999 then
        self.conf_turbo_antilag_min_rpm = engturbo_def.antilag_min_rpm
    end

    if engturbo_def.antilag_power ~= 9999 then
        self.conf_turbo_antilag_power_factor = engturbo_def.antilag_power;
    end

	local i
	local i_max = (engturbo_def.num_turbos - 1)
	DBG_log_message("Lua: turbo setup: i_max="..tostring(i_max))
	for i = 0, i_max, 1 do
		DBG_log_message("Lua: TURBO SETUP LOOP: [i]="..tostring(i))
		self.turbo_curr_rpm[i] = 0
		self.turbo_cur_bov_rpm[i] = 0
	end

end

-- Equals legacy `BeamEngine::setOptions`
-- Param S = the state object
function ClassicPowertrain.set_engine_options(self, engoption_def)

	RoR.log_message("DBG ENTER function ClassicPowertrain.set_engine_options(self, engoption_def), engoption_def.shift_time =" .. tostring(engoption_def.shift_time))

    self.conf_engine_inertia = engoption_def.inertia
	self.conf_engine_type    = engoption_def.type

	-- IMPORTANT: Swap arguments for compatibility with legacy implementation
	-- This bug has been in RoR since 2009: https://github.com/only-a-ptr/ror-legacy-svn-trunk/commit/834d3ddda1b1dfe591520534dc58feb7a7efdfe5
	-- Developer commentary: http://www.rigsofrods.com/threads/120811-Dev-LuaPowertrain-project?p=1390652#post1390652
	local def_shift_time = engoption_def.clutch_time
	local def_clutch_time = engoption_def.shift_time

	if engoption_def._clutch_force_use_default ~= true then
		self.conf_clutch_force   = engoption_def.clutch_force
	end

	if engoption_def.clutch_time then
		self.conf_clutch_time = def_clutch_time
	end

	if engoption_def.post_shift_time then
		self.conf_post_shift_time = engoption_def.post_shift_time
	end

	if engoption_def.shift_time > 0 then
		self.conf_shift_time              = def_shift_time
	end

	-- NOT A BUG, A FEATURE!
	-- Introduced with 1st implementation: https://github.com/RigsOfRods/rigs-of-rods/commit/fa1916e24365be8920ec32f4f6cf4fa450bf6bd1#diff-d47d357a254e0b1523e9a63fa4af7382R93
	if engoption_def.stall_rpm > 0 then
		self.conf_engine_idle_rpm = engoption_def.stall_rpm
	end

	if engoption_def.max_idle_mixture > 0 then
		self.conf_engine_max_idle_mixture = engoption_def.max_idle_mixture
	end

	if engoption_def.min_idle_mixture > 0 then
		self.conf_engine_min_idle_mixture = engoption_def.min_idle_mixture
	end

    if engoption_def.type == "c" then -- Car

        self.conf_engine_has_air = false
        if self.conf_clutch_force < 0.0 then
            self.conf_clutch_force = 5000.0
        end

    elseif engoption_def.type == "e" then -- Electric car

        self.conf_engine_has_air = false
        if self.conf_clutch_force < 0.0 then
            self.conf_clutch_force = 5000.0
        end

    else -- Truck (default)

        -- TODO: Restore old turbo simulation!!
        self.conf_engine_has_air = false
        if self.conf_clutch_force < 0.0 then
            self.conf_clutch_force = 10000.0
        end

    end
	RoR.log_message("DBG EXIT function ClassicPowertrain.set_engine_options(self, engoption_def), self.conf_shift_time =" .. tostring(self.conf_shift_time))

end

--void BeamEngine::UpdateBeamEngineShifts()
function ClassicPowertrain.update_shifts(self)
DBG_log_message("Lua: ENTER update_shifts()")
--{
	--if (m_autoselect == MANUALMODE) return;
	if (self.autoselect == ClassicPowertrain.AutoSwitch.MANUALMODE) then
		DBG_log_message("Lua: EXIT  update_shifts()")
		return
	end

--#ifdef USE_OPENAL
	--SoundScriptManager::getSingleton().trigOnce(m_vehicle_index, SS_TRIG_SHIFT);
	DBG_log_message("Lua: SND trigOnce SHIFT")
	self:sound_script_trigger_once(RoR.SoundTriggerSources.SHIFT)
--#endif // USE_OPENAL

	--bool engine_is_electric = (m_conf_engine_type == 'e');
	local engine_is_electric = (self.conf_engine_type == "e")

	--if (m_autoselect == REAR)
	if self.autoselect == ClassicPowertrain.AutoSwitch.REAR
	--{
	then
		--m_curr_gear = -1;
		self.curr_gear = -1
	--} else if (m_autoselect == ClassicPowertrain.AutoSwitch.NEUTRAL && !engine_is_electric)
	--{
	elseif (self.autoselect == ClassicPowertrain.AutoSwitch.NEUTRAL and not engine_is_electric) then
		--m_curr_gear =  0;
		self.curr_gear = 0
	elseif (self.autoselect == ClassicPowertrain.AutoSwitch.ONE) then
	--} else if (m_autoselect == ClassicPowertrain.AutoSwitch.ONE)
	--{
		--m_curr_gear =  1;
		self.curr_gear = 1
	--}
	--else if (!engine_is_electric) //no other gears for electric cars
	elseif not engine_is_electric then
	--{
		-- search for an appropriate gear

		--int newGear = 1;
		local new_gear = 1

		--while (newGear < m_conf_num_gears && m_cur_wheel_revolutions > 0.0f && m_cur_wheel_revolutions * m_conf_gear_ratios[newGear + 1] > m_conf_engine_max_rpm - 100.0f)
		DBG_log_message("Lua: +while loop")
		while (new_gear < self.conf_num_gears and self.cur_wheel_revolutions > 0.0 and self.cur_wheel_revolutions * m_conf_gear_ratios[new_gear + 1] > self.conf_engine_max_rpm - 100.0)
		--{
		do
			--newGear++;
			new_gear = new_gear + 1
		--}
		end
		DBG_log_message("Lua: -while loop")

		--m_curr_gear = newGear;
		self.curr_gear = new_gear

		--if (m_autoselect == TWO)
		--{
		--	m_curr_gear = std::min(m_curr_gear, 2);
		--}
		if (self.autoselect == ClassicPowertrain.AutoSwitch.TWO) then
			self.curr_gear = math.min(self.curr_gear, 2)
		end
	--}
	end
DBG_log_message("Lua: EXIT  update_shifts()")
end

--void BeamEngine::autoSetAcc(float val)
-- !!! called externally
function ClassicPowertrain.auto_set_acc(self, val)
DBG_log_message("Lua: ENTER auto_set_acc()")
--{
	--m_auto_curr_acc = val;-
	self.auto_curr_acc = val


--	if (!m_is_shifting)
--	{
--		setAcc(val);
--	}
	if not self.is_shifting then
		self.curr_acc = val
	end
DBG_log_message("Lua: EXIT  auto_set_acc()")
end


-- float BeamEngine::getTurboPSI()
-- !!! called externally
function ClassicPowertrain.update_and_get_turbo_psi(self)
	DBG_log_message("Lua: ENTER update_and_get_turbo_psi()")

    local i

    --m_turbo_psi = 0;
    self.turbo_psi = 0

	--if (m_conf_turbo_has_bov)
	--{
    if self.conf_turbo_has_bov then

		--for (int i = 0; i < m_conf_num_turbos; i++)
		DBG_log_message("Lua: update_and_get_turbo_psi +for loop")
        for i = 0, (self.conf_num_turbos - 1), 1 do

			--m_turbo_psi += m_turbo_cur_bov_rpm[i] / 10000.0f;
            self.turbo_psi = self.turbo_psi + self.turbo_cur_bov_rpm[i] / 10000.0
        end
		DBG_log_message("Lua: update_and_get_turbo_psi -for loop")
	--}
	--else
	--{
    else

		--for (int i = 0; i < m_conf_num_turbos; i++)
		local i
		DBG_log_message("Lua: update_and_get_turbo_psi +for loop")
        for  i = 0, (self.conf_num_turbos - 1), 1 do

			--m_turbo_psi += m_turbo_curr_rpm[i] / 10000.0f;
			DBG_log_message("Lua | turbo_psi:"..tostring(self.turbo_psi)..", turbo_curr_rpm:"
				..tostring(self.turbo_curr_rpm[i])..", [i]:"..tostring(i)..", num_turbos:"..tostring(self.conf_num_turbos))

            self.turbo_psi = self.turbo_psi + self.turbo_curr_rpm[i] / 10000.0
        end
		DBG_log_message("Lua: update_and_get_turbo_psi -for loop")
	--}
    end

	DBG_log_message("Lua: EXIT  update_and_get_turbo_psi()")
	return self.turbo_psi
end


--void BeamEngine::setstarter(int v)
-- !!! caled externally
function ClassicPowertrain.enable_starter(self, value)
	DBG_log_message("Lua: ENTER enable_starter()")
--{
	--m_starter_is_running = (v == 1);
	self.starter_is_running = value

	--if (v && m_curr_engine_rpm < 750.0f)
	--{
	--	setAcc(1.0f);
	--}
	if value and self.curr_engine_rpm < 750 then
		self.curr_acc = 1
	end
	DBG_log_message("Lua: EXIT  enable_starter()")
end


-- float BeamEngine::CalcEnginePower(float rpm)
-- Called from `update_beam_engine()`
function ClassicPowertrain.calc_engine_power(self, rpm)
	DBG_log_message("Lua: ENTER calc_engine_power()")


	--float atValue = 0.0f;
    -- //Additional torque (turbo integreation)
    local addi_torque_value = 0.0

	--float rpmRatio = rpm / (m_conf_engine_max_rpm * 1.25f);
    local rpm_ratio = rpm / (self.conf_engine_max_rpm * 1.25)

	--rpmRatio = std::min(rpmRatio, 1.0f);
    rpm_ratio = math.min(rpm_ratio, 1.0)


    -- @@ torquecurve @@ --
    -- Ratio [0 - 1]
	--float tqValue = 0.0f; --//This is not a value, it's more of a ratio(0-1), really got me lost..
    local torque_ratio = 0.0


    --if (m_conf_engine_torque_curve)
	--{
	--	tqValue = m_conf_engine_torque_curve->getEngineTorque(rpmRatio);
	--}
    if self.legacy_torque_curve ~= nil then
		--RoR.log_message("Lua [!!!] about to evaluate legacy torque curve")
        torque_ratio = RoR.evaluate_legacy_torque_curve(self.legacy_torque_curve, rpm_ratio)
		--RoR.log_message("Lua [+++] legacy torquecurve done OK")
    end



	--if (m_conf_engine_has_turbo)
	--{
    if self.conf_engine_has_turbo then
		--if (m_conf_turbo_version == 1)
		--{
        if self.conf_turbo_version == 1 then
			--for (int i = 0; i < m_conf_num_turbos; i++)
            local i
			DBG_log_message("Lua: +for loop")
            for i = 0, self.conf_num_turbos, 1 do

                --atValue = m_conf_turbo_addi_torque[i] * (m_turbo_curr_rpm[i] / m_conf_turbo_max_rpm);
                addi_torque_value = self.conf_turbo_addi_torque[i] * (self.turbo_curr_rpm[i] / self.conf_turbo_max_rpm)
            end
			DBG_log_message("Lua: -for loop")

		--}
		--else
		--{
        else

			--atValue = (((getTurboPSI() * 6.8) * m_conf_engine_torque) / 100); //1psi = 6% more power
            addi_torque_value = (((self:update_and_get_turbo_psi() * 6.8) * self.conf_engine_torque) / 100); --1psi = 6% more power
		--}
        end
	--}
    end

	local result = (self.conf_engine_torque * torque_ratio) + addi_torque_value

	DBG_log_message("Lua: EXIT  calc_engine_power()")

	--local dbg_txt = string.format("Powertrain.CalcEnginePower(): input RPM: %10.3f | rpm_ratio: %10.3f | torque_ratio: %10.3f | addi_torque_value:  %10.3f | RESULT: %10.3f",
    --    rpm, rpm_ratio, torque_ratio, addi_torque_value, result);
	--RoR.log_message(dbg_txt)

	--return (m_conf_engine_torque * tqValue) + atValue;
    return result
--}
end



--// for hydros acceleration
--float BeamEngine::getCrankFactor()
-- TIGHT-LOOP: called at least once
-- !!! called externally
function ClassicPowertrain.calc_crank_factor(self)
--{
	DBG_log_message("Lua: ENTER calc_crank_factor()")

    --float minWorkingRPM = m_conf_engine_idle_rpm * 1.1f; // minWorkingRPM > m_conf_engine_idle_rpm avoids commands deadlocking the engine
    local min_working_rpm = self.conf_engine_idle_rpm * 1.1 -- minWorkingRPM > m_conf_engine_idle_rpm avoids commands deadlocking the engine

	--float rpmRatio = (m_curr_engine_rpm - minWorkingRPM) / (m_conf_engine_max_rpm - minWorkingRPM);
    local rpm_ratio = (self.curr_engine_rpm - min_working_rpm) / (self.conf_engine_max_rpm - min_working_rpm)

	--rpmRatio = std::max(0.0f, rpmRatio); // Avoids a negative rpmRatio when m_curr_engine_rpm < minWorkingRPM
    rpm_ratio = math.max(0.0, rpm_ratio) -- Avoids a negative rpmRatio when self.curr_engine_rpm < min_working_rpm

	--rpmRatio = std::min(rpmRatio, 1.0f); // Avoids a rpmRatio > 1.0f when m_curr_engine_rpm > m_conf_engine_max_rpm
    rpm_ratio = math.min(rpm_ratio, 1.0) -- Avoids a rpmRatio > 1.0f when self.curr_engine_rpm > self.conf_engine_max_rpm

	--float crankfactor = 5.0f * rpmRatio;
    local crank_factor = 5.0 * rpm_ratio

	DBG_log_message("Lua: EXIT  calc_crank_factor()")

	--return crankfactor;
    return crank_factor
--}
end

-- getSmoke()
-- !!! called externally
function ClassicPowertrain.calc_smoke_factor(self)

	DBG_log_message("Lua: ENTER calc_smoke_factor()")

	--if (m_is_engine_running)
	if self.is_engine_running then
	--{
		DBG_log_message("Lua: EXIT calc_smoke_factor()")

		--return m_curr_acc * (1.0f - m_turbo_curr_rpm[0] /* doesn't matter */ / m_conf_turbo_max_rpm);// * m_conf_engine_torque / 5000.0f;
		return self.curr_acc * (1.0 - self.turbo_curr_rpm[0] / self.conf_turbo_max_rpm)
	--}
	end

	DBG_log_message("Lua: EXIT  calc_smoke_factor()")
	return -1
end



--void BeamEngine::stop()
-- !!! called externally
function ClassicPowertrain.stop(self)
--{
	DBG_log_message("Lua: ENTER stop()")

	--if (!m_is_engine_running) return;
    if not self.is_engine_running then return end

	--m_is_engine_running = false;
    self.is_engine_running = false

	-- Script Event - engine death
    -- @@ disabled
	--TRIGGER_EVENT(SE_TRUCK_ENGINE_DIED, m_vehicle_index);

    --#ifdef USE_OPENAL
	--SoundScriptManager::getSingleton().trigStop(m_vehicle_index, SS_TRIG_ENGINE);
    self:sound_script_trigger_stop(RoR.SoundTriggerSources.ENGINE)
--#endif // USE_OPENAL
--}
	DBG_log_message("Lua: EXIT  stop()")
end

--void BeamEngine::start()
-- !!! called externally
function ClassicPowertrain.start(self)

	DBG_log_message("Lua: ENTER start()")

--	if (m_transmission_mode == AUTOMATIC)
	if (self.transmission_mode == ClassicPowertrain.ShiftMode.AUTOMATIC)
	--{
	then
		--m_curr_gear = 1;
		self.curr_gear = 1

		--m_autoselect = DRIVE;
		self.autoselect = ClassicPowertrain.AutoSwitch.DRIVE
	--} else
	--{
	else
		--if (m_transmission_mode == SEMIAUTO)
		if (self.transmission_mode == ClassicPowertrain.ShiftMode.SEMIAUTO)
		--{
		then
			--m_curr_gear = 1;
			self.curr_gear = 1
		--} else
		--{
		else
			--m_curr_gear = 0;
			self.curr_gear = 0
		--}
		end
		--m_autoselect = MANUALMODE;
		self.autoselect = ClassicPowertrain.AutoSwitch.MANUALMODE;
	--}
	end


--	m_curr_clutch = 0.0f;
--	m_curr_engine_rpm = 750.0f;
--	m_curr_clutch_torque = 0.0f;

	self.curr_clutch = 0.0
	self.curr_engine_rpm = 750.0
	self.curr_clutch_torque = 0.0

	--for (int i = 0; i < m_conf_num_turbos; i++)
	DBG_log_message("Lua: +for loop")
	for i = 0, (self.conf_num_turbos - 1), 1 do
	--{
		--m_turbo_curr_rpm[i] = 0.0f;
		self.turbo_curr_rpm[i] = 0.0
		self.turbo_cur_bov_rpm[i] = 0.0
	--}
	end
	DBG_log_message("Lua: -for loop")

--	m_air_pressure = 0.0f;
--	m_is_engine_running = true;
--	m_starter_has_contact = true;

	self.air_pressure = 0.0
	self.is_engine_running = true
	self.starter_has_contact = true

--#ifdef USE_OPENAL
	--SoundScriptManager::getSingleton().trigStart(m_vehicle_index, SS_TRIG_IGNITION);
	self:sound_script_trigger_start(RoR.SoundTriggerSources.IGNITION)
	--setAcc(0.0f);
	self.curr_acc = 0

	--SoundScriptManager::getSingleton().trigStart(m_vehicle_index, SS_TRIG_ENGINE);
	self:sound_script_trigger_start(RoR.SoundTriggerSources.ENGINE)
--#endif // USE_OPENAL
	DBG_log_message("Lua: EXIT  start()")

end


--void BeamEngine::autoShiftSet(int mode)
-- !!! called externally
--{
function ClassicPowertrain.auto_shift_set(self, mode)
	DBG_log_message("Lua: ENTER auto_shift_set()")

	--m_autoselect = (autoswitch)mode;
	self.autoselect = mode

	--this->UpdateBeamEngineShifts();
	self:update_shifts()
--}
	DBG_log_message("Lua: EXIT  auto_shift_set()")

end

--void BeamEngine::autoShiftUp()
-- !!! called externally
function ClassicPowertrain.auto_shift_up()
--{
	DBG_log_message("Lua: ENTER auto_shift_up()")

	--if (m_autoselect != REAR)
	if self.autoselect ~= ClassicPowertrain.AutoSwitch.REAR
	--{
	then
		--m_autoselect = (autoswitch)(m_autoselect-1);
		self.autoselect = self.autoselect-1

		--this->UpdateBeamEngineShifts();
		self:update_shifts()
	--}
	end
	DBG_log_message("Lua: EXIT  auto_shift_up()")
--}
end

--void BeamEngine::autoShiftDown()
-- !!! called externally
function ClassicPowertrain.auto_shift_down()
--{
	DBG_log_message("Lua: ENTER auto_shift_down()")

	--if (m_autoselect != ClassicPowertrain.AutoSwitch.ONE)
	if (self.autoselect ~= ClassicPowertrain.AutoSwitch.ONE)
	--{
	then
		--m_autoselect = (autoswitch)(m_autoselect+1);
		self.autoselect = self.autoselect + 1

		--this->UpdateBeamEngineShifts();
		self:update_shifts()
	--}
	end
	DBG_log_message("Lua: EXIT  auto_shift_down()")
--}
end

--void BeamEngine::setManualClutch(float val)
-- !!! called externally
function ClassicPowertrain.set_manual_clutch(self, val)
--{
	DBG_log_message("Lua: ENTER set_manual_clutch()")

	--if (m_transmission_mode >= ClassicPowertrain.ShiftMode.MANUAL)
	if (self.transmission_mode >= ClassicPowertrain.ShiftMode.MANUAL)
	--{
	then
		--val = std::max(0.0f, val);
		val = math.max(0, val)

		--m_curr_clutch = 1.0 - val;
		self.curr_clutch = 1 - val
	--}
	end

	DBG_log_message("Lua: EXIT  set_manual_clutch()")

--}
end


--void BeamEngine::BeamEngineShiftTo(int newGear)
-- !!! called externally
function ClassicPowertrain.shift_to(self, new_gear)
--{
	DBG_log_message("Lua: ENTER shift_to()")
	--this->BeamEngineShift(newGear - m_curr_gear);
	self:shift(new_gear - self.curr_gear)
	DBG_log_message("Lua: EXIT  shift_to()")
--}
end


--void BeamEngine::BeamEngineShift(int val)
-- !!! Called externally
function ClassicPowertrain.shift(self, value)

	DBG_log_message("Lua: ENTER shift()")

	--int getNumGears() { return m_conf_gear_ratios.size() - 2; };
	local num_gears = self.conf_num_gears -- num. forward gears

	--if (!val || m_curr_gear + val < -1 || m_curr_gear + val > getNumGears()) return;
	if value == 0 or (self.curr_gear + value < -1) or (self.curr_gear + value > num_gears) then
		return
	end

	--if (m_transmission_mode < MANUAL)
	if (self.transmission_mode < ClassicPowertrain.ShiftMode.MANUAL) then

--#ifdef USE_OPENAL
		--SoundScriptManager::getSingleton().trigStart(m_vehicle_index, SS_TRIG_SHIFT);
		DBG_log_message("Lua: SND trigStart SHIFT")
		self:sound_script_trigger_start(RoR.SoundTriggerSources.SHIFT)
--#endif // USE_OPENAL

		--m_curr_gear_change_relative = val;
		self.curr_gear_change_relative = value

		--m_is_shifting = 1;
		self.is_shifting = true

		--m_shift_clock = 0.0f;
		self.shift_clock = 0.0

		--setAcc(0.0f);
		self.curr_acc = 0

	--} else
	else

		--if (m_curr_clutch > 0.25f)
		if (self.curr_clutch > 0.25) then
--#ifdef USE_OPENAL
			--SoundScriptManager::getSingleton().trigOnce(m_vehicle_index, SS_TRIG_GEARSLIDE);
			self:sound_script_trigger_once(RoR.SoundTriggerSources.GEARSLIDE)
--#endif // USE_OPENAL
		--} else
		else

--#ifdef USE_OPENAL
			--SoundScriptManager::getSingleton().trigOnce(m_vehicle_index, SS_TRIG_SHIFT);
			DBG_log_message("Lua: SND trigOnce SHIFT")
			self:sound_script_trigger_once(RoR.SoundTriggerSources.SHIFT)
--#endif // USE_OPENAL

			--m_curr_gear += val;
			self.curr_gear = self.curr_gear + value
		--}
		end
	--}
	end

	DBG_log_message("Lua: EXIT  shift()")
--}
end


-- this is mainly for smoke...
--void BeamEngine::netForceSettings(float rpm, float force, float clutch, int gear, bool _running, bool _contact, char _automode)
-- !!! called externally
function ClassicPowertrain.network_set_state(self, rpm, force, clutch, gear_index, is_running, has_contact, auto_mode)

	DBG_log_message("Lua: ENTER network_set_state()")
--{
--	m_curr_engine_rpm = rpm;
--	m_curr_acc       = force;
--	m_curr_clutch    = clutch;
--	m_curr_gear      = gear;
--	m_is_engine_running      = _running; //(fabs(rpm)>10.0);
--	m_starter_has_contact      = _contact;

	self.curr_engine_rpm     = rpm
	self.curr_acc            = force
	self.curr_clutch         = clutch
	self.curr_gear           = gear
	self.is_engine_running   = is_running
	self.starter_has_contact = has_contact

	--if (_automode != -1)
	--{
	--	m_transmission_mode = _automode;
	--}

	if auto_mode ~= -1 then
		self.transmission_mode = auto_mode
	end

	DBG_log_message("Lua: EXIT  network_set_state()")
end


-- void BeamEngine::toggleAutoMode()
-- !!! called externally

function ClassicPowertrain.toggle_auto_transmission_mode(self)

	DBG_log_message("Lua: ENTER toggle_auto_transmission_mode()")

	--m_transmission_mode = (m_transmission_mode + 1) % (MANUAL_RANGES + 1);
	self.transmission_mode = (self.transmission_mode + 1) % (ClassicPowertrain.ShiftMode.MANUAL_RANGES + 1); -- Rely on numeric constants

	-- this switches off all automatic symbols when in manual mode

	--if (m_transmission_mode != AUTOMATIC)
	if (self.transmission_mode ~= ClassicPowertrain.ShiftMode.AUTOMATIC)
	--{
	then
		--m_autoselect = MANUALMODE;
		self.autoselect = ClassicPowertrain.AutoSwitch.MANUALMODE
	--} else
	--{
	else
		--m_autoselect = NEUTRAL;
		self.autoselect = ClassicPowertrain.AutoSwitch.NEUTRAL
	--}
	end

	--if (m_transmission_mode == MANUAL_RANGES)
	if (self.transmission_mode == ClassicPowertrain.ShiftMode.MANUAL_RANGES)
	--{
	then
		--this->setGearRange(0);
		self.curr_gear_range = 0

		--this->setGear(0);
		self.curr_gear = 0
	--}
	end

	DBG_log_message("Lua: EXIT  toggle_auto_transmission_mode()")
end

--void BeamEngine::toggleContact()
-- !!! called externally
function ClassicPowertrain.toggle_starter_contact()

	DBG_log_message("Lua: ENTER toggle_starter_contact()")

	--m_starter_has_contact = !m_starter_has_contact;
	self.starter_has_contact = not self.starter_has_contact
--#ifdef USE_OPENAL
	--if (m_starter_has_contact)
	if (selfstarter_has_contact)
	--{
	then
		--SoundScriptManager::getSingleton().trigStart(m_vehicle_index, SS_TRIG_IGNITION);
		self:sound_script_trigger_start(RoR.SoundTriggerSources.IGNITION)
	--} else
	--{
	else
		--SoundScriptManager::getSingleton().trigStop(m_vehicle_index, SS_TRIG_IGNITION);
		self:sound_script_trigger_stop(RoR.SoundTriggerSources.IGNITION)
	--}
	end
--#endif // USE_OPENAL
	DBG_log_message("Lua: EXIT  toggle_starter_contact()")

end

-- PORTING NOTE: Not sure how this works...
--void BeamEngine::offstart()
-- !!! called externally
function ClassicPowertrain.offstart(self)

	DBG_log_message("Lua: ENTER offstart()")

	--m_curr_gear = 0;
	self.curr_gear = 0

	--m_curr_clutch = 0.0f;
	self.curr_clutch = 0.0

	--if (m_conf_engine_type != 'e') // e = Electric engine
	--	m_autoselect = NEUTRAL; //no Neutral in electric engines
	--else
	--	m_autoselect = ONE;

	if (self.conf_engine_type ~= "e") then -- e = Electric engine
		self.autoselect = ClassicPowertrain.AutoSwitch.NEUTRAL --no Neutral in electric engines
	else
		self.autoselect = ClassicPowertrain.AutoSwitch.ONE
	end

	--m_curr_engine_rpm = 0.0f;
	--m_is_engine_running = false;
	--m_starter_has_contact = false;

	self.curr_engine_rpm = 0.0
	self.is_engine_running = false
	self.starter_has_contact = false


--#ifdef USE_OPENAL
	--SoundScriptManager::getSingleton().trigStop(m_vehicle_index, SS_TRIG_IGNITION);

	-- DBG
	self:sound_script_trigger_stop(RoR.SoundTriggerSources.IGNITION)

	--SoundScriptManager::getSingleton().trigStop(m_vehicle_index, SS_TRIG_ENGINE);
	self:sound_script_trigger_stop(RoR.SoundTriggerSources.ENGINE)
--#endif // USE_OPENAL

	DBG_log_message("Lua: EXIT  offstart()")

end

---------------- Soundscript wrapper functions -----------------
---------------- Mainly for debugging --------------------------

function ClassicPowertrain.sound_script_trigger_start(self, source)
	local action = RoR.SoundTriggerAction.TRIGGER_START
	local veh_index = self.vehicle_index
	--RoR.log_message("Lua: ClassicPowertrain.sound_script_trigger_start()")
	--RoR.log_message("self: " .. tostring(self) .. ", vehicle_index: " .. tostring(veh_index) .. ", source: " .. tostring(source) .. ", action:" .. tostring(action))
	RoR.sound_script_trigger(veh_index, source, action)
end

function ClassicPowertrain.sound_script_trigger_stop(self, source)
	local action = RoR.SoundTriggerAction.TRIGGER_STOP
	local veh_index = self.vehicle_index
	--RoR.log_message("Lua: ClassicPowertrain.sound_script_trigger_stop()")
	--RoR.log_message("self: " .. tostring(self) .. ", vehicle_index: " .. tostring(veh_index) .. ", source: " .. tostring(source) .. ", action:" .. tostring(action))
	RoR.sound_script_trigger(veh_index, source, action)
end

function ClassicPowertrain.sound_script_trigger_once(self, source)
	local action = RoR.SoundTriggerAction.TRIGGER_ONCE
	local veh_index = self.vehicle_index
	--RoR.log_message("Lua: ClassicPowertrain.sound_script_trigger_once()")
	--RoR.log_message("self: " .. tostring(self) .. ", vehicle_index: " .. tostring(veh_index) .. ", source: " .. tostring(source) .. ", action:" .. tostring(action))
	RoR.sound_script_trigger(veh_index, source, action)
end

function ClassicPowertrain.sound_script_trigger_toggle(self, source)
	local action = RoR.SoundTriggerAction.TRIGGER_TOGGLE
	local veh_index = self.vehicle_index
	--RoR.log_message("Lua: ClassicPowertrain.sound_script_trigger_toggle()")
	--RoR.log_message("self: " .. tostring(self) .. ", vehicle_index: " .. tostring(veh_index) .. ", source: " .. tostring(source) .. ", action:" .. tostring(action))
	RoR.sound_script_trigger(veh_index, source, action)
end

function ClassicPowertrain.sound_script_modulate(self, source, value)
	local veh_index = self.vehicle_index
	--RoR.log_message("Lua: ClassicPowertrain.sound_script_modulate()")
	--RoR.log_message("self: " .. tostring(self) .. ", vehicle_index: " .. tostring(veh_index) .. ", source: " .. tostring(source) .. ", value:" .. tostring(value))
	RoR.sound_script_modulate(veh_index, source, value)
end


local DBG_log_full_state = function(self)

	-- # ------------------------------------------------
	-- # Logs entire engine state as 1 line in RoR.log
	-- #
	-- # The output is not intended for reading by human,
	-- # only for comparsion by DIFF tool
	-- #
	-- # Equivalent logging from legacy C++ code:
	-- #     https://github.com/only-a-ptr/rigs-of-rods/tree/lua-powertrain-inspection
	-- # ------------------------------------------------
	function str_bool(val)
		return val and " TRUE" or "FALSE"
	end

	txt = string.format("DBG Powertrain | ref_wheel_revolutions: %12.2f | cur_wheel_revolutions: %12.2f | curr_gear: %12.2f | curr_gear_range: %12.2f | abs_velocity: %12.2f "
		.."| rel_velocity: %12.2f | vehicle_index: %12.2f | curr_clutch: %12.2f | curr_clutch_torque: %40.2f | engine_hydropump: %12.2f | post_shift_clock: %12.2f | shift_clock: %12.2f | conf_shift_time: %12.2f | is_post_shifting: %s | is_engine_running: %s "
		.."| curr_gear_change_relative: %12.2f | air_pressure: %12.2f | auto_curr_acc: %12.2f | transmission_mode: %12.2f | autoselect: %12.2f | starter_has_contact: %s | starter_is_running: %s | curr_acc: %12.2f | prime: %12.2f | curr_engine_rpm: %40.2f "
		.."| is_shifting: %s | autotrans_curr_shift_behavior: %12.2f | autotrans_up_shift_delay_counter: %12.2f | conf_autotrans_full_rpm_range: %12.2f | turbo_curr_rpm[0]: %12.2f | turbo_torque: %12.2f | turbo_inertia: %12.2f | turbo_psi: %12.2f | turbo_bov_torque: %12.2f "
		.."| air_pressure: %12.2f | conf_turbo_has_wastegate %s | conf_turbo_has_bov %s | conf_turbo_has_flutter %s | conf_turbo_has_antilag %s | conf_engine_has_air %s | conf_engine_has_turbo %s | conf_num_turbos %12.2f | conf_turbo_max_rpm %12.2f | conf_turbo_engine_rpm_operation %12.2f "
		.."| conf_turbo_version %12.2f | conf_turbo_min_bov_psi %12.2f | conf_turbo_wg_max_psi %12.2f | conf_turbo_wg_threshold_p %12.2f | conf_turbo_wg_threshold_n %12.2f | conf_turbo_antilag_chance_rand %12.2f | conf_turbo_antilag_min_rpm %12.2f | conf_turbo_antilag_power_factor %12.2f "
		.."| conf_turbo_max_psi %12.2f | conf_turbo_inertia_factor %12.2f | conf_engine_diff_ratio %12.2f | conf_engine_torque %12.2f | conf_clutch_force %12.2f | conf_clutch_time %12.2f | conf_engine_idle_rpm %12.2f | conf_engine_inertia %12.2f | conf_engine_max_idle_mixture %12.2f "
		.."| conf_engine_max_rpm %12.2f | conf_engine_min_idle_mixture %12.2f | conf_engine_min_rpm %12.2f | conf_engine_braking_torque %12.2f | conf_engine_torque %12.2f | conf_engine_stall_rpm %12.2f | conf_post_shift_time %12.2f | engine_type: %s",

        self.ref_wheel_revolutions,
        self.cur_wheel_revolutions,
        self.curr_gear,
        self.curr_gear_range,
        self.abs_velocity,
        self.rel_velocity,
        self.vehicle_index,
        self.curr_clutch,
        self.curr_clutch_torque,
        self.engine_hydropump,
        self.post_shift_clock,
        self.shift_clock,
        self.conf_shift_time,
        str_bool(self.is_post_shifting),
        str_bool(self.is_engine_running),
        self.curr_gear_change_relative,
        self.air_pressure,
        self.auto_curr_acc,
        self.transmission_mode,
        self.autoselect,
        str_bool(self.starter_has_contact),
        str_bool(self.starter_is_running),
        self.curr_acc,
        self.prime,
        self.curr_engine_rpm,
        str_bool(self.is_shifting),
        self.autotrans_curr_shift_behavior,
        self.autotrans_up_shift_delay_counter,
        self.conf_autotrans_full_rpm_range,
        self.turbo_curr_rpm[0],
        self.turbo_torque,
        self.turbo_inertia,
        self.turbo_psi,
        self.turbo_bov_torque,
        self.air_pressure,

        str_bool(self.conf_turbo_has_wastegate),
        str_bool(self.conf_turbo_has_bov),
        str_bool(self.conf_turbo_has_flutter),
        str_bool(self.conf_turbo_has_antilag),
        str_bool(self.conf_engine_has_air),
        str_bool(self.conf_engine_has_turbo),

        self.conf_num_turbos,
        self.conf_turbo_max_rpm,
        self.conf_turbo_engine_rpm_operation,
        self.conf_turbo_version,
        self.conf_turbo_min_bov_psi,
        self.conf_turbo_wg_max_psi,
        self.conf_turbo_wg_threshold_p,
        self.conf_turbo_wg_threshold_n,
        self.conf_turbo_antilag_chance_rand,
        self.conf_turbo_antilag_min_rpm,
        self.conf_turbo_antilag_power_factor,
        self.conf_turbo_max_psi,
        self.conf_turbo_inertia_factor,
        self.conf_engine_diff_ratio,
        self.conf_engine_torque,
        self.conf_clutch_force,
        self.conf_clutch_time,
        self.conf_engine_idle_rpm,
        self.conf_engine_inertia,
        self.conf_engine_max_idle_mixture,
        self.conf_engine_max_rpm,
        self.conf_engine_min_idle_mixture,
        self.conf_engine_min_rpm,
        self.conf_engine_braking_torque,
        self.conf_engine_torque,
        self.conf_engine_stall_rpm,
        self.conf_post_shift_time,
        self.conf_engine_type
        )


	RoR.log_message(txt)

end


-- TIGHT-LOOP: Called at least once per frame.
-- This is a 1:1 port of legacy `BeamEngine::UpdateBeamEngine()`
-- C++: void BeamEngine::UpdateBeamEngine(float dt, int doUpdate)
--
-- Vehicle data:
--
--     node0_velocity ~~ [number] ~~ truck->nodes[0].Velocity.length();
--
--     hdir_velocity ~~ [number | nil] ~~ "velocity" below (NOTE: I have no idea what the calculation does ~ only_a_ptr, 12/2015)
--		if (truck->cameranodepos[0] >= 0 && truck->cameranodedir[0] >=0)
--		{
--			Vector3 hdir = (truck->nodes[truck->cameranodepos[0]].RelPosition - truck->nodes[truck->cameranodedir[0]].RelPosition).normalisedCopy();
--			velocity = hdir.dotProduct(truck->nodes[0].Velocity);
--		}
--
--     wheel0_radius ~~ [number] ~~ truck->wheels[0].radius
--
--     vehicle_brake_force ~~ [number] ~~ truck->brakeforce
--     vehicle_brake_ratio ~~ [number] ~~ truck->brake
function ClassicPowertrain.update_beam_engine(self, delta_time_sec, do_update, node0_velocity, hdir_velocity, wheel0_radius, vehicle_brake_force, vehicle_brake_ratio)
	DBG_log_message("Lua: ENTER update_beam_engine()")

	--[[ # Log arguments (DBG)
	RoR.log_message(
		string.format("do_update: %s | node0_velocity: %12.2f | hdir_velocity: %12.2f "
		.. "| wheel0_radius: %12.2f | vehicle_brake_force: %12.2f | vehicle_brake_ratio: %12.2f",
		(do_update and " TRUE" or "FALSE"), node0_velocity, hdir_velocity, wheel0_radius, vehicle_brake_force, vehicle_brake_ratio)
	)
	--]]

	DBG_log_full_state(self)

	--Beam* truck = BeamFactory::getSingleton().getTruck(this->m_vehicle_index);

	--if (!truck) return;

	--float acc = this->m_curr_acc;
    local acc = self.curr_acc
    -- bool engine_is_electric = (m_conf_engine_type == 'e');
    local is_electric = self.conf_engine_type == "e"

    local idle_mixture = 0
    -------------------------------- CalcIdleMixture() -------------------------

    --if (m_curr_engine_rpm < m_conf_engine_idle_rpm)
    if self.curr_engine_rpm < self.conf_engine_idle_rpm then
	--{
		-- determine the fuel injection needed to counter the engine braking force

        ------------------------- getAccToHoldRPM() ----------------------------

        --float BeamEngine::getAccToHoldRPM(float rpm)
        --{
        --    float rpmRatio = rpm / (m_conf_engine_max_rpm * 1.25f);
        local rpm_ratio = self.curr_engine_rpm / (self.conf_engine_max_rpm * 1.25)

        --    rpmRatio = std::min(rpmRatio, 1.0f);
        rpm_ratio = math.min(rpm_ratio, 1.0)


        --    return (-m_conf_engine_braking_torque * rpmRatio) / CalcEnginePower(m_curr_engine_rpm);
        local acc_to_hold_rpm = (-self.conf_engine_braking_torque * rpm_ratio) / self:calc_engine_power(self.curr_engine_rpm)
        --}

        ------------------------- END getAccToHoldRPM() ----------------------------
        --float idleMix = getAccToHoldRPM(m_curr_engine_rpm);
        local idle_mix = acc_to_hold_rpm

		--idleMix = std::max(0.06f, idleMix);
        idle_mix = math.max(0.06, idle_mix)

		--idleMix = idleMix * (1.0f + (m_conf_engine_idle_rpm - m_curr_engine_rpm) / 100.0f);
        idle_mix = idle_mix * (1.0 + (self.conf_engine_idle_rpm - self.curr_engine_rpm) / 100.0)

		--idleMix = std::max(m_conf_engine_min_idle_mixture, idleMix);
        idle_mix = math.max(self.conf_engine_min_idle_mixture, idle_mix)

		--idleMix = std::min(idleMix, m_conf_engine_max_idle_mixture);
        idle_mix = math.min(idle_mix, self.conf_engine_max_idle_mixture)

		--return idleMix;
        idle_mixture = idle_mix
	--}
    else

	--return 0.0f;
        idle_mixture = 0
    end

    ------------------------------ END CalcIdleMixture() ------------------------

	DBG_log_message("Lua: UPDATE ^1")

	--acc = std::max(CalcIdleMixture(), acc);
    acc = math.max(idle_mixture, acc)

	local DBG_acc_snap1 = acc
	local DBG_acc_snap1_idlemix = idle_mixture

    ----------------------------- CalcPrimeMixture() ---------------------------
    local prime_mixture = 0
    --float BeamEngine::CalcPrimeMixture()
    --{

    --if (m_prime)
    --{
    if self.prime ~= 0 then

        --float crankfactor = getCrankFactor();
        local crank_factor = self:calc_crank_factor()

        --if (crankfactor < 0.9f)
        --{
        if crank_factor < 0.9 then

            -- crankfactor is between 0.0f and 0.9f

            --return 1.0f;
            prime_mixture = 1

        --} else if (crankfactor < 1.0f)
        --{
        elseif crank_factor < 1.0 then

            -- crankfactor is between 0.9f and 1.0f

            --return 10.0f * (1.0f - crankfactor);
            prime_mixture = 10 * (1 - crank_factor)
        --}
        end
    --}

    --return 0.0f;
    --}
    end

	DBG_log_message("Lua: UPDATE ^2")
    ----------------------------- END CalcPrimeMixture() ---------------------------

	--acc = std::max(CalcPrimeMixture(), acc);
    acc = math.max(prime_mixture, acc)

	local DBG_acc_snap2 = acc
	local DBG_acc_snap2_primemix = prime_mixture


--	if (doUpdate)
--	{
    if do_update then

--#ifdef USE_OPENAL
--		SoundScriptManager::getSingleton().modulate(m_vehicle_index, SS_MOD_INJECTOR, acc);
        self:sound_script_modulate(RoR.SoundModulationSources.INJECTOR, acc)

--#endif // USE_OPENAL
--	}
    end

	--if (m_conf_engine_has_air)
    if self.conf_engine_has_air == true then
	--{
		-- air pressure

        self.air_pressure = self.air_pressure + delta_time_sec * self.curr_engine_rpm
		--m_air_pressure += dt * m_curr_engine_rpm;

        --if (m_air_pressure > 50000.0f)
		--{
        if self.air_pressure > 50000.0 then
--#ifdef USE_OPENAL

			--SoundScriptManager::getSingleton().trigOnce(m_vehicle_index, SS_TRIG_AIR_PURGE);
            self:sound_script_trigger_once(RoR.SoundTriggerSources.AIR_PURGE)

            --#endif // USE_OPENAL

			--m_air_pressure = 0.0f;
            self.air_pressure = 0.0
		--}
        end
	--}
    end

	DBG_log_message("Lua: UPDATE ^3")

	--if (m_conf_engine_has_turbo)
	--{
    if self.conf_engine_has_turbo then

        local i

		--for (int i = 0; i < m_conf_num_turbos; i++)
		--{
		DBG_log_message("Lua: +for loop (for each turbo)")
        for i = 0, self.conf_num_turbos, 1 do

			-- update turbo speed (lag)
			-- reset each of the values for each turbo

            --m_turbo_torque = 0.0f;
			--m_turbo_bov_torque = 0.0f;
			--m_turbo_inertia = 0.000003f * m_conf_turbo_inertia_factor;
            self.turbo_torque = 0.0
			self.turbo_bov_torque = 0.0
			self.turbo_inertia = 0.000003 * self.conf_turbo_inertia_factor


			-- braking (compression)

			--m_turbo_torque -= m_turbo_curr_rpm[i] / m_conf_turbo_max_rpm;
			--m_turbo_bov_torque -= m_turbo_cur_bov_rpm[i] / m_conf_turbo_max_rpm;
            self.turbo_torque = self.turbo_torque - (self.turbo_curr_rpm[i] / self.conf_turbo_max_rpm)
			self.turbo_bov_torque = self.turbo_bov_torque - (self.turbo_cur_bov_rpm[i] / self.conf_turbo_max_rpm)

			-- powering (exhaust) with limiter


			--if (m_curr_engine_rpm >= m_conf_turbo_engine_rpm_operation)
			--{
			if (self.curr_engine_rpm >= self.conf_turbo_engine_rpm_operation) then

                --if (m_turbo_curr_rpm[i] <= m_conf_turbo_max_rpm && m_is_engine_running && acc > 0.06f)
				--{
                if (self.turbo_curr_rpm[i] <= self.conf_turbo_max_rpm and self.is_engine_running and acc > 0.06) then

					--if (m_conf_turbo_has_wastegate)
					--{
                    if (self.conf_turbo_has_wastegate) then

						--if (m_turbo_curr_rpm[i] < m_conf_turbo_wg_max_psi * m_conf_turbo_wg_threshold_p && !m_conf_turbo_has_flutter)
						--{
                        if self.turbo_curr_rpm[i] < self.conf_turbo_wg_max_psi * self.conf_turbo_wg_threshold_p and (not self.conf_turbo_has_flutter) then

							--m_turbo_torque += 1.5f * acc * (((m_curr_engine_rpm - m_conf_turbo_engine_rpm_operation) / (m_conf_engine_max_rpm - m_conf_turbo_engine_rpm_operation)));
                            self.turbo_torque = self.turbo_torque + (1.5 * acc * (((self.curr_engine_rpm - self.conf_turbo_engine_rpm_operation) / (self.conf_engine_max_rpm - self.conf_turbo_engine_rpm_operation))))

                        --}
						--else
						--{
                        else

							--m_conf_turbo_has_flutter = true;
                            self.conf_turbo_has_flutter = true

							--m_turbo_torque -= (m_turbo_curr_rpm[i] / m_conf_turbo_max_rpm) *1.5;
                            self.turbo_torque = self.turbo_torque - ((self.turbo_curr_rpm[i] / self.conf_turbo_max_rpm) *1.5)

                        --}
                        end

						--if (m_conf_turbo_has_flutter)
						--{
                        if (self.conf_turbo_has_flutter) then

							--SoundScriptManager::getSingleton().trigStart(m_vehicle_index, SS_TRIG_TURBOWASTEGATE);
							self:sound_script_trigger_start(RoR.SoundTriggerSources.TURBOWASTEGATE)

                            --if (m_turbo_curr_rpm[i] < m_conf_turbo_wg_max_psi * m_conf_turbo_wg_threshold_n)
							--{
                            if (self.turbo_curr_rpm[i] < self.conf_turbo_wg_max_psi * self.conf_turbo_wg_threshold_n) then

								--m_conf_turbo_has_flutter = false;
                                self.conf_turbo_has_flutter = false

                                --SoundScriptManager::getSingleton().trigStop(m_vehicle_index, SS_TRIG_TURBOWASTEGATE);
                                self:sound_script_trigger_stop(RoR.SoundTriggerSources.TURBOWASTEGATE)
							--}
                            end

						--}
                        end

                    --}
					--else
                    else
                    	--m_turbo_torque += 1.5f * acc * (((m_curr_engine_rpm - m_conf_turbo_engine_rpm_operation) / (m_conf_engine_max_rpm - m_conf_turbo_engine_rpm_operation)));
						local extra_torque = 1.5 * acc * (((self.curr_engine_rpm - self.conf_turbo_engine_rpm_operation) / (self.conf_engine_max_rpm - self.conf_turbo_engine_rpm_operation)))
						self.turbo_torque = self.turbo_torque + extra_torque

                    end

				--}
				--else
				--{
                else

					--m_turbo_torque += 0.1f * (((m_curr_engine_rpm - m_conf_turbo_engine_rpm_operation) / (m_conf_engine_max_rpm - m_conf_turbo_engine_rpm_operation)));
					local extra_torque = 0.1 * (((self.curr_engine_rpm - self.conf_turbo_engine_rpm_operation) / (self.conf_engine_max_rpm - self.conf_turbo_engine_rpm_operation)))
					self.turbo_torque = self.turbo_torque + extra_torque
				--}
                end

				-- Update waste gate, it's like a BOV on the exhaust part of the turbo, acts as a limiter


				--if (m_conf_turbo_has_wastegate)
				--{
                if (self.conf_turbo_has_wastegate) then

                	--if (m_turbo_curr_rpm[i] > m_conf_turbo_wg_max_psi * 0.95)
					if (self.turbo_curr_rpm[i] > self.conf_turbo_wg_max_psi * 0.95) then

                        --m_turbo_inertia = m_turbo_inertia *0.7; //Kill m_conf_engine_inertia so it flutters
						self.turbo_inertia = self.turbo_inertia * 0.7 -- Kill inertia so it flutters

					--else
                    else

						--m_turbo_inertia = m_turbo_inertia *1.3; //back to normal m_conf_engine_inertia
                        self.turbo_inertia = self.turbo_inertia *1.3 -- back to normal inertia

                    end
				--}
                end
			--}
            end

			-- simulate compressor surge
			--if (!m_conf_turbo_has_bov)
			--{
            if not self.conf_turbo_has_bov then

				--if (m_turbo_curr_rpm[i] > 13 * 10000 && m_curr_acc < 0.06f)
				--{
                if (self.turbo_curr_rpm[i] > 13 * 10000 and self.curr_acc < 0.06) then

					--m_turbo_torque += (m_turbo_torque * 2.5);
                    self.turbo_torque = self.turbo_torque + (self.turbo_torque * 2.5)

                --}
                end
			--}
            end

			-- anti lag


			--if (m_conf_turbo_has_antilag && m_curr_acc < 0.5)
			--{
            if (self.conf_turbo_has_antilag and self.curr_acc < 0.5) then

                --float f = frand();
				local f = math.random();

				--if (m_curr_engine_rpm > m_conf_turbo_antilag_min_rpm && f > m_conf_turbo_antilag_chance_rand)
				--{
                if (self.curr_engine_rpm > self.conf_turbo_antilag_min_rpm and f > self.conf_turbo_antilag_chance_rand) then

					--if (m_turbo_curr_rpm[i] > m_conf_turbo_max_rpm*0.35 && m_turbo_curr_rpm[i] < m_conf_turbo_max_rpm)
					--{
                    if (self.turbo_curr_rpm[i] > self.conf_turbo_max_rpm*0.35 and self.turbo_curr_rpm[i] < self.conf_turbo_max_rpm) then

						--m_turbo_torque -= (m_turbo_torque * (f * m_conf_turbo_antilag_power_factor));
                        self.turbo_torque = self.turbo_torque - (self.turbo_torque * (f * self.conf_turbo_antilag_power_factor))

                        --SoundScriptManager::getSingleton().trigStart(m_vehicle_index, SS_TRIG_TURBOBACKFIRE);
                        self:sound_script_trigger_start(RoR.SoundTriggerSources.TURBOBACKFIRE)

					--}
                    end

				--}
				--else
                else

					--SoundScriptManager::getSingleton().trigStop(m_vehicle_index, SS_TRIG_TURBOBACKFIRE);
                    self:sound_script_trigger_stop(RoR.SoundTriggerSources.TURBOBACKFIRE)
                end
			--}
            end

			-- update main turbo rpm


			--m_turbo_curr_rpm[i] += dt * m_turbo_torque / m_turbo_inertia;
            self.turbo_curr_rpm[i] = self.turbo_curr_rpm[i] + (delta_time_sec * self.turbo_torque / self.turbo_inertia)

			--Update BOV
			--It's basicly an other turbo which is limmited to the main one's rpm, but it doesn't affect its rpm.  It only affects the power going into the engine.
			--This one is used to simulate the pressure between the engine and the compressor.
			--I should make the whole turbo code work this way. -Max98


			--if (m_conf_turbo_has_bov)
			--{
            if (self.conf_turbo_has_bov) then

				-- if (m_turbo_cur_bov_rpm[i] < m_turbo_curr_rpm[i])
                if (self.turbo_cur_bov_rpm[i] < self.turbo_curr_rpm[i]) then

                    --m_turbo_bov_torque += 1.5f * acc * (((m_curr_engine_rpm) / (m_conf_engine_max_rpm)));
					self.turbo_bov_torque = self.turbo_bov_torque + (1.5 * acc * (((self.curr_engine_rpm) / (self.conf_engine_max_rpm))))

                --else
                else

					--m_turbo_bov_torque += 0.07f * (((m_curr_engine_rpm) / (m_conf_engine_max_rpm)));
                    self.turbo_bov_torque = self.turbo_bov_torque + (0.07 * (((self.curr_engine_rpm) / (self.conf_engine_max_rpm))))

                end


				--if (m_curr_acc < 0.06 && m_turbo_curr_rpm[i] > m_conf_turbo_min_bov_psi * 10000)
				--{
                if (self.curr_acc < 0.06 and self.turbo_curr_rpm[i] > self.conf_turbo_min_bov_psi * 10000) then

					--SoundScriptManager::getSingleton().trigStart(m_vehicle_index, SS_TRIG_TURBOBOV);
                    self:sound_script_trigger_start(RoR.SoundTriggerSources.TURBOBOV)

					--m_turbo_cur_bov_rpm[i] += dt * m_turbo_bov_torque / (m_turbo_inertia * 0.1);
                    self.turbo_cur_bov_rpm[i] = self.turbo_cur_bov_rpm[i] + (delta_time_sec * self.turbo_bov_torque / (self.turbo_inertia * 0.1))
				--}
				--else
				--{
                else

					--SoundScriptManager::getSingleton().trigStop(m_vehicle_index, SS_TRIG_TURBOBOV);
                    self:sound_script_trigger(RoR.SoundTriggerSources.TURBOBOV)

					--if (m_turbo_cur_bov_rpm[i] < m_turbo_curr_rpm[i])
					--	m_turbo_cur_bov_rpm[i] += dt * m_turbo_bov_torque / (m_turbo_inertia * 0.05);
					--else
					--	m_turbo_cur_bov_rpm[i] += dt * m_turbo_bov_torque / m_turbo_inertia;

					local extra_bov_rpm = 0
                    if (self.turbo_cur_bov_rpm[i] < self.turbo_curr_rpm[i]) then
						extra_bov_rpm = delta_time_sec * self.turbo_bov_torque / (self.turbo_inertia * 0.05)
                    else
						extra_bov_rpm = delta_time_sec * self.turbo_bov_torque / self.turbo_inertia
                    end
					self.turbo_cur_bov_rpm[i] = self.turbo_cur_bov_rpm[i] + extra_bov_rpm
				--}
                end
			--}
            end
		--}
        end -- end "for"
		DBG_log_message("Lua: -for loop (for each turbo)")
	--}
    end

	DBG_log_message("Lua: UPDATE ^4")

	-- update engine speed

	--float totaltorque = 0.0f;
    local total_torque = 0.0

	-- engine braking

	--if (m_starter_has_contact)
	--{
    if (self.starter_has_contact) then

        --totaltorque += m_conf_engine_braking_torque * m_curr_engine_rpm / m_conf_engine_max_rpm;
        total_torque = total_torque + (self.conf_engine_braking_torque * self.curr_engine_rpm / self.conf_engine_max_rpm)

	--} else
	--{
    else

		--totaltorque += 10.0f * m_conf_engine_braking_torque * m_curr_engine_rpm / m_conf_engine_max_rpm;
        total_torque = total_torque + (10.0 * self.conf_engine_braking_torque * self.curr_engine_rpm / self.conf_engine_max_rpm)
	--}
    end

	local DBG_totaltorque_sample1 = total_torque

	-- braking by hydropump


	--if (m_curr_engine_rpm > 100.0f)
	--{
    if (self.curr_engine_rpm > 100.0) then

		--totaltorque -= 8.0f * m_engine_hydropump / (m_curr_engine_rpm * 0.105f * dt);
        total_torque = total_torque - (8.0 * self.engine_hydropump / (self.curr_engine_rpm * 0.105 * delta_time_sec))
	--}
    end

	local DBG_totaltorque_sample2 = total_torque

	--if (m_is_engine_running && m_starter_has_contact && m_curr_engine_rpm < (m_conf_engine_max_rpm * 1.25f))
	--{
	--	totaltorque += CalcEnginePower(m_curr_engine_rpm) * acc;
	--}

    if (self.is_engine_running and self.starter_has_contact and self.curr_engine_rpm < (self.conf_engine_max_rpm * 1.25)) then
		total_torque = total_torque + (self:calc_engine_power(self.curr_engine_rpm) * acc)
	end

	local DBG_totaltorque_sample3 = total_torque

	--if (!engine_is_electric)
	--{
    if (not engine_is_electric) then
		--if (m_is_engine_running && m_curr_engine_rpm < m_conf_engine_stall_rpm)
		--{
        if (self.is_engine_running and self.curr_engine_rpm < self.conf_engine_stall_rpm) then

        -------- @@ TODO

			--stop(); //No, electric engine has no stop

			--No, electric engine has no stop
			self:stop()
		--}
		end

		-- m_starter_is_running

		--if (m_starter_has_contact && m_starter_is_running && m_curr_engine_rpm < m_conf_engine_stall_rpm * 1.5f)
		--{
		if (self.starter_has_contact and self.starter_is_running and self.curr_engine_rpm < self.conf_engine_stall_rpm * 1.5) then

			--totaltorque += -m_conf_engine_braking_torque; //No m_starter_is_running in electric engines
			total_torque = total_torque + (-m_conf_engine_braking_torque) -- No self.starter_is_running in electric engines
		--}
		end


		-- restart


		--if (!m_is_engine_running && m_curr_engine_rpm > m_conf_engine_stall_rpm && m_starter_has_contact)
		--{
		if ( (not self.is_engine_running) and self.curr_engine_rpm > self.conf_engine_stall_rpm and self.starter_has_contact) then

			--m_is_engine_running = true;
			self.is_engine_running = true

--#ifdef USE_OPENAL
	--		SoundScriptManager::getSingleton().trigStart(m_vehicle_index, SS_TRIG_ENGINE);
			self:sound_script_trigger_start(RoR.SoundTriggerSources.ENGINE)
--#endif // USE_OPENAL
		--}
		end
	--}
	end

	local DBG_totaltorque_sample4 = total_torque

	DBG_log_message("Lua: UPDATE ^5")

	-- clutch


	--float retorque = 0.0f;
	local retorque = 0

	--if (m_curr_gear)
	--{
	--	retorque = m_curr_clutch_torque / m_conf_gear_ratios[m_curr_gear + 1];
	--}

	if (self.curr_gear ~= 0) then
		retorque = self.curr_clutch_torque / self.conf_gear_ratios[self.curr_gear + 1]
	end

	--totaltorque -= retorque;
	total_torque = total_torque - retorque


	-- integration

	--[[ # debugging
	local dbg_txt = string.format("Lua - DBG | totaltorque { 1: %10.3f | 2: %10.3f | 3: %10.3f | 4: %10.3f } |"
		.. " acc: { 1: %10.3f (idlemix: %10.3f) | 2: %10.3f (primemix: %10.3f) | 3: %10.3f } |"
		.. " retorque: %10.3f | self.curr_engine_rpm: %10.3f | self.conf_engine_inertia: %10.3f ",
		DBG_totaltorque_sample1, DBG_totaltorque_sample2, DBG_totaltorque_sample3, DBG_totaltorque_sample4,
		DBG_acc_snap1, DBG_acc_snap1_idlemix, DBG_acc_snap2, DBG_acc_snap2_primemix, acc,
		retorque, self.curr_engine_rpm, self.conf_engine_inertia)
	RoR.log_message(dbg_txt)
	-- # END debugging --]]


	--m_curr_engine_rpm += dt * totaltorque / m_conf_engine_inertia;
	self.curr_engine_rpm = self.curr_engine_rpm + ( delta_time_sec * total_torque / self.conf_engine_inertia)

	-- update clutch torque

	--if (m_curr_gear)
	--{
	if (self.curr_gear) then

		--float gearboxspinner = m_curr_engine_rpm / m_conf_gear_ratios[m_curr_gear + 1];
		local gearbox_spinner = self.curr_engine_rpm / self.conf_gear_ratios[self.curr_gear + 1]

		--m_curr_clutch_torque = (gearboxspinner - m_cur_wheel_revolutions) * m_curr_clutch * m_curr_clutch * m_conf_clutch_force;
		self.curr_clutch_torque = (gearbox_spinner - self.cur_wheel_revolutions) * self.curr_clutch * self.curr_clutch * self.conf_clutch_force

	--} else
	--{
	else

		--m_curr_clutch_torque = 0.0f;
		self.curr_clutch_torque = 0.0
	--}
	end

	DBG_log_message("Lua: UPDATE ^6")

	--m_curr_engine_rpm = std::max(0.0f, m_curr_engine_rpm);
	self.curr_engine_rpm = math.max(0.0, self.curr_engine_rpm)

--	if (m_transmission_mode < MANUAL)
	--{
	if (self.transmission_mode < ClassicPowertrain.ShiftMode.MANUAL) then

		-- auto-shift


		--if (m_is_shifting && !engine_is_electric) //No shifting in electric cars
		--{
		-- No shifting in electric cars
		if (self.is_shifting and not engine_is_electric) then

			--m_shift_clock += dt;
			self.shift_clock = self.shift_clock + delta_time_sec

			-- clutch

			--if (m_shift_clock < m_conf_clutch_time)
			--{
			if (self.shift_clock < self.conf_clutch_time) then

				--m_curr_clutch = 1.0f - (m_shift_clock / m_conf_clutch_time);
				self.curr_clutch = 1.0 - (self.shift_clock / self.conf_clutch_time)

			--} else if (m_shift_clock > (m_conf_shift_time - m_conf_clutch_time))
			--{
			elseif (self.shift_clock > (self.conf_shift_time - self.conf_clutch_time)) then

				--m_curr_clutch = 1.0f - (m_conf_shift_time - m_shift_clock) / m_conf_clutch_time;
				self.curr_clutch = 1.0 - (self.conf_shift_time - self.shift_clock) / self.conf_clutch_time

			--} else
			--{
			else

				--m_curr_clutch = 0.0f;
				self.curr_clutch = 0.0
			--}
			end

			-- shift


			--if (m_curr_gear_change_relative && m_shift_clock > m_conf_clutch_time / 2.0f)
			--{
			if (self.curr_gear_change_relative and self.shift_clock > self.conf_clutch_time / 2.0) then

--#ifdef USE_OPENAL
				--SoundScriptManager::getSingleton().trigStart(m_vehicle_index, SS_TRIG_SHIFT);
				DBG_log_message("Lua: SND trigStart SHIFT")
				self:sound_script_trigger_start(RoR.SoundTriggerSources.SHIFT)
--#endif // USE_OPENAL

				--m_curr_gear += m_curr_gear_change_relative;
				--m_curr_gear = std::max(-1, m_curr_gear);
				--m_curr_gear = std::min(m_curr_gear, m_conf_num_gears);

				local gear = self.curr_gear
				gear = gear + self.curr_gear_change_relative
				gear = math.max(-1, gear)
				gear = math.min(gear, self.conf_num_gears)
				self.curr_gear = gear

				--m_curr_gear_change_relative = 0;
				self.curr_gear_change_relative = 0
			--}
			end

			-- end of shifting
			--if (m_shift_clock > m_conf_shift_time)
			--{
			if (self.shift_clock > self.conf_shift_time) then

--#ifdef USE_OPENAL
				--SoundScriptManager::getSingleton().trigStop(m_vehicle_index, SS_TRIG_SHIFT);
				DBG_log_message("Lua: SND trigStop SHIFT")
				self:sound_script_trigger_stop(RoR.SoundTriggerSources.SHIFT)
--#endif // USE_OPENAL

				--setAcc(m_auto_curr_acc);
				self.curr_acc = self.auto_curr_acc

				--m_is_shifting = 0;
				self.is_shifting = false

				--m_curr_clutch = 1.0f;
				self.curr_clutch = 1.0

				--m_is_post_shifting = true;
				self.is_post_shifting = true

				--m_post_shift_clock = 0.0f;
				self.post_shift_clock = 0.0
			--}
			end
		--} else
		else
			--setAcc(m_auto_curr_acc);
			self.curr_acc = self.auto_curr_acc
		end



		-- auto declutch


		--if (!engine_is_electric)
		--{
		if (not engine_is_electric) then

			--if (m_is_post_shifting)
			if (self.is_post_shifting) then

				--m_post_shift_clock += dt;
				self.post_shift_clock = self.post_shift_clock + delta_time_sec

				--if (m_post_shift_clock > m_conf_post_shift_time)
				--{
				if (self.post_shift_clock > self.conf_post_shift_time) then
					self.is_post_shifting = false
				--}
				end
			--}
			end

			--if (m_is_shifting)
			--{
			if (self.is_shifting) then

				-- we are shifting, just avoid stalling in worst case


				--if (m_curr_engine_rpm < m_conf_engine_stall_rpm * 1.2f)
				--{
				--	m_curr_clutch = 0.0f;
				--}
				if (self.curr_engine_rpm < self.conf_engine_stall_rpm * 1.2) then
					self.curr_clutch = 0
				end
			--}
			--else if (m_is_post_shifting)
			--{
			elseif (self.is_post_shifting) then

				-- we are postshifting, no gear change


				--if (m_curr_engine_rpm < m_conf_engine_stall_rpm * 1.2f && acc < 0.5f)
				if (self.curr_engine_rpm < self.conf_engine_stall_rpm * 1.2 and acc < 0.5) then
					--m_curr_clutch = 0.0f;
					self.curr_clutch = 0
				--}
				--else
				--{
				else
					--m_curr_clutch = 1.0f;
					self.curr_clutch = 1
				--}
				end
			--}
			--else if (m_curr_engine_rpm < m_conf_engine_stall_rpm * 1.2f && acc < 0.5f)
			--{
			elseif (self.curr_engine_rpm < self.conf_engine_stall_rpm * 1.2 and acc < 0.5) then

				--m_curr_clutch = 0.0f;
				self.curr_clutch = 0.0
			--}
			--else if (std::abs(m_curr_gear) == 1)
			--{
			elseif (math.abs(self.curr_gear) == 1) then

				-- 1st gear : special

				--if (m_curr_engine_rpm > m_conf_engine_min_rpm)
				if (self.curr_engine_rpm > self.conf_engine_min_rpm) then

					--m_curr_clutch = (m_curr_engine_rpm - m_conf_engine_min_rpm) / (m_conf_engine_max_rpm - m_conf_engine_min_rpm);
					self.curr_clutch = (self.curr_engine_rpm - self.conf_engine_min_rpm) / (self.conf_engine_max_rpm - self.conf_engine_min_rpm)

					--m_curr_clutch = std::min(m_curr_clutch, 1.0f);
					self.curr_clutch = math.min(self.curr_clutch, 1.0)
				--}
				--else
				--{
				else
					--m_curr_clutch = 0.0f;
					self.curr_clutch = 0.0
				--}
				end
			--}
			--else
			--{
			else
				--m_curr_clutch = 1.0f;
				self.curr_clutch = 1.0
			--}
			end
		--} else
		else
			--m_curr_clutch = 1.0f;
			self.curr_clutch = 1.0
		end
	--}
	end

	DBG_log_message("Lua: UPDATE ^7")

	--if (doUpdate && !m_is_shifting && !m_is_post_shifting)
	if (do_update and not self.is_shifting and not self.is_post_shifting) then

		--float halfRPMRange     = m_conf_autotrans_full_rpm_range / 2.f; ~~~~~~~~ PORTING NOTE: Replaced by (rpm_range / 2)
		--float oneThirdRPMRange = m_conf_autotrans_full_rpm_range / 3.f; ~~~~~~~~ PORTING NOTE: Replaced by (rpm_range / 3)

		local rpm_range = self.conf_autotrans_full_rpm_range

		-- gear hack


		--this->m_abs_velocity = truck->nodes[0].Velocity.length();
		self.abs_velocity = node0_velocity

		--float velocity = m_abs_velocity;
		local velocity = self.abs_velocity


		-- PORTING NOTE: The following block is pre-computed and result "velocity" is passed as argument "hdir_velocity" -> number or nil
		--if (truck->cameranodepos[0] >= 0 && truck->cameranodedir[0] >=0)
		--{
		--	Vector3 hdir = (truck->nodes[truck->cameranodepos[0]].RelPosition - truck->nodes[truck->cameranodedir[0]].RelPosition).normalisedCopy();
		--	velocity = hdir.dotProduct(truck->nodes[0].Velocity);
		--}
		if hdir_velocity ~= nil then
			velocity = hdir_velocity
		end


		--m_rel_velocity = std::abs(velocity);
		self.rel_velocity = math.abs(velocity)


		--if (truck->wheels[0].radius != 0)
		--{
		--	m_ref_wheel_revolutions = velocity / truck->wheels[0].radius * RAD_PER_SEC_TO_RPM;
		--}

		if (wheel0_radius ~= 0) then
			self.ref_wheel_revolutions = velocity / wheel0_radius * ClassicPowertrain.RAD_PER_SEC_TO_RPM
		end


		--if (m_transmission_mode == AUTOMATIC && (m_autoselect == DRIVE || m_autoselect == TWO) && m_curr_gear > 0)
		local is_shift_auto       = self.transmission_mode == ClassicPowertrain.ShiftMode.AUTOMATIC
		local is_autoselect_drive = self.autoselect == ClassicPowertrain.AutoSwitch.DRIVE
		local is_autoselect_two   = self.autoselect == ClassicPowertrain.AutoSwitch.TWO

		if (is_shift_auto and (is_autoselect_drive or is_autoselect_two) and self.curr_gear > 0) then

			--if ((m_curr_engine_rpm > m_conf_engine_max_rpm - 100.0f && m_curr_gear > 1) || m_cur_wheel_revolutions * m_conf_gear_ratios[m_curr_gear + 1] > m_conf_engine_max_rpm - 100.0f)
			local condition1a = (self.curr_engine_rpm > self.conf_engine_max_rpm - 100.0 and self.curr_gear > 1)
			local condition1b = self.cur_wheel_revolutions * self.conf_gear_ratios[self.curr_gear + 1] > self.conf_engine_max_rpm - 100.0

			local curr_gear_rpm = self.cur_wheel_revolutions * self.conf_gear_ratios[self.curr_gear]
			local next_gear_rpm = self.cur_wheel_revolutions * self.conf_gear_ratios[self.curr_gear + 1]

			if (condition1a or condition1b) then

				--if ((m_autoselect == DRIVE && m_curr_gear < m_conf_num_gears) || (m_autoselect == TWO && m_curr_gear < std::min(2, m_conf_num_gears)) && !engine_is_electric)
				if ((is_autoselect_drive and self.curr_gear < self.conf_num_gears) or (is_autoselect_two and self.curr_gear < math.min(2, self.conf_num_gears)) and not engine_is_electric) then

					--this->BeamEngineShift(1);
					self:shift(1)
				--}
				end

			--} else if (m_curr_gear > 1 && m_ref_wheel_revolutions * m_conf_gear_ratios[m_curr_gear] < m_conf_engine_max_rpm && (m_curr_engine_rpm < m_conf_engine_min_rpm || (m_curr_engine_rpm < m_conf_engine_min_rpm + m_autotrans_curr_shift_behavior * halfRPMRange / 2.0f &&
			--    CalcEnginePower(m_cur_wheel_revolutions * m_conf_gear_ratios[m_curr_gear]) > CalcEnginePower(m_cur_wheel_revolutions * m_conf_gear_ratios[m_curr_gear + 1]))) && !engine_is_electric)

			elseif (not engine_is_electric
				and self.curr_gear > 1
				and self.ref_wheel_revolutions * self.conf_gear_ratios[self.curr_gear] < self.conf_engine_max_rpm
				and (self.curr_engine_rpm < self.conf_engine_min_rpm or (self.curr_engine_rpm < self.conf_engine_min_rpm + self.autotrans_curr_shift_behavior * (rpm_range/2) / 2.0
				and self:calc_engine_power(curr_gear_rpm) > self:calc_engine_power(next_gear_rpm))))
			then
			--{
				--this->BeamEngineShift(-1);
				self:shift(-1)
			--}
			end

			--int newGear = m_curr_gear;
			local new_gear = self.curr_gear

			--float brake = 0.0f;
			local brake = 0

			--if (truck->brakeforce > 0.0f) ~~~~~ NOTE: truck->brakeforce is passed in as argument "vehicle_brake_force"
			--{
			--	brake = truck->brake / truck->brakeforce; ~~~~~ NOTE: truck->brake is passed in as argument "vehicle_brake_ratio"
			--}

			if vehicle_brake_force > 0 then
				brake = vehicle_brake_ratio / vehicle_brake_force
			end

			--m_autotrans_rpm_buffer.push_front(m_curr_engine_rpm);
			--m_autotrans_acc_buffer.push_front(acc);
			--m_autotrans_brake_buffer.push_front(brake);

			self.autotrans_rpm_buffer:push_front(self.curr_engine_rpm)
			self.autotrans_acc_buffer:push_front(acc)
			self.autotrans_brake_buffer:push_front(brake)

			--float avgRPM50 = 0.0f;
			--float avgRPM200 = 0.0f;
			--float avgAcc50 = 0.0f;
			--float avgAcc200 = 0.0f;
			--float avgBrake50 = 0.0f;
			--float avgBrake200 = 0.0f;

			local avg_rpm_50    = 0
			local avg_rpm_200   = 0
			local avg_acc_50    = 0
			local avg_acc_200   = 0
			local avg_brake_50  = 0
			local avg_brake_200 = 0


			--for (unsigned int i=0; i < m_autotrans_acc_buffer.size(); i++)
			local i
			local buffer_start_index = self.autotrans_acc_buffer.first
			DBG_log_message("Lua: +for loop (autotrans buffer)")
			for i = buffer_start_index, self.autotrans_acc_buffer.last, 1 do

				--if (i < 50)
				if i < (buffer_start_index + 50) then

					--avgRPM50 += m_autotrans_rpm_buffer[i];
					avg_rpm_50 = avg_rpm_50 + self.autotrans_rpm_buffer[i]

					--avgAcc50 += m_autotrans_acc_buffer[i];
					avg_acc_50 = avg_acc_50 + self.autotrans_acc_buffer[i]

					--avgBrake50 += m_autotrans_brake_buffer[i];
					avg_brake_50 = avg_brake_50 + self.autotrans_brake_buffer[i]
				--}
				end

				--avgRPM200 += m_autotrans_rpm_buffer[i];
				--avgAcc200 += m_autotrans_acc_buffer[i];
				--avgBrake200 += m_autotrans_brake_buffer[i];

				avg_rpm_200   = avg_rpm_200 + self.autotrans_rpm_buffer[i]
				avg_acc_200   = avg_acc_200 + self.autotrans_acc_buffer[i]
				avg_brake_200 = avg_brake_200 + self.autotrans_brake_buffer[i]
			--}
			end
			DBG_log_message("Lua: -for loop (autotrans buffer)")

			--avgRPM50 /= std::min(m_autotrans_rpm_buffer.size(), (std::deque<float>::size_type)50);
			avg_rpm_50 = avg_rpm_50 / math.min(self.autotrans_rpm_buffer:size(), 50)

			--avgAcc50 /= std::min(m_autotrans_acc_buffer.size(), (std::deque<float>::size_type)50);
			avg_acc_50 = avg_acc_50 / math.min(self.autotrans_acc_buffer:size(), 50)

			--avgBrake50 /= std::min(m_autotrans_brake_buffer.size(), (std::deque<float>::size_type)50);
			avg_brake_50 = avg_brake_50 / math.min(self.autotrans_brake_buffer:size(), 50)

			--avgRPM200 /= m_autotrans_rpm_buffer.size();
			--avgAcc200 /= m_autotrans_acc_buffer.size();
			--avgBrake200 /= m_autotrans_brake_buffer.size();

			avg_rpm_200   = avg_rpm_200 / self.autotrans_rpm_buffer:size()
			avg_acc_200   = avg_acc_200 / self.autotrans_acc_buffer:size()
			avg_brake_200 = avg_brake_200 / self.autotrans_brake_buffer:size()

			--if (!engine_is_electric)
			if (not engine_is_electric) then

				--if (avgAcc50 > 0.8f || avgAcc200 > 0.8f || avgBrake50 > 0.8f || avgBrake200 > 0.8f)
				if (avg_acc_50 > 0.8 or avg_acc_200 > 0.8 or avg_brake_50 > 0.8 or avg_brake_200 > 0.8) then
				--{
					--m_autotrans_curr_shift_behavior = std::min(m_autotrans_curr_shift_behavior + 0.01f, 1.0f);
					self.autotrans_curr_shift_behavior = math.min(self.autotrans_curr_shift_behavior + 0.01, 1.0)
				--}
				--else if (acc < 0.5f && avgAcc50 < 0.5f && avgAcc200 < 0.5f && brake < 0.5f && avgBrake50 < 0.5f && avgBrake200 < 0.5 )
				elseif (acc < 0.5 and avg_acc_50 < 0.5 and avg_acc_200 < 0.5 and brake < 0.5 and avg_brake_50 < 0.5 and avg_brake_200 < 0.5 ) then
				--{
					self.autotrans_curr_shift_behavior = self.autotrans_curr_shift_behavior / 1.01
				--}
				end

				-- ~~~~ Separated out during porting
				--CalcEnginePower(m_cur_wheel_revolutions * m_conf_gear_ratios[newGear])   * m_conf_gear_ratios[newGear]
				local new_gear_engine_power = self:calc_engine_power(self.cur_wheel_revolutions * self.conf_gear_ratios[new_gear])   * self.conf_gear_ratios[new_gear]

				--CalcEnginePower(m_cur_wheel_revolutions * m_conf_gear_ratios[newGear+1]) * m_conf_gear_ratios[newGear+1])
				local next_gear_engine_power = (self:calc_engine_power(self.cur_wheel_revolutions * self.conf_gear_ratios[new_gear + 1]) * self.conf_gear_ratios[new_gear + 1])

				--if (avgAcc50 > 0.8f && m_curr_engine_rpm < m_conf_engine_max_rpm - oneThirdRPMRange)
				if (avg_acc_50 > 0.8 and self.curr_engine_rpm < self.conf_engine_max_rpm - (rpm_range/3)) then
				--{

					--while (newGear > 1 && m_cur_wheel_revolutions * m_conf_gear_ratios[newGear] < m_conf_engine_max_rpm - oneThirdRPMRange &&  @@ separated out @@
					while (new_gear > 1
						and self.cur_wheel_revolutions * self.conf_gear_ratios[new_gear] < self.conf_engine_max_rpm - (rpm_range/3)
						and	new_gear_engine_power > next_gear_engine_power)
					--{
					do
						--newGear--;
						new_gear = new_gear - 1
					--}
					end
				--} else if (avgAcc50 > 0.6f && acc < 0.8f && acc > avgAcc50 + 0.1f && m_curr_engine_rpm < m_conf_engine_min_rpm + halfRPMRange)
				elseif (avg_acc_50 > 0.6 and acc < 0.8 and acc > avg_acc_50 + 0.1 and self.curr_engine_rpm < self.conf_engine_min_rpm + (rpm_range/2)) then

					--if (newGear > 1 && m_cur_wheel_revolutions * m_conf_gear_ratios[newGear] < m_conf_engine_min_rpm + halfRPMRange &&  @@ separated out @@
					if (new_gear > 1 and self.cur_wheel_revolutions * self.conf_gear_ratios[new_gear] < self.conf_engine_min_rpm + (rpm_range/2)
						and new_gear_engine_power > next_gear_engine_power)
					--{
					then
						--newGear--;
						new_gear = new_gear - 1
					--}
					end
				--} else if (avgAcc50 > 0.4f && acc < 0.8f && acc > avgAcc50 + 0.1f && m_curr_engine_rpm < m_conf_engine_min_rpm + halfRPMRange)
				elseif (avg_acc_50 > 0.4 and acc < 0.8 and acc > avg_acc_50 + 0.1 and self.curr_engine_rpm < self.conf_engine_min_rpm + (rpm_range/2))
				--{
				then

					--if (newGear > 1 && m_cur_wheel_revolutions * m_conf_gear_ratios[newGear] < m_conf_engine_min_rpm + oneThirdRPMRange && @@ separated out @@
					if (new_gear > 1 and self.cur_wheel_revolutions * self.conf_gear_ratios[new_gear] < self.conf_engine_min_rpm + (rpm_range/3)
						and new_gear_engine_power > next_gear_engine_power)
					--{
					then
						--newGear--;
						new_gear = new_gear - 1
					--}
					end
				--}
				--else if (m_curr_gear < (m_autoselect == TWO ? std::min(2, m_conf_num_gears) : m_conf_num_gears) &&
				--	avgBrake200 < 0.2f && acc < std::min(avgAcc200 + 0.1f, 1.0f) && m_curr_engine_rpm > avgRPM200 - m_conf_autotrans_full_rpm_range / 20.0f)
				elseif (self.curr_gear < (self.autoselect == ClassicPowertrain.AutoSwitch.TWO and math.min(2, self.conf_num_gears) or self.conf_num_gears) -- ~~~~ PORTING NOTE: Lua's equivalent to ternary operator (A ? B : C) is (A and B or C).
					and avg_brake_200 < 0.2 and acc < math.min(avg_acc_200 + 0.1, 1.0) and self.curr_engine_rpm > avg_rpm_200 - self.conf_autotrans_full_rpm_range / 20.0)
				--{
				then

					--if (avgAcc200 < 0.6f && avgAcc200 > 0.4f && m_curr_engine_rpm > m_conf_engine_min_rpm + oneThirdRPMRange && m_curr_engine_rpm < m_conf_engine_max_rpm - oneThirdRPMRange)
					if (avg_acc_200 < 0.6 and avg_acc_200 > 0.4 and self.curr_engine_rpm > self.conf_engine_min_rpm + (rpm_range/3) and self.curr_engine_rpm < self.conf_engine_max_rpm - (rpm_range/3))

					--{
					then
						--if (m_cur_wheel_revolutions * m_conf_gear_ratios[newGear + 2] > m_conf_engine_min_rpm + oneThirdRPMRange)
						if (self.cur_wheel_revolutions * self.conf_gear_ratios[new_gear + 2] > self.conf_engine_min_rpm + (rpm_range/3))

						--{
						then
							--newGear++;
							new_gear = new_gear + 1;
						--}
						end
					--}
					--else if (avgAcc200 < 0.4f && avgAcc200 > 0.2f && m_curr_engine_rpm > m_conf_engine_min_rpm + oneThirdRPMRange)
					elseif (avg_acc_200 < 0.4 and avg_acc_200 > 0.2 and self.curr_engine_rpm > self.conf_engine_min_rpm + (rpm_range/3))

					--{
					then

						--if (m_cur_wheel_revolutions * m_conf_gear_ratios[newGear + 2] > m_conf_engine_min_rpm + oneThirdRPMRange / 2.0f)
						if (self.cur_wheel_revolutions * self.conf_gear_ratios[new_gear + 2] > self.conf_engine_min_rpm + (rpm_range / 3) / 2.0)

						--{
						then
							--newGear++;
							new_gear = new_gear + 1
						--}
						end
					--}
					--else if (avgAcc200 < 0.2f && m_curr_engine_rpm > m_conf_engine_min_rpm + oneThirdRPMRange / 2.0f && m_curr_engine_rpm < m_conf_engine_min_rpm + halfRPMRange)
					elseif (avg_acc_200 < 0.2 and self.curr_engine_rpm > self.conf_engine_min_rpm + (rpm_range/3) / 2.0 and self.curr_engine_rpm < self.conf_engine_min_rpm + (rpm_range/2))

					--{
					then

						--if (m_cur_wheel_revolutions * m_conf_gear_ratios[newGear + 2] > m_conf_engine_min_rpm + oneThirdRPMRange / 2.0f)
						if (self.cur_wheel_revolutions * self.conf_gear_ratios[new_gear + 2] > self.conf_engine_min_rpm + (rpm_range/3) / 2.0)

						--{
						then
							--newGear++;
							new_gear = new_gear + 1
						--}
						end
					--}
					end



					--if (newGear > m_curr_gear)
					if new_gear > self.curr_gear

					--{
					then

						--m_autotrans_up_shift_delay_counter++;
						self.autotrans_up_shift_delay_counter = self.autotrans_up_shift_delay_counter + 1

						--if (m_autotrans_up_shift_delay_counter <= 100 * m_autotrans_curr_shift_behavior)
						if (self.autotrans_up_shift_delay_counter <= 100 * self.autotrans_curr_shift_behavior)

						--{
						then
							--newGear = m_curr_gear;
							new_gear = self.curr_gear
						--}
						end
					--}
					--else
					else
					--{
						--m_autotrans_up_shift_delay_counter = 0;
						self.autotrans_up_shift_delay_counter = 0
					--}
					end
				--}
				end

				--            std::abs(m_cur_wheel_revolutions * (m_conf_gear_ratios[newGear + 1] - m_conf_gear_ratios[m_curr_gear + 1])) -- ~~~~ PORTING NOTE: Separated out from condition below.
				local revs = math.abs(self.cur_wheel_revolutions * (self.conf_gear_ratios[new_gear + 1] - self.conf_gear_ratios[self.curr_gear + 1]))

				--if (newGear < m_curr_gear && %%% > oneThirdRPMRange / 6.0f ||
				--	newGear > m_curr_gear && %%% > oneThirdRPMRange / 3.0f && !engine_is_electric)
				if (not engine_is_electric and new_gear < self.curr_gear and revs > (rpm_range/3) / 6.0 or new_gear > self.curr_gear and revs > (rpm_range/3) / 3.0)

				--{
				then
					--if (m_abs_velocity - m_rel_velocity < 0.5f)
					if (self.abs_velocity - self.rel_velocity < 0.5)
					then
						--this->BeamEngineShiftTo(newGear);
						self:shift(new_gear)
					end
				--}
				end
			--}
			end


			--if (m_autotrans_acc_buffer.size() > 200)
			if (self.autotrans_acc_buffer:size() > 200)
			--{
			then
				--m_autotrans_rpm_buffer.pop_back();
				--m_autotrans_acc_buffer.pop_back();
				--m_autotrans_brake_buffer.pop_back();

				self.autotrans_rpm_buffer:pop_back()
				self.autotrans_acc_buffer:pop_back()
				self.autotrans_brake_buffer:pop_back()
			--}
			end

			-- avoid over-revving


			--if (m_transmission_mode <= SEMIAUTO && m_curr_gear != 0)
			if (self.transmission_mode <= ClassicPowertrain.ShiftMode.SEMIAUTO and self.curr_gear ~= 0)
			--{
			then

				--if (std::abs(m_cur_wheel_revolutions * m_conf_gear_ratios[m_curr_gear + 1]) > m_conf_engine_max_rpm * 1.25f)
				local _coef = math.abs(self.cur_wheel_revolutions * self.conf_gear_ratios[self.curr_gear + 1])

				if (_coef > self.conf_engine_max_rpm * 1.25)
				--{
				then
					--float clutch = 0.0f + 1.0f / (1.0f + std::abs(m_cur_wheel_revolutions * m_conf_gear_ratios[m_curr_gear + 1] - m_conf_engine_max_rpm * 1.25f) / 2.0f);
					local clutch = 1.0 / (1.0 + math.abs(_coef - self.conf_engine_max_rpm * 1.25) / 2.0)

					--m_curr_clutch = std::min(clutch, m_curr_clutch);
					self.curr_clutch = math.min(clutch, self.curr_clutch)
				--}
				end

				--if (m_curr_gear * m_cur_wheel_revolutions < -10.0f)
				if (self.curr_gear * self.cur_wheel_revolutions < -10.0)
				--{
				then
					--float clutch = 0.0f + 1.0f / (1.0f + std::abs(-10.0f - m_curr_gear * m_cur_wheel_revolutions) / 2.0f);
					local clutch = 1.0 / (1.0 + math.abs(-10.0 - self.curr_gear * self.cur_wheel_revolutions) / 2.0)

					--m_curr_clutch = std::min(clutch, m_curr_clutch);
					self.curr_clutch = math.min(clutch, self.curr_clutch)
				--}
				end
			--}
			end
		--}
		end
	--}
	end

	DBG_log_message("Lua: UPDATE ^8")

	-- audio stuff

	--UpdateBeamEngineAudio(doUpdate);
	--RoR.log_message("Lua: Invoking ClassicPowertrain:update_audio() self:"..tostring(self)..", do_update:"..tostring(do_update))
	self:update_audio(do_update)
	DBG_log_message("Lua: EXIT  update_beam_engine()")

--}
end

--void BeamEngine::UpdateBeamEngineAudio(int doUpdate)
function ClassicPowertrain.update_audio(self, do_update)
	DBG_log_message("Lua: ENTER update_audio()")
--{
--#ifdef USE_OPENAL

	--if (m_conf_engine_has_turbo)
	if (self.conf_engine_has_turbo)
	--{
	then
		--for (int i = 0; i < m_conf_num_turbos; i++)
		local i
		for i = 0, (conf_num_turbos - 1), 1 do
			 --SoundScriptManager::getSingleton().modulate(m_vehicle_index, SS_MOD_TURBO, m_turbo_curr_rpm[i]);
			self:sound_script_modulate(RoR.SoundModulationSources.TURBO, self.turbo_curr_rpm[i])
		end
	--}
	end

	--if (doUpdate)
	if do_update
	--{
	then
		--SoundScriptManager::getSingleton().modulate(m_vehicle_index, SS_MOD_ENGINE, m_curr_engine_rpm);
		self:sound_script_modulate(RoR.SoundModulationSources.ENGINE, self.curr_engine_rpm)

		--SoundScriptManager::getSingleton().modulate(m_vehicle_index, SS_MOD_TORQUE, m_curr_clutch_torque);
		self:sound_script_modulate(RoR.SoundModulationSources.TORQUE, self.curr_clutch_torque)

		--SoundScriptManager::getSingleton().modulate(m_vehicle_index, SS_MOD_GEARBOX, m_cur_wheel_revolutions);
		self:sound_script_modulate(RoR.SoundModulationSources.GEARBOX, self.cur_wheel_revolutions)
	--}
	end

	-- reverse gear beep

	--if (m_curr_gear == -1 && m_is_engine_running)
	if (self.curr_gear == -1 and self.is_engine_running)
	--{
	then
		--SoundScriptManager::getSingleton().trigStart(m_vehicle_index, SS_TRIG_REVERSE_GEAR);
		self:sound_script_trigger_start(RoR.SoundTriggerSources.REVERSE_GEAR)

	--} else
	--{
	else
		--SoundScriptManager::getSingleton().trigStop(m_vehicle_index, SS_TRIG_REVERSE_GEAR);
		self:sound_script_trigger_stop(self.vehicle_index, RoR.SoundTriggerSources.REVERSE_GEAR)
	--}
	end
--#endif // USE_OPENAL
--}
	DBG_log_message("Lua: EXIT  update_audio()")

end



