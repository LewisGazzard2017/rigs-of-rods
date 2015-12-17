
-- init_rig.lua
-- This script is executed upon spawning a rig in simulation.
-- It's responsible for setting up all Lua-based systems, i.e. powertrain

-- ============================================================================

-- Module 'RoR' is created by the application and contains
-- instruments necessary to communicate with the application
RoR.log_message("Hello RoR! This is Lua speaking!")

-- WORKAROUND: standard require() function refuses to work under windows
RoR.import = function(file) dofile(RoR.scripts_path .. file) end

RoR.import("sound/soundscripts.lua")

-- Global variable "truckfile" is created by the application
-- Contains classic powertrain config (engine, engoption...)

if truckfile == nil then
	RoR.log_message("ERROR: Global variable 'truckfile' not set.")
	return -- Terminate this script
end

-- Global variable "rig" is also from application

if rig == nil then
	RoR.log_message("ERROR: Global variable 'rig' not set.")
	return
end

if truckfile.engine == nil then
	RoR.log_message("INFO: No engine defined.")
	return
end

-- For now, the only powertrain implementation
-- is emulation of the classic behavior
RoR.import("powertrain/classic_powertrain.lua")

local num_forward_gears = (#truckfile.engine.forward_gears + 1)

local gears_compat = {
	[0] = truckfile.engine.reverse_gear_ratio,
	[1] = truckfile.engine.neutral_gear_ratio
}

for i = 0, (num_forward_gears - 1), 1 do
	gears_compat[i+2] = truckfile.engine.forward_gears[i]
end

RoR.log_message("Lua: rig number: ".. tostring(rig.number))
RoR.log_message("Lua: legacy torque curve: ".. tostring(rig.legacy_torque_curve))

local classic = ClassicPowertrain.new(
	truckfile.engine.shift_down_rpm,
	truckfile.engine.shift_up_rpm,
	truckfile.engine.torque,
	gears_compat,
	num_forward_gears,
	truckfile.engine.global_gear_ratio,
	rig.number
	)

if truckfile.engoption ~= nil then
	classic.set_engine_options(classic, truckfile.engoption)
end

if truckfile.engturbo ~= nil then
	if truckfile.engturbo.type == 1 then
		classic:set_turbo_version1_options(
			truckfile.engturbo.inertia_factor,
			truckfile.engturbo.num_turbos,
			truckfile.engturbo.additional_torque,
			truckfile.engturbo.engine_rpm_op
		)
	else
		classic:set_turbo_version2_options(truckfile.engturbo)
	end
end

if rig.legacy_torque_curve ~= nil then
	classic:set_legacy_torque_curve(rig.legacy_torque_curve)
end

-- Create global variable "powertrain"
-- IMPORTANT!!!
-- This object will be accessed externally by the application
powertrain = classic


