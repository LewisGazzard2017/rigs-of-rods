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
*/

#pragma once

#include "MyGUI_PythonExport.h"

#include <boost/python/detail/wrap_python.hpp>
	// Replaces #include <Python.h>, recommended in Boost docs
	// Must be included before any OS headers - Python limitation

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python.hpp>

#include "MyGUI.h"
#include "PythonBindingUtils.h"

using namespace RoR;
using namespace boost::python;



// ================================================================================
// Stand-alone functions
// ================================================================================

MyGUI::VectorWidgetPtr PY_MyGUI_LayoutManager_LoadLayout(std::string file_path, std::string widget_names_prefix, MyGUI::Widget* parent)
{
	return MyGUI::LayoutManager::getInstance().loadLayout(file_path, widget_names_prefix, parent);
}

void PY_MyGUI_WidgetManager_DestroyWidget(MyGUI::Widget* widget)
{
	MyGUI::WidgetManager::getInstance().destroyWidget(widget);
}



/// ====================================================================================
/// @class Delegate template system - used for binding event callbacks to widgets
///
/// It has 2 parts:
/// 1. "Delegate" - templated class referencing callback function + object to invoke.
/// 2. "EventHandle" - templated container of delegates. Each event for the particular
///                    Widget has one EventHandle instance.
///
/// Both Delegate and EventHandle must be qualified with equal number+types of
///     arguments to the callback function.
/// ====================================================================================
class PY_MyGUI_DelegateBase
{
public:
	PY_MyGUI_DelegateBase(object obj, object func):
		m_object(obj),
		m_function(func)
		{}

	virtual ~PY_MyGUI_DelegateBase()
	{
	}

	/*virtual bool ComparePythonDelegate(PY_MyGUI_DelegateBase* other)
	{
		return (other->m_object == this->m_object) && (other->m_function == this->m_function);
	}*/

	virtual bool ComparePythonDelegate( const PY_MyGUI_DelegateBase* const other)
	{
		return (other->m_object == this->m_object) && (other->m_function == this->m_function);
	}

	/// Cloned from MyGUI header "MyGUI_DelegateImplement.h".
	/// I don't really know what it's for. ~ only_a_ptr
	/// The IDelegateUnlink interface is defined in "MyGUI_Delegate.h" and inherited by "MyGUI::Widget"
	virtual bool compare(MyGUI::delegates::IDelegateUnlink* _unlink) const
	{
		return false;
	}

protected:
	boost::python::object m_object;
	boost::python::object m_function;
};


// Macro for implementing per-qualified-Delegate comparsion.
//
// Slow dynamic_cast<>, but this is only invoked when a callback
//      is being removed from a widget, which rarely happens.
#define COMPARE_MyGUI_IDELEGATE_AND_PY_DELEGATEBASE(THIS_PTR, OTHER_PTR)                                      \
	PY_MyGUI_DelegateBase* delegate_base = dynamic_cast<PY_MyGUI_DelegateBase*>(OTHER_PTR);                   \
	if (delegate_base == nullptr) { return false; }                                                           \
	const PY_MyGUI_DelegateBase* const this_base = static_cast<const PY_MyGUI_DelegateBase* const>(THIS_PTR); \
	return delegate_base->ComparePythonDelegate(this_base);                                                   \



/// Delegate for callbacks with 1 parameter - Widget*
typedef MyGUI::delegates::IDelegate1<MyGUI::Widget*> PY_MyGUI_IDelegate1_Widget;
class PY_MyGUI_Delegate_WidgetVoid: 
	public PY_MyGUI_IDelegate1_Widget, //MyGUI::delegates::IDelegate1<MyGUI::Widget*>,
	public PY_MyGUI_DelegateBase
{
public:
	PY_MyGUI_Delegate_WidgetVoid(object obj, object func):
		PY_MyGUI_DelegateBase(obj, func)
		{}

	// <typeid> should be disabled... forever, if possible... >:(
	virtual bool isType( const std::type_info& _type)
	{
		return _type == typeid(PY_MyGUI_Delegate_WidgetVoid);
	}

	virtual bool compare(MyGUI::delegates::IDelegate1<MyGUI::Widget*>* other) const
	{
		COMPARE_MyGUI_IDELEGATE_AND_PY_DELEGATEBASE(this, other)
	}

	virtual void invoke(MyGUI::Widget* widget)
	{
		// Without "boost::ref", you receive "TypeError, no converter found"; see http://stackoverflow.com/a/5337551
		if (m_object.is_none())
		{
			m_function(boost::ref(widget));
		}
		else
		{
			
			m_function(m_object, boost::ref(widget));
		}
	}
};

