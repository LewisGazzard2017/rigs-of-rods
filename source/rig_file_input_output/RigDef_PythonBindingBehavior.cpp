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

void PythonBinding::ExportTrigger()
{
    class_<RigDef::RailGroup>("RailGroup")   
        .def_readwrite("id",        &RailGroup::id)
        .def_readwrite("node_list", &RailGroup::node_list)
        ;

    class_<RigDef::Ropable>("Ropable")   
        .def_readwrite("node",               &Ropable::node)
        .def_readwrite("int group",          &Ropable::int group)
        .def_readwrite("_has_group_set",     &Ropable::_has_group_set)
        .def_readwrite("multilock",          &Ropable::multilock)
        .def_readwrite("_has_multilock_set", &Ropable::_has_multilock_set)
        ;

    class_<RigDef::Rope>("Rope")   
        .def_readwrite("root_node",          &Rope::root_node)
        .def_readwrite("end_node",           &Rope::end_node)
        .def_readwrite("invisible",          &Rope::invisible)
        .def_readwrite("_has_invisible_set", &Rope::_has_invisible_set)
        .def_readwrite("beam_defaults",      &Rope::beam_defaults)
        .def_readwrite("detacher_group",     &Rope::detacher_group)
        ;

    class_<RigDef::Screwprop>("Screwprop")   
        .def_readwrite("prop_node", &Screwprop::prop_node)
        .def_readwrite("back_node", &Screwprop::back_node)
        .def_readwrite("top_node",  &Screwprop::top_node)
        .def_readwrite("power",     &Screwprop::power)
        ;
    
    PYTHON_REGISTER_STD_VECTOR(RigDef::NodeCollision, "NodeCollisionVector")
        class_<RigDef::NodeCollision>("NodeCollision")   NodeCollision
        .def_readwrite("node",   &NodeCollision::node)
        .def_readwrite("radius", &NodeCollision::radius)
        ;
    
    class_<RigDef::Lockgroup>("Lockgroup")
        // Const
        .def_readonly("SPECIAL_NUMBER_DEFAULT", &Lockgroup::LOCKGROUP_DEFAULT)
        .def_readonly("SPECIAL_NUMBER_NOLOCK",  &Lockgroup::LOCKGROUP_NOLOCK)
        // Attrs
        .def_readwrite("number",  &Lockgroup::number)
        .def_readwrite("nodes",   &Lockgroup::nodes)
        ;
    
    class_<RigDef::CollisionBox>("CollisionBox")   
        .def_readwrite("nodes", &CollisionBox::nodes)
        ;
    
    class_<RigDef::Hook>("Hook")   
        .def_readwrite("node",                        &Hook::node)
        .def_readwrite("option_hook_range",           &Hook::option_hook_range)
        .def_readwrite("option_speed_coef",           &Hook::option_speed_coef)
        .def_readwrite("option_max_force",            &Hook::option_max_force)
        .def_readwrite("option_hookgroup",            &Hook::option_hookgroup)
        .def_readwrite("option_lockgroup",            &Hook::option_lockgroup)
        .def_readwrite("option_timer",                &Hook::option_timer)
        .def_readwrite("option_minimum_range_meters", &Hook::option_minimum_range_meters)
        
        .add_property("use_option_self_lock",  &Hook:: HasOptionSelfLock,  &Hook::SetHasOptionSelfLock )
        .add_property("use_option_auto_lock",  &Hook::HasOptionAutoLock,  &Hook::SetHasOptionAutoLock )
        .add_property("use_option_no_disable", &Hook::HasOptionNoDisable, &Hook::SetHasOptionNoDisable)
        .add_property("use_option_no_rope",    &Hook::HasOptionNoRope,    &Hook::SetHasOptionNoRope   )
        .add_property("use_option_visible",    &Hook::HasOptionVisible,   &Hook::SetHasOptionVisible  )
        ;
    
    class_<RigDef::Rotator>("Rotator")
        .add_property("node_1", &Rotator::PY_GetNode1, &Rotator::PY_SetNode1)
        .add_property("node_2", &Rotator::PY_GetNode2, &Rotator::PY_SetNode2)
        
        .def("get_base_plate_node",   &Rotator::PY_GetBasePlateNode)
        .def("set_base_plate_node",   &Rotator::PY_SetBasePlateNode)
        .def("get_rot_plate_node",    &Rotator::PY_GetRotPlateNode)
        .def("set_rot_plate_node",    &Rotator::PY_SetRotPlateNode)
        
        .def_readwrite("rate",             &Rotator::rate)
        .def_readwrite("spin_left_key",    &Rotator::spin_left_key)
        .def_readwrite("spin_right_key",   &Rotator::spin_right_key)
        .def_readwrite("inertia",          &Rotator::inertia)
        .def_readwrite("inertia_defaults", &Rotator::inertia_defaults)
        .def_readwrite("engine_coupling",  &Rotator::engine_coupling)
        .def_readwrite("needs_engine",     &Rotator::needs_engine)
        ;
    
    
    class_<RigDef::Rotator2>("Rotator2")
        .add_property("node_1", &Rotator2::PY_GetNode1, &Rotator2::PY_SetNode1)
        .add_property("node_2", &Rotator2::PY_GetNode2, &Rotator2::PY_SetNode2)
        
        .def("get_base_plate_node",   &Rotator2::PY_GetBasePlateNode)
        .def("set_base_plate_node",   &Rotator2::PY_SetBasePlateNode)
        .def("get_rot_plate_node",    &Rotator2::PY_GetRotPlateNode)
        .def("set_rot_plate_node",    &Rotator2::PY_SetRotPlateNode)
        
        .def_readwrite("rate",             &Rotator2::rate)
        .def_readwrite("spin_left_key",    &Rotator2::spin_left_key)
        .def_readwrite("spin_right_key",   &Rotator2::spin_right_key)
        .def_readwrite("inertia",          &Rotator2::inertia)
        .def_readwrite("inertia_defaults", &Rotator2::inertia_defaults)
        .def_readwrite("engine_coupling",  &Rotator2::engine_coupling)
        .def_readwrite("needs_engine",     &Rotator2::needs_engine)
        .def_readwrite("rotating_force",   &Rotator2::rotating_force)
        .def_readwrite("tolerance",        &Rotator2::tolerance)
        .def_readwrite("description",      &Rotator2::description)
        ;
    
    PYTHON_REGISTER_STD_VECTOR(RigDef::Tie, "TieVector")
    class_<RigDef::Tie>("Tie")
        .add_property("is_invisible", &Tie::PY_HasOptionInvisible, &Tie::PY_SetOptionInvisible )
        .def_readwrite("root_node",         &Tie::root_node)
        .def_readwrite("max_reach_length",  &Tie::max_reach_length)
        .def_readwrite("auto_shorten_rate", &Tie::auto_shorten_rate)
        .def_readwrite("min_length",        &Tie::min_length)
        .def_readwrite("max_length",        &Tie::max_length)
        .def_readwrite("max_stress",        &Tie::max_stress)
        .def_readwrite("beam_defaults",     &Tie::beam_defaults)
        .def_readwrite("detacher_group",    &Tie::detacher_group)
        .def_readwrite("group",             &Tie::group)
        .def_readwrite("_group_set",        &Tie::_group_set)
        ;
    
    PYTHON_REGISTER_STD_VECTOR(RigDef::Cab, "CabTriangleVector")
    class_<RigDef::Cab>("CabTriangle")
        .def("get_node", &Cab::PY_GetNode)
        .def("set_node", &Cab::PY_SetNode)
        .def("get_option_D_contact_buoyant", &Cab::GetOption_D_ContactBuoyant     )
        .def("get_option_F_10x_tougher_buoyant", &Cab::GetOption_F_10xTougherBuoyant  )
        .def("get_option_S_unpenetrable_buoyant", &Cab::GetOption_S_UnpenetrableBuoyant)
        
        .add_property("use_option_c_contact",           &Cab::HasOption_c_CONTACT          , &Cab::SetOption_c_CONTACT          )
        .add_property("use_option_b_buoyant",           &Cab::HasOption_b_BUOYANT          , &Cab::SetOption_b_BUOYANT          )
        .add_property("use_option_p_10xtougher",        &Cab::HasOption_p_10xTOUGHER       , &Cab::SetOption_p_10xTOUGHER       )
        .add_property("use_option_u_invulnerable",      &Cab::HasOption_u_INVULNERABLE     , &Cab::SetOption_u_INVULNERABLE     )
        .add_property("use_option_s_buoyant_no_drag",   &Cab::HasOption_s_BUOYANT_NO_DRAG  , &Cab::SetOption_s_BUOYANT_NO_DRAG  )
        .add_property("use_option_r_buoyant_only_drag", &Cab::HasOption_r_BUOYANT_ONLY_DRAG, &Cab::SetOption_r_BUOYANT_ONLY_DRAG)
        ;


    PYTHON_REGISTER_STD_VECTOR(RigDef::Texcoord, "TexcoordVector")
    class_<RigDef::Texcoord>("Texcoord")
        .def_readwrite("u", &Texcoord::u)
        .def_readwrite("v", &Texcoord::v)
        ;
    
    PYTHON_REGISTER_STD_VECTOR(RigDef::Submesh, "SubmeshVector")
    class_<RigDef::Submesh>("Submesh")
        .def_readwrite("use_backmesh",  &Submesh::backmesh)
        .def_readwrite("texcoords",     &Submesh::texcoords)
        .def_readwrite("cab_triangles", &Submesh::cab_triangles)
        ;
    
    class_<RigDef::SlideNode>("SlideNode")
        .def_readwrite("slide_node",              &SlideNode::slide_node)
        .def_readwrite("rail_nodes",              &SlideNode::rail_nodes)
        .def_readwrite("spring_rate",             &SlideNode::spring_rate)
        .def_readwrite("break_force",             &SlideNode::break_force)
        .def_readwrite("tolerance",               &SlideNode::tolerance)
        .def_readwrite("railgroup_id",            &SlideNode::railgroup_id)
        .def_readwrite("_railgroup_id_set",       &SlideNode::_railgroup_id_set)
        .def_readwrite("attachment_rate",         &SlideNode::attachment_rate)
        .def_readwrite("max_attachment_distance", &SlideNode::max_attachment_distance)
        .def_readwrite("_break_force_set",        &SlideNode::_break_force_set)
        
        .add_property("use_constraint_attach_all",     &SlideNode::HasConstraint_a_AttachAll    , &SlideNode::SetConstraint_a_AttachAll    )
        .add_property("use_constraint_attach_foreign", &SlideNode::HasConstraint_f_AttachForeign, &SlideNode::SetConstraint_f_AttachForeign)
        .add_property("use_constraint_attach_self",    &SlideNode::HasConstraint_s_AttachSelf   , &SlideNode::SetConstraint_s_AttachSelf   )
        .add_property("use_constraint_attach_none",    &SlideNode::HasConstraint_n_AttachNone   , &SlideNode::SetConstraint_n_AttachNone   )
        ;

}
