
// Misc utilities for binding Python

#pragma once

#include <boost/python/detail/wrap_python.hpp> 
	// Replaces #include <Python.h>, recommended in Boost docs. Must be included before system headers (Python restriction)
#include <boost/python.hpp>

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
	static void LogUnicodeObject(Ogre::Log* m_log, boost::python::object& py_unicode_obj);
	/// Assumes sys.stdout + sys.stderr were assigned StringIO objects.
	static void LogStdOutput(Ogre::Log* m_log, boost::python::object& main_namespace);
	/// Python likes paths with '/'; '\' is an escape.
	static void PathConvertSlashesToForward(std::string& path);
};

} // namespace RoR