typedef MyGUI::delegates::IDelegate2<MyGUI::Widget*, MyGUI::Widget*> PY_MyGUI_IDelegate2_Widget_Widget;

/// Delegate for callbacks with 2 parameters - Widget* and Widget*
class PY_MyGUI_Delegate_WidgetWidget: 
	public PY_MyGUI_IDelegate2_Widget_Widget,
	public PY_MyGUI_DelegateBase
{
public:
	PY_MyGUI_Delegate_WidgetWidget(object obj, object func):
		PY_MyGUI_DelegateBase(obj, func)
		{}

	// <typeid> should be disabled... forever, if possible... >:(
	virtual bool isType( const std::type_info& _type)
	{
		return _type == typeid(PY_MyGUI_IDelegate2_Widget_Widget);
	}

	virtual bool compare(MyGUI::delegates::IDelegate2<MyGUI::Widget*, MyGUI::Widget*>* other) const
	{
		COMPARE_MyGUI_IDELEGATE_AND_PY_DELEGATEBASE(this, other)
	}

	virtual void invoke(MyGUI::Widget* widget_a, MyGUI::Widget* widget_b)
	{
		// Without "boost::ref", you receive "TypeError, no converter found"; see http://stackoverflow.com/a/5337551
		if (m_object.is_none())
		{
			m_function(boost::ref(widget_a), boost::ref(widget_b));
		}
		else
		{
			
			m_function(m_object, boost::ref(widget_a), boost::ref(widget_b));
		}
	}
};

// ================================================================================
// Widget typecasting extension methods
// ================================================================================

/// Extension method
/// Adds Python callback to MyGUI::EventHandle* container
static void PY_EventHandleWidgetWidget_AddDelegate(
		MyGUI::EventHandle_WidgetWidget* _this, 
		boost::python::object object, 
		boost::python::object function)
{
	*_this += new PY_MyGUI_Delegate_WidgetWidget(object, function);
}


/// Extension method
/// Adds Python callback to MyGUI::EventHandle* container
static void PY_EventHandleWidgetVoid_AddDelegate(
		MyGUI::EventHandle_WidgetVoid* _this, 
		boost::python::object object, 
		boost::python::object function)
{
	*_this += new PY_MyGUI_Delegate_WidgetVoid(object, function);
}




BOOST_PYTHON_MODULE(ror_gui)
{
    // WIDGETS
	MyGUI_PythonExport::ExportClassWidget();
	MyGUI_PythonExport::ExportClassTextBox();


	class_<MyGUI::MenuControl, bases<MyGUI::Widget>, boost::noncopyable>("MyGUI_MenuControl", no_init)
		.def("get_item_count", &MyGUI::MenuControl::getItemCount)
		//.def("get_item_at", &MyGUI::MenuControl::getItemAt)
		;

	// Ready
	class_<MyGUI::Button, bases<MyGUI::TextBox>, boost::noncopyable>("MyGUI_Button", no_init)
		;

	// Ready
	class_<MyGUI::EditBox, bases<MyGUI::TextBox>, boost::noncopyable>("MyGUI_EditBox", no_init)
		;

	// Ready
	class_<MyGUI::MenuBar, bases<MyGUI::MenuControl>, boost::noncopyable>("MyGUI_MenuBar", no_init)
		;

	// Ready
	class_<MyGUI::ComboBox, bases<MyGUI::EditBox>, boost::noncopyable>("MyGUI_ComboBox", no_init)
		;

	class_<MyGUI::PopupMenu, bases<MyGUI::MenuControl>, boost::noncopyable>("MyGUI_PopupMenu", no_init)
		;

	class_<MyGUI::Window, bases<MyGUI::TextBox>, boost::noncopyable>("MyGUI_Window", no_init)
		;

	class_<MyGUI::EventHandle_WidgetVoid>("MyGUI_EventHandle_WidgetVoid")
		.def("add_delegate", &PY_EventHandleWidgetVoid_AddDelegate) // Static extension method
		;

	class_<MyGUI::EventHandle_WidgetWidget>("MyGUI_EventHandle_WidgetWidget")
		.def("add_delegate", &PY_EventHandleWidgetWidget_AddDelegate) // Static extension method
		;

	def("load_layout", PY_MyGUI_LayoutManager_LoadLayout);

	// MISC
	MyGUI_PythonExport::ExportDatatypes();

}

// static
// To be called on Python init
// Source: https://wiki.python.org/moin/boost.python/EmbeddingPython
void MyGUI_PythonExport::AppendInittab()
{
	PyImport_AppendInittab("ror_gui", PyInit_ror_gui);
}


