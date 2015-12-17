
ROR LUA SCRIPTING
=================

RoR invokes specific script files with defined purpose.

When a vehicle is spawned, the application invokes
"init_rig.lua" script. This script sets up per-vehicle
systems, such as powertrain.

POWERTRAIN
----------

The powertrain is set up by init_rig.lua which recieves
data from the application as 2 global tables:
    truckfile - contains vehicle parameters specified by author
    rig       - contains vehicle data assigned by the application

Below is a reference to the provided data tables.

truckfile.engine:
[Doc: http://www.rigsofrods.com/wiki/pages/Truck_Description_File#Engine]
    shift_down_rpm     - Number
    shift_up_rpm       - Number
    torque             - Number
    global_gear_ratio  - Number
    reverse_gear_ratio - Number
    neutral_gear_ratio - Number
    forward_gears      - List of gear ratios (numbers). Indexed from 0

truckfile.engoption:
[Doc: http://www.rigsofrods.com/wiki/pages/Truck_Description_File#Engoption]
    type                      - String. "t" = truck, "c" = car, "e" = electric car
    inertia                   - Number
    clutch_force              - Number
    _clutch_force_use_default - Boolean
    shift_time                - Number
    clutch_time               - Number
    post_shift_time           - Number
    idle_rpm                  - Number
    _idle_rpm_use_default     - Boolean
    stall_rpm                 - Number
    max_idle_mixture          - Number
    min_idle_mixture          - Number

truckfile.engturbo (version 1):
[Doc: http://www.rigsofrods.com/wiki/pages/Truck_Description_File#Engturbo]
    inertia_factor    - Number
    num_turbos        - Number, max 4
    additional_torque - Number
    engine_rpm_op     - Number

truckfile.engturbo (version 2):
[Doc: http://www.rigsofrods.com/wiki/pages/Truck_Description_File#Engturbo]
    inertia_factor      - Number
    num_turbos          - Number
    max_psi             - Number
    engine_rpm_op       - Number
    has_bov             - Boolean
    bov_min_psi         - Number
    has_wastegate       - Boolean
    wastegate_max_psi   - Number
    wastegate_threshold - Number
    has_antilag         - Boolean
    antilag_chance      - Number
    antilag_min_rpm     - Number
    antilag_power       - Number


