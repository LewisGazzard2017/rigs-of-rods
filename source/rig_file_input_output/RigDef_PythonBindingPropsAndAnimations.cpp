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
	@date   10/2015
	@author Petr Ohlidal
	Separated PythonBinding::Export*() method to aid compilation.
	Technique recommended in boost docs: http://www.boost.org/doc/libs/1_59_0/libs/python/doc/tutorial/doc/html/python/techniques.html#python.reducing_compiling_time
*/

#include "RigDef_PythonBinding.h"

#include <boost/python/detail/wrap_python.hpp>
	// Replaces #include <Python.h>, recommended in Boost docs
	// Must be included before any OS heders - Python limitation
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/register_ptr_to_python.hpp>

#include "RigDef_Node.h"
#include "RigDef_File.h"

using namespace boost::python;
using namespace RigDef;

void PythonBinding::ExportPropsAndAnimations()
{

    enum_<RigDef::Prop::Special>("SpecialPropType")
        .value("LEFT_REAR_VIEW_MIRROR",       Prop::SPECIAL_LEFT_REAR_VIEW_MIRROR )
        .value("RIGHT_REAR_VIEW_MIRROR",      Prop::SPECIAL_RIGHT_REAR_VIEW_MIRROR )
        .value("STEERING_WHEEL_LEFT_HANDED",  Prop::SPECIAL_STEERING_WHEEL_LEFT_HANDED )
        .value("STEERING_WHEEL_RIGHT_HANDED", Prop::SPECIAL_STEERING_WHEEL_RIGHT_HANDED )
        .value("SPINPROP",                    Prop::SPECIAL_SPINPROP )
        .value("PALE",                        Prop::SPECIAL_PALE )
        .value("DRIVER_SEAT",                 Prop::SPECIAL_DRIVER_SEAT )
        .value("DRIVER_SEAT_2",               Prop::SPECIAL_DRIVER_SEAT_2 )
        .value("BEACON",                      Prop::SPECIAL_BEACON )
        .value("REDBEACON",                   Prop::SPECIAL_REDBEACON )
        .value("LIGHTBAR",                    Prop::SPECIAL_LIGHTBAR )
        .value("INVALID",                     Prop::SPECIAL_INVALID )
        ;

    class_<RigDef::Prop::SteeringWheelSpecial>("SteeringWheelSpecialProp")
        .def_readwrite("offset",         &Prop::SteeringWheelSpecial::offset)
        .def_readwrite("_offset_is_set", &Prop::SteeringWheelSpecial::_offset_is_set)
        .def_readwrite("rotation_angle", &Prop::SteeringWheelSpecial::rotation_angle)
        .def_readwrite("mesh_name",      &Prop::SteeringWheelSpecial::mesh_name)
        ;

    class_<RigDef::Prop::BeaconSpecial>("BeaconSpecialProp")
        .def_readwrite("flare_material_name", &Prop::BeaconSpecial::flare_material_name)
        .def_readwrite("color",               &Prop::BeaconSpecial::color)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Prop, "PropVector")
    class_<RigDef::Prop>("Prop")
        .def_readwrite("reference_node",              &Prop::reference_node)
        .def_readwrite("x_axis_node",                 &Prop::x_axis_node)
        .def_readwrite("y_axis_node",                 &Prop::y_axis_node)
        .def_readwrite("offset",                      &Prop::offset)
        .def_readwrite("rotation",                    &Prop::rotation)
        .def_readwrite("mesh_name",                   &Prop::mesh_name)
        .def_readwrite("animations",                  &Prop::animations)
        .def_readwrite("camera_settings",             &Prop::camera_settings)
        .def_readwrite("special",                     &Prop::special)
        .def_readwrite("special_prop_beacon",         &Prop::special_prop_beacon)
        .def_readwrite("special_prop_steering_wheel", &Prop::special_prop_steering_wheel)
        ;

    enum_<RigDef::Animation::MotorSource::WhichSource>("AnimationSourceMotorDetail")
        .value("AERO_INVALID",  Animation::MotorSource::SOURCE_AERO_INVALID)
        .value("AERO_THROTTLE", Animation::MotorSource::SOURCE_AERO_THROTTLE)
        .value("AERO_RPM",      Animation::MotorSource::SOURCE_AERO_RPM     )
        .value("AERO_TORQUE",   Animation::MotorSource::SOURCE_AERO_TORQUE  )
        .value("AERO_PITCH",    Animation::MotorSource::SOURCE_AERO_PITCH   )
        .value("AERO_STATUS",   Animation::MotorSource::SOURCE_AERO_STATUS  )
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Animation::MotorSource, "MotorSourceVector")
    class_<RigDef::Animation::MotorSource>("AnimationMotorSource")
        .def_readwrite("source", &Animation::MotorSource::source)
        .def_readwrite("motor",  &Animation::MotorSource::motor)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Animation, "AnimationVector")
    class_<RigDef::Animation>("Animation")
        .add_property("use_source_air_speed",            &Animation::HasSource_AirSpeed          , &Animation::SetHasSource_AirSpeed)
        .add_property("use_source_vertical_velocity",    &Animation::HasSource_VerticalVelocity  , &Animation::SetHasSource_VerticalVelocity)
        .add_property("use_source_altimeter_100k",       &Animation::HasSource_AltiMeter100k     , &Animation::SetHasSource_AltiMeter100k)
        .add_property("use_source_altimeter_10k",        &Animation::HasSource_AltiMeter10k      , &Animation::SetHasSource_AltiMeter10k)
        .add_property("use_source_altimeter_1k",         &Animation::HasSource_AltiMeter1k       , &Animation::SetHasSource_AltiMeter1k)
        .add_property("use_source_aoa",                  &Animation::HasSource_AOA               , &Animation::SetHasSource_AOA)
        .add_property("use_source_flap",                 &Animation::HasSource_Flap              , &Animation::SetHasSource_Flap)
        .add_property("use_source_air_brake",            &Animation::HasSource_AirBrake          , &Animation::SetHasSource_AirBrake)
        .add_property("use_source_roll",                 &Animation::HasSource_Roll              , &Animation::SetHasSource_Roll)
        .add_property("use_source_pitch",                &Animation::HasSource_Pitch             , &Animation::SetHasSource_Pitch)
        .add_property("use_source_brakes",               &Animation::HasSource_Brakes            , &Animation::SetHasSource_Brakes)
        .add_property("use_source_accel",                &Animation::HasSource_Accel             , &Animation::SetHasSource_Accel)
        .add_property("use_source_clutch",               &Animation::HasSource_Clutch            , &Animation::SetHasSource_Clutch)
        .add_property("use_source_speedo",               &Animation::HasSource_Speedo            , &Animation::SetHasSource_Speedo)
        .add_property("use_source_tacho",                &Animation::HasSource_Tacho             , &Animation::SetHasSource_Tacho)
        .add_property("use_source_turbo",                &Animation::HasSource_Turbo             , &Animation::SetHasSource_Turbo)
        .add_property("use_source_parking_brake",        &Animation::HasSource_ParkingBrake      , &Animation::SetHasSource_ParkingBrake)
        .add_property("use_source_manushift_left_right", &Animation::HasSource_ManuShiftLeftRight, &Animation::SetHasSource_ManuShiftLeftRight)
        .add_property("use_source_manushift_back_forth", &Animation::HasSource_ManuShiftBackForth, &Animation::SetHasSource_ManuShiftBackForth)
        .add_property("use_source_seqential_shift",      &Animation::HasSource_SeqentialShift    , &Animation::SetHasSource_SeqentialShift)
        .add_property("use_source_shifterlin",           &Animation::HasSource_ShifterLin        , &Animation::SetHasSource_ShifterLin)
        .add_property("use_source_torque",               &Animation::HasSource_Torque            , &Animation::SetHasSource_Torque)
        .add_property("use_source_heading",              &Animation::HasSource_Heading           , &Animation::SetHasSource_Heading)
        .add_property("use_source_diff_lock",            &Animation::HasSource_DiffLock          , &Animation::SetHasSource_DiffLock)
        .add_property("use_source_boat_rudder",          &Animation::HasSource_BoatRudder        , &Animation::SetHasSource_BoatRudder)
        .add_property("use_source_boat_throttle",        &Animation::HasSource_BoatThrottle      , &Animation::SetHasSource_BoatThrottle)
        .add_property("use_source_steering_wheel",       &Animation::HasSource_SteeringWheel     , &Animation::SetHasSource_SteeringWheel)
        .add_property("use_source_aileron",              &Animation::HasSource_Aileron           , &Animation::SetHasSource_Aileron)
        .add_property("use_source_elevator",             &Animation::HasSource_Elevator          , &Animation::SetHasSource_Elevator)
        .add_property("use_source_aerial_rudder",        &Animation::HasSource_AerialRudder      , &Animation::SetHasSource_AerialRudder)
        .add_property("use_source_permanent",            &Animation::HasSource_Permanent         , &Animation::SetHasSource_Permanent)
        .add_property("use_source_event",                &Animation::HasSource_Event             , &Animation::SetHasSource_Event )

        .add_property("use_mode_rotation_x",   &Animation::HasMode_ROTATION_X   , &Animation::SetHasMode_ROTATION_X   )
        .add_property("use_mode_rotation_y",   &Animation::HasMode_ROTATION_Y   , &Animation::SetHasMode_ROTATION_Y   )
        .add_property("use_mode_rotation_z",   &Animation::HasMode_ROTATION_Z   , &Animation::SetHasMode_ROTATION_Z   )
        .add_property("use_mode_offset_x",     &Animation::HasMode_OFFSET_X     , &Animation::SetHasMode_OFFSET_X     )
        .add_property("use_mode_offset_y",     &Animation::HasMode_OFFSET_Y     , &Animation::SetHasMode_OFFSET_Y     )
        .add_property("use_mode_offset_z",     &Animation::HasMode_OFFSET_Z     , &Animation::SetHasMode_OFFSET_Z     )
        .add_property("use_mode_auto_animate", &Animation::HasMode_AUTO_ANIMATE , &Animation::SetHasMode_AUTO_ANIMATE )
        .add_property("use_mode_no_flip",      &Animation::HasMode_NO_FLIP      , &Animation::SetHasMode_NO_FLIP      )
        .add_property("use_mode_bounce",       &Animation::HasMode_BOUNCE       , &Animation::SetHasMode_BOUNCE       )
        .add_property("use_mode_event_lock",   &Animation::HasMode_EVENT_LOCK   , &Animation::SetHasMode_EVENT_LOCK   )

        .def_readwrite("ratio",         &Animation::ratio)
        .def_readwrite("lower_limit",   &Animation::lower_limit)
        .def_readwrite("upper_limit",   &Animation::upper_limit)
        .def_readwrite("source",        &Animation::source)
        .def_readwrite("motor_sources", &Animation::motor_sources)
        .def_readwrite("mode",          &Animation::mode)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Animator, "AnimatorVector")
    class_<RigDef::Animator>("Animator")
        .def_readwrite("lenghtening_factor", &Animator::lenghtening_factor)
        .def_readwrite("flags",              &Animator::flags)
        .def_readwrite("short_limit",        &Animator::short_limit)
        .def_readwrite("long_limit",         &Animator::long_limit)
        .def_readwrite("aero_animator",      &Animator::aero_animator)
        .def_readwrite("inertia_defaults",   &Animator::inertia_defaults)
        .def_readwrite("beam_defaults",      &Animator::beam_defaults)
        .def_readwrite("detacher_group",     &Animator::detacher_group)

        .add_property("node_1", &Animator::PY_GetNode1, &Animator::PY_SetNode1)
        .add_property("node_2", &Animator::PY_GetNode2, &Animator::PY_SetNode2)

        .add_property("use_option_visible",           &Animator::HasOption_VISIBLE          , &Animator::SetOption_VISIBLE          )
        .add_property("use_option_invisible",         &Animator::HasOption_INVISIBLE        , &Animator::SetOption_INVISIBLE        )
        .add_property("use_option_airspeed",          &Animator::HasOption_AIRSPEED         , &Animator::SetOption_AIRSPEED         )
        .add_property("use_option_vertical_velocity", &Animator::HasOption_VERTICAL_VELOCITY, &Animator::SetOption_VERTICAL_VELOCITY)
        .add_property("use_option_altimeter_100k",    &Animator::HasOption_ALTIMETER_100K   , &Animator::SetOption_ALTIMETER_100K   )
        .add_property("use_option_altimeter_10k",     &Animator::HasOption_ALTIMETER_10K    , &Animator::SetOption_ALTIMETER_10K    )
        .add_property("use_option_altimeter_1k",      &Animator::HasOption_ALTIMETER_1K     , &Animator::SetOption_ALTIMETER_1K     )
        .add_property("use_option_angle_of_attack",   &Animator::HasOption_ANGLE_OF_ATTACK  , &Animator::SetOption_ANGLE_OF_ATTACK  )
        .add_property("use_option_flap",              &Animator::HasOption_FLAP             , &Animator::SetOption_FLAP             )
        .add_property("use_option_air_brake",         &Animator::HasOption_AIR_BRAKE        , &Animator::SetOption_AIR_BRAKE        )
        .add_property("use_option_roll",              &Animator::HasOption_ROLL             , &Animator::SetOption_ROLL             )
        .add_property("use_option_pitch",             &Animator::HasOption_PITCH            , &Animator::SetOption_PITCH            )
        .add_property("use_option_brakes",            &Animator::HasOption_BRAKES           , &Animator::SetOption_BRAKES           )
        .add_property("use_option_accel",             &Animator::HasOption_ACCEL            , &Animator::SetOption_ACCEL            )
        .add_property("use_option_clutch",            &Animator::HasOption_CLUTCH           , &Animator::SetOption_CLUTCH           )
        .add_property("use_option_speedo",            &Animator::HasOption_SPEEDO           , &Animator::SetOption_SPEEDO           )
        .add_property("use_option_tacho",             &Animator::HasOption_TACHO            , &Animator::SetOption_TACHO            )
        .add_property("use_option_turbo",             &Animator::HasOption_TURBO            , &Animator::SetOption_TURBO            )
        .add_property("use_option_parking",           &Animator::HasOption_PARKING          , &Animator::SetOption_PARKING          )
        .add_property("use_option_shift_left_right",  &Animator::HasOption_SHIFT_LEFT_RIGHT , &Animator::SetOption_SHIFT_LEFT_RIGHT )
        .add_property("use_option_shift_back_forth",  &Animator::HasOption_SHIFT_BACK_FORTH , &Animator::SetOption_SHIFT_BACK_FORTH )
        .add_property("use_option_sequential_shift",  &Animator::HasOption_SEQUENTIAL_SHIFT , &Animator::SetOption_SEQUENTIAL_SHIFT )
        .add_property("use_option_gear_select",       &Animator::HasOption_GEAR_SELECT      , &Animator::SetOption_GEAR_SELECT      )
        .add_property("use_option_torque",            &Animator::HasOption_TORQUE           , &Animator::SetOption_TORQUE           )
        .add_property("use_option_difflock",          &Animator::HasOption_DIFFLOCK         , &Animator::SetOption_DIFFLOCK         )
        .add_property("use_option_boat_rudder",       &Animator::HasOption_BOAT_RUDDER      , &Animator::SetOption_BOAT_RUDDER      )
        .add_property("use_option_boat_throttle",     &Animator::HasOption_BOAT_THROTTLE    , &Animator::SetOption_BOAT_THROTTLE    )
        .add_property("use_option_short_limit",       &Animator::HasOption_SHORT_LIMIT      , &Animator::SetOption_SHORT_LIMIT      )
        .add_property("use_option_long_limit",        &Animator::HasOption_LONG_LIMIT       , &Animator::SetOption_LONG_LIMIT       )
        ;

    class_<RigDef::AeroAnimator>("AeroAnimator")
        .def_readwrite("motor", &AeroAnimator::motor)

        .add_property("use_option_throttle", &AeroAnimator::HasOption_THROTTLE, &AeroAnimator::SetOption_THROTTLE)
        .add_property("use_option_rpm",      &AeroAnimator::HasOption_RPM     , &AeroAnimator::SetOption_RPM     )
        .add_property("use_option_torque",   &AeroAnimator::HasOption_TORQUE  , &AeroAnimator::SetOption_TORQUE  )
        .add_property("use_option_pitch",    &AeroAnimator::HasOption_PITCH   , &AeroAnimator::SetOption_PITCH   )
        .add_property("use_option_status",   &AeroAnimator::HasOption_STATUS  , &AeroAnimator::SetOption_STATUS  )
        ;
}
