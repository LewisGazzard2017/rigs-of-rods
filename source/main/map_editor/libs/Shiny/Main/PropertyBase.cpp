#include "MapEditor_Global.h"
#include "PropertyBase.hpp"
#include "RoRPrerequisites.h"

#include <vector>
#include <iostream>

//STUNTPORT #include <boost/lexical_cast.hpp>
//STUNTPORT #include <boost/algorithm/string.hpp>
#include <OgreStringConverter.h>

#include <fstream>

namespace sh
{

	IntValue::IntValue(int in)
		: mValue(in)
	{
	}

	IntValue::IntValue(const std::string& in)
	{
        mValue = Ogre::StringConverter::parseInt(in);
		//STUNTPORT_ORIG//mValue = boost::lexical_cast<int>(in);
	}

	std::string IntValue::serialize()
	{
		//STUNTPORT ORIG//return TOSTRING(mValue);
        return Ogre::StringConverter::toString(mValue);
	}

	// ------------------------------------------------------------------------------

	BooleanValue::BooleanValue (bool in)
		: mValue(in)
	{
	}

	BooleanValue::BooleanValue (const std::string& in)
	{
		if (in == "true")
			mValue = true;
		else if (in == "false")
			mValue = false;
		else
		{
			std::stringstream msg;
			msg << "sh::BooleanValue: Warning: Unrecognized value \"" << in << "\" for property value of type BooleanValue";
			throw std::runtime_error(msg.str());
		}
	}

	std::string BooleanValue::serialize ()
	{
		if (mValue)
			return "true";
		else
			return "false";
	}

	// ------------------------------------------------------------------------------

	StringValue::StringValue (const std::string& in)
	{
		mStringValue = in;
	}

	std::string StringValue::serialize()
	{
		return mStringValue;
	}

	// ------------------------------------------------------------------------------

	LinkedValue::LinkedValue (const std::string& in)
	{
		mStringValue = in;
		mStringValue.erase(0, 1);
	}

	std::string LinkedValue::serialize()
	{
		throw std::runtime_error ("can't directly get a linked value");
	}

	std::string LinkedValue::get(PropertySetGet* context) const
	{
		PropertyValuePtr p = context->getProperty(mStringValue);
		return retrieveValue<StringValue>(p, NULL).get();
	}

	// ------------------------------------------------------------------------------

	FloatValue::FloatValue (float in)
	{
		mValue = in;
	}

	FloatValue::FloatValue (const std::string& in)
	{
		mValue = Ogre::StringConverter::parseReal(in); //STUNTPORT ORIG//PARSEREAL (in);
	}

	std::string FloatValue::serialize ()
	{
		//STUNTPORT_ORIG//return TOSTRING(mValue);
        return Ogre::StringConverter::toString(mValue);
	}

	// ------------------------------------------------------------------------------

	Vector2::Vector2 (float x, float y)
		: mX(x)
		, mY(y)
	{
	}

	Vector2::Vector2 (const std::string& in)
	{
		// // //std::vector<std::string> tokens;
		// // //boost::split(tokens, in, boost::is_any_of(" "));
        Ogre::StringVector tokens = Ogre::StringUtil::split(in, " ");
		assert ((tokens.size() == 2) && "Invalid Vector2 conversion");
		mX = PARSEREAL (tokens[0]);
		mY = PARSEREAL  (tokens[1]);
	}

	std::string Vector2::serialize ()
	{
		return TOSTRING(mX) + " "
			+ TOSTRING(mY);
	}

	// ------------------------------------------------------------------------------

	Vector3::Vector3 (float x, float y, float z)
		: mX(x)
		, mY(y)
		, mZ(z)
	{
	}

	Vector3::Vector3 (const std::string& in)
	{
		// // //std::vector<std::string> tokens;
		// // //boost::split(tokens, in, boost::is_any_of(" "));
        Ogre::StringVector tokens = Ogre::StringUtil::split(in, " ");
		assert ((tokens.size() == 3) && "Invalid Vector3 conversion");
		mX = PARSEREAL  (tokens[0]);
		mY = PARSEREAL  (tokens[1]);
		mZ = PARSEREAL  (tokens[2]);
	}

	std::string Vector3::serialize ()
	{
		return TOSTRING(mX) + " "
			+ TOSTRING(mY) + " "
			+ TOSTRING(mZ);
	}

	// ------------------------------------------------------------------------------

