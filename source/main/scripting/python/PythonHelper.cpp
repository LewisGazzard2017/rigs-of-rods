
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
void PythonHelper::LogUnicodeObject(Ogre::Log* m_log, boost::python::object& py_unicode_obj)
{
	m_log->logMessage("LogPythonUnicodeObject(): START");

	if (py_unicode_obj.is_none())
	{
		m_log->logMessage("LogPythonUnicodeObject FAIL, py_unicode_obj is None");
		return;
	}
	m_log->logMessage("LogPythonUnicodeObject(): LINE DONE is_none()?");

	if (!PyUnicode_Check(py_unicode_obj.ptr()))
	{
		m_log->logMessage("LogPythonUnicodeObject():failed to get info :-( [ failed PyUnicode_Check test]");
		return;
	}
	m_log->logMessage("LogPythonUnicodeObject(): LINE DONE PyUnicode_Check ");

	if (!PyUnicode_READY(py_unicode_obj.ptr()))
	{
		m_log->logMessage("LogPythonUnicodeObject(): We are not PyUnicode_READY...");
		Py_UNICODE* w_str = PyUnicode_AS_UNICODE(py_unicode_obj.ptr());
		if (w_str == nullptr)
		{
			m_log->logMessage("LogPythonUnicodeObject(): PyUnicode_AS_UNICODE() gave NULL :-(");
			return;
		}
		MyGUI::UString u_str(w_str);

		m_log->logMessage("LogPythonUnicodeObject(): OUTPUT:");
		m_log->logMessage(u_str);
		return;
	}
	int kind = PyUnicode_KIND(py_unicode_obj.ptr());
	m_log->logMessage(std::string("LogPythonUnicodeObject(): PyUnicode_READY OK! Kind: ")
		+ UnicodeKindToString(kind));

	Py_UCS1* ucs1 = nullptr;
	Py_UCS2* ucs2 = nullptr;
	Py_UCS4* ucs4 = nullptr;

	m_log->logMessage("LogPythonUnicodeObject(): OUTPUT:");
	switch (kind)
	{
	case PyUnicode_WCHAR_KIND: break;

	case PyUnicode_1BYTE_KIND:
	{
		ucs1 = PyUnicode_1BYTE_DATA(py_unicode_obj.ptr()); m_log->logMessage((char*)ucs1); break;
	}

	case PyUnicode_2BYTE_KIND:
	{
		ucs2 = PyUnicode_2BYTE_DATA(py_unicode_obj.ptr());
		MyGUI::UString u_str((wchar_t*)ucs2);
		m_log->logMessage(u_str);
		break;
	}

	case PyUnicode_4BYTE_KIND:
	{
		ucs4 = PyUnicode_4BYTE_DATA(py_unicode_obj.ptr()); break;
	}
	}
}

// Static
void PythonHelper::LogStdOutput(Ogre::Log* m_log, boost::python::object& main_namespace)
{
	// https://docs.python.org/3/library/io.html#io.StringIO
	using namespace boost::python;

	m_log->logMessage("LogPythonStdOutput(): START");

	m_log->logMessage("LogPythonStdOutput(): getting stdout");
	object stdout_str = eval("sys.stdout.getvalue()", main_namespace);
	m_log->logMessage("LogPythonStdOutput(): logging stdout");
	PythonHelper::LogUnicodeObject(m_log, stdout_str);

	m_log->logMessage("LogPythonStdOutput(): getting stderr");
	object stderr_str = eval("sys.stderr.getvalue()", main_namespace);
	m_log->logMessage("LogPythonStdOutput(): logging stderr");
	PythonHelper::LogUnicodeObject(m_log, stderr_str);

	m_log->logMessage("LogPythonStdOutput(): END");
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

