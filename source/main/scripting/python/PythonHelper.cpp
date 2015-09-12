
#include "PythonHelper.h"

#include "OgreLog.h"
#include "MyGUI_UString.h"

using namespace RoR;

// Static
const char* PythonHelper::UnicodeKindToString(int py_kind)
{
	switch (py_kind)
	{
		case PyUnicode_WCHAR_KIND: return "PyUnicode_WCHAR_KIND";
		case PyUnicode_1BYTE_KIND: return "PyUnicode_1BYTE_KIND";
		case PyUnicode_2BYTE_KIND: return "PyUnicode_2BYTE_KIND";
		case PyUnicode_4BYTE_KIND: return "PyUnicode_4BYTE_KIND";
		default: return "~Invalid~";
	}
}

// Static
void PythonHelper::PathConvertSlashesToForward(std::string& path)
{
	unsigned pos;
	while ((pos = path.find('\\')) != std::string::npos)
	{
		path.replace(pos, 1, "/");
	}
}

// Static
Ogre::Vector3 PythonHelper::Vector3_Py2Ogre(boost::python::object& pos)
{
	using namespace boost::python;
	auto x = extract<float>(pos.attr("x"));
	auto y = extract<float>(pos.attr("y"));
	auto z = extract<float>(pos.attr("z"));
	return Ogre::Vector3(x,y,z);
}

// Static
Ogre::ColourValue PythonHelper::Color_Py2Ogre(boost::python::object& pos)
{
	using namespace boost::python;
	auto r = extract<float>(pos.attr("r"));
	auto g = extract<float>(pos.attr("g"));
	auto b = extract<float>(pos.attr("b"));
	auto a = extract<float>(pos.attr("a"));
	return Ogre::ColourValue(r,g,b,a);
}