	Vector4::Vector4 (float x, float y, float z, float w)
		: mX(x)
		, mY(y)
		, mZ(z)
		, mW(w)
	{
	}

	Vector4::Vector4 (const std::string& in)
	{
		// // // std::vector<std::string> tokens;
		// // // boost::split(tokens, in, boost::is_any_of(" "));
        Ogre::StringVector tokens = Ogre::StringUtil::split(in, " ");
		assert ((tokens.size() == 4) && "Invalid Vector4 conversion");
		mX = PARSEREAL  (tokens[0]);
		mY = PARSEREAL  (tokens[1]);
		mZ = PARSEREAL  (tokens[2]);
		mW = PARSEREAL  (tokens[3]);
	}

	std::string Vector4::serialize ()
	{
		return TOSTRING(mX) + " "
			+ TOSTRING(mY) + " "
			+ TOSTRING(mZ) + " "
			+ TOSTRING(mW);
	}

	// ------------------------------------------------------------------------------

	void PropertySet::setProperty (const std::string& name, PropertyValuePtr &value, PropertySetGet* context)
	{
		if (!setPropertyOverride (name, value, context))
		{
			std::stringstream msg;
			msg << "sh::PropertySet: Warning: No match for property with name '" << name << "'";
			throw std::runtime_error(msg.str());
		}
	}

	bool PropertySet::setPropertyOverride (const std::string& name, PropertyValuePtr &value, PropertySetGet* context)
	{
		// if we got here, none of the sub-classes were able to make use of the property
		return false;
	}

	// ------------------------------------------------------------------------------

	PropertySetGet::PropertySetGet (PropertySetGet* parent)
		: mParent(parent)
		, mContext(NULL)
	{
	}

	PropertySetGet::PropertySetGet ()
		: mParent(NULL)
		, mContext(NULL)
	{
	}

	void PropertySetGet::setParent (PropertySetGet* parent)
	{
		mParent = parent;
	}

	void PropertySetGet::setContext (PropertySetGet* context)
	{
		mContext = context;
	}

	PropertySetGet* PropertySetGet::getContext()
	{
		return mContext;
	}

	void PropertySetGet::setProperty (const std::string& name, PropertyValuePtr value)
	{
		mProperties [name] = value;
	}

	void PropertySetGet::deleteProperty(const std::string &name)
	{
		mProperties.erase(name);
	}

	PropertyValuePtr& PropertySetGet::getProperty (const std::string& name)
	{
		bool found = (mProperties.find(name) != mProperties.end());

		if (!found)
		{
			if (!mParent)
				throw std::runtime_error ("Trying to retrieve property \"" + name + "\" that does not exist");
			else
				return mParent->getProperty (name);
		}
		else
			return mProperties[name];
	}

	bool PropertySetGet::hasProperty (const std::string& name) const
	{
		bool found = (mProperties.find(name) != mProperties.end());

		if (!found)
		{
			if (!mParent)
				return false;
			else
				return mParent->hasProperty (name);
		}
		else
			return true;
	}

	void PropertySetGet::copyAll (PropertySet* target, PropertySetGet* context, bool copyParent)
	{
		if (mParent && copyParent)
			mParent->copyAll (target, context);
		for (PropertyMap::iterator it = mProperties.begin(); it != mProperties.end(); ++it)
		{
			target->setProperty(it->first, it->second, context);
		}
	}

	void PropertySetGet::copyAll (PropertySetGet* target, PropertySetGet* context, bool copyParent)
	{
		if (mParent && copyParent)
			mParent->copyAll (target, context);
		for (PropertyMap::iterator it = mProperties.begin(); it != mProperties.end(); ++it)
		{
			std::string val = retrieveValue<StringValue>(it->second, this).get();
			target->setProperty(it->first, sh::makeProperty(new sh::StringValue(val)));
		}
	}

	void PropertySetGet::save(std::ofstream &stream, const std::string& indentation)
	{
		for (PropertyMap::iterator it = mProperties.begin(); it != mProperties.end(); ++it)
		{
			if (typeid( *(it->second) ) == typeid(LinkedValue))
				stream << indentation << it->first << " " << "$" + static_cast<LinkedValue*>(&*(it->second))->_getStringValue() << '\n';
			else
				stream << indentation << it->first << " " << retrieveValue<StringValue>(it->second, this).get() << '\n';
		}
	}
}
