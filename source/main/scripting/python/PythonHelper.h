
// Misc utilities for binding Python

#pragma once

#include <boost/python/detail/wrap_python.hpp> 
	// Replaces #include <Python.h>, recommended in Boost docs. Must be included before system headers (Python restriction)
#include <boost/python.hpp>
#include <OgreVector3.h>
#include <OgreColourValue.h>

#include "RigEditor_Types.h"

// Forward decl.
namespace Ogre
{
	class Log;
}

namespace RoR
{

class PythonHelper
{
public:
	static const char* UnicodeKindToString(int py_kind);

	/// Python likes paths with '/'; '\' is an escape.
	static void PathConvertSlashesToForward(std::string& path);
	
	static Ogre::Vector3          Vector3_FromPython(boost::python::object& obj);
	static RigEditor::Vector2int  Vector2int_FromPython(boost::python::object& obj);
	static Ogre::ColourValue      Color_FromPython(boost::python::object& obj);
};

} // namespace RoR
