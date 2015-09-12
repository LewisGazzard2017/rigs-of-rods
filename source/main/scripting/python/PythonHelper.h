
// Misc utilities for binding Python

#pragma once

#include <boost/python/detail/wrap_python.hpp> 
	// Replaces #include <Python.h>, recommended in Boost docs. Must be included before system headers (Python restriction)
#include <boost/python.hpp>
#include <OgreVector3.h>

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
	
	static Ogre::Vector3 PythonVector3ToOgreVector3(boost::python::object& obj);
};

} // namespace RoR
