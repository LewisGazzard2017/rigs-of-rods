/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2013+     Petr Ohlidal & contributors

    For more information, see http://www.rigsofrods.org/

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

/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

/// @file
/// @author Thomas Fischer
/// @date   31th of July 2009
/// @brief AngelScript registrations of OGRE objects, mostly math-related.

// This code makes frequent use of "placement-new" costruct
// Example: `new(self) Vector3();`
// More info: https://en.wikipedia.org/wiki/Placement_syntax

// TODO: Old angelscript used lowercase class names: degree, radian, vector3, quaternion

#include "OgreAngelscript.h"

#include "AngelScriptUtils.h"
#include "RoRPrerequisites.h"

#include <angelscript.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>

using namespace Ogre;
using namespace AngelScript;

#define LOGSTREAM Ogre::LogManager::getSingleton().stream() << "[RoR|Scripting] "

// Forward decl.
void RegisterOgreVector3(AsObjectRegProxy& proxy);
void RegisterOgreRadian(AsObjectRegProxy& proxy);
void RegisterOgreDegree(AsObjectRegProxy& proxy);
void RegisterOgreQuaternion(AsObjectRegProxy& proxy, AsSetupHelper* helper);
void RegisterColourValue(AsObjectRegProxy& proxy);
void RegisterAxisAlignedBox(AsObjectRegProxy& proxy);
void RegisterSphere(AsObjectRegProxy& proxy);

bool RegisterOgreObjects(AsSetupHelper* helper)
{
    // Register object types first - methods have circular dependencies
    asDWORD flags = asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLFLOATS;

    AsObjectRegProxy as_degree (helper, "Degree",         sizeof(Ogre::Degree),         flags);
    AsObjectRegProxy as_radian (helper, "Radian",         sizeof(Ogre::Radian),         flags);
    AsObjectRegProxy as_vector3(helper, "Vector3",        sizeof(Ogre::Vector3),        flags);
    AsObjectRegProxy as_quat   (helper, "Quaternion",     sizeof(Ogre::Quaternion),     flags);
    AsObjectRegProxy as_colour (helper, "ColourValue",    sizeof(Ogre::ColourValue),    flags); // From OGITOR
    AsObjectRegProxy as_sphere (helper, "Sphere",         sizeof(Ogre::Sphere),         flags); // From OGITOR
    AsObjectRegProxy as_aabb   (helper, "AxisAlignedBox", sizeof(Ogre::AxisAlignedBox), flags); // From OGITOR

    RegisterOgreVector3(as_vector3);
    RegisterOgreRadian(as_radian);
    RegisterOgreDegree(as_degree);
    RegisterOgreQuaternion(as_quat, helper);
    RegisterColourValue(as_colour);
    RegisterSphere(as_sphere);
    RegisterAxisAlignedBox(as_aabb);

    return !helper->CheckErrors();
}


// ============================= Ogre::Vector3 ============================= //


static void Vector3DefaultConstructor(Vector3* self)
{
    new(self) Vector3();
}

static void Vector3CopyConstructor(const Vector3& other, Vector3* self)
{
    new(self) Vector3(other);
}

static void Vector3InitConstructor(float x, float y, float z, Vector3* self)
{
    new(self) Vector3(x, y, z);
}

static void Vector3InitConstructorScaler(float s, Vector3* self)
{
    new(self) Vector3(s, s, s);
}

// Returns true on success
void RegisterOgreVector3(AsObjectRegProxy& proxy)
{
    proxy.AddProperty("float x", offsetof(Ogre::Vector3, x));
    proxy.AddProperty("float y", offsetof(Ogre::Vector3, y));
    proxy.AddProperty("float z", offsetof(Ogre::Vector3, z));

    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f()",                    asFUNCTION(Vector3DefaultConstructor),    asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(Vector3InitConstructor),       asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(const Vector3 &in)",   asFUNCTION(Vector3CopyConstructor),       asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(float)",               asFUNCTION(Vector3InitConstructorScaler), asCALL_CDECL_OBJLAST);

    proxy.AddMethod("float opIndex(int) const",                     asMETHODPR(Vector3, operator[], (size_t) const, float),        asCALL_THISCALL);
    proxy.AddMethod("Vector3 &f(const Vector3 &in)",                asMETHODPR(Vector3, operator =, (const Vector3 &), Vector3&),  asCALL_THISCALL);
    proxy.AddMethod("bool opEquals(const Vector3 &in) const",       asMETHODPR(Vector3, operator==,(const Vector3&) const, bool),  asCALL_THISCALL);
    proxy.AddMethod("Vector3 opAdd(const Vector3 &in) const",       asMETHODPR(Vector3, operator+,(const Vector3&) const, Vector3),asCALL_THISCALL);
    proxy.AddMethod("Vector3 opSub(const Vector3 &in) const",       asMETHODPR(Vector3, operator-,(const Vector3&) const, Vector3),asCALL_THISCALL);
    proxy.AddMethod("Vector3 opMul(float) const",                   asMETHODPR(Vector3, operator*,(const float) const, Vector3),   asCALL_THISCALL);
    proxy.AddMethod("Vector3 opMul(const Vector3 &in) const",       asMETHODPR(Vector3, operator*,(const Vector3&) const, Vector3),asCALL_THISCALL);
    proxy.AddMethod("Vector3 opDiv(float) const",                   asMETHODPR(Vector3, operator/,(const float) const, Vector3),   asCALL_THISCALL);
    proxy.AddMethod("Vector3 opDiv(const Vector3 &in) const",       asMETHODPR(Vector3, operator/,(const Vector3&) const, Vector3),asCALL_THISCALL);
    proxy.AddMethod("Vector3 opAdd() const",                        asMETHODPR(Vector3, operator+,() const, const Vector3&),       asCALL_THISCALL);
    proxy.AddMethod("Vector3 opSub() const",                        asMETHODPR(Vector3, operator-,() const, Vector3),              asCALL_THISCALL);
    proxy.AddMethod("Vector3 &opAddAssign(const Vector3 &in)",      asMETHODPR(Vector3, operator+=,(const Vector3 &),Vector3&),    asCALL_THISCALL);
    proxy.AddMethod("Vector3 &opAddAssign(float)",                  asMETHODPR(Vector3, operator+=,(const float),Vector3&),        asCALL_THISCALL);
    proxy.AddMethod("Vector3 &opSubAssign(const Vector3 &in)",      asMETHODPR(Vector3, operator-=,(const Vector3 &),Vector3&),    asCALL_THISCALL);
    proxy.AddMethod("Vector3 &opSubAssign(float)",                  asMETHODPR(Vector3, operator-=,(const float),Vector3&),        asCALL_THISCALL);
    proxy.AddMethod("Vector3 &opMulAssign(const Vector3 &in)",      asMETHODPR(Vector3, operator*=,(const Vector3 &),Vector3&),    asCALL_THISCALL);
    proxy.AddMethod("Vector3 &opMulAssign(float)",                  asMETHODPR(Vector3, operator*=,(const float),Vector3&),        asCALL_THISCALL);
    proxy.AddMethod("Vector3 &opDivAssign(const Vector3 &in)",      asMETHODPR(Vector3, operator/=,(const Vector3 &),Vector3&),    asCALL_THISCALL);
    proxy.AddMethod("Vector3 &opDivAssign(float)",                  asMETHODPR(Vector3, operator/=,(const float),Vector3&),        asCALL_THISCALL);
    proxy.AddMethod("bool isNaN() const",                           asMETHOD(  Vector3, isNaN),                                    asCALL_THISCALL);
    proxy.AddMethod("float length() const",                         asMETHOD(  Vector3, length),                                   asCALL_THISCALL);
    proxy.AddMethod("float squaredLength() const",                  asMETHOD(  Vector3, squaredLength),                            asCALL_THISCALL);
    proxy.AddMethod("float distance(const Vector3 &in) const",      asMETHOD(  Vector3, distance),                                 asCALL_THISCALL);
    proxy.AddMethod("float dotProduct(const Vector3 &in) const",    asMETHOD(  Vector3, dotProduct),                               asCALL_THISCALL);
    proxy.AddMethod("float absDotProduct(const Vector3 &in) const", asMETHOD(  Vector3, absDotProduct),                            asCALL_THISCALL);
    proxy.AddMethod("float normalise()",                            asMETHOD(  Vector3, normalise),                                asCALL_THISCALL);
    proxy.AddMethod("float crossProduct(const Vector3 &in) const",  asMETHOD(  Vector3, crossProduct),                             asCALL_THISCALL);
    proxy.AddMethod("Vector3 midPoint(const Vector3 &in) const",    asMETHOD(  Vector3, midPoint),                                 asCALL_THISCALL);
    proxy.AddMethod("void makeFloor(const Vector3 &in)",            asMETHOD(  Vector3, makeFloor),                                asCALL_THISCALL);
    proxy.AddMethod("void makeCeil(const Vector3 &in)",             asMETHOD(  Vector3, makeCeil),                                 asCALL_THISCALL);
    proxy.AddMethod("Vector3 perpendicular() const",                asMETHOD(  Vector3, perpendicular),                            asCALL_THISCALL);
    proxy.AddMethod("Radian angleBetween(const Vector3 &in)",       asMETHOD(  Vector3, angleBetween),                             asCALL_THISCALL);
    proxy.AddMethod("bool isZeroLength() const",                    asMETHOD(  Vector3, isZeroLength),                             asCALL_THISCALL);
    proxy.AddMethod("Vector3 normalisedCopy() const",               asMETHOD(  Vector3, normalisedCopy),                           asCALL_THISCALL);
    proxy.AddMethod("Vector3 reflect(const Vector3 &in) const",     asMETHOD(  Vector3, reflect),                                  asCALL_THISCALL);
    proxy.AddMethod("bool positionEquals(const Vector3 &in, float) const",                  asMETHOD(Vector3, positionEquals),     asCALL_THISCALL);
    proxy.AddMethod("bool positionCloses(const Vector3 &in, float) const",                  asMETHOD(Vector3, positionCloses),     asCALL_THISCALL);
    proxy.AddMethod("Vector3 randomDeviant(const Radian &in, const Vector3 &in) const",     asMETHOD(Vector3, randomDeviant),      asCALL_THISCALL);
    proxy.AddMethod("bool directionEquals(const Vector3 &in, Radian &in) const",            asMETHOD(Vector3, directionEquals),    asCALL_THISCALL);
    proxy.AddMethod("Quaternion getRotationTo(const Vector3 &in, const Vector3 &in) const", asMETHOD(Vector3, getRotationTo),      asCALL_THISCALL);
    proxy.AddMethod("float squaredDistance(const Vector3 &in) const",                       asMETHOD(Vector3, squaredDistance),    asCALL_THISCALL);
}


// ============================= Ogre::Radian ============================= //


static void RadianDefaultConstructor(Radian* self)
{
    new(self) Radian();
}

static void RadianCopyConstructor(const Radian& other, Radian* self)
{
    new(self) Radian(other);
}

static void RadianInitConstructor(float r, Radian* self)
{
    new(self) Radian(r);
}

static int RadianCmp(const Radian& a, const Radian& b)
{
    if (a > b)
        return 1;
    else if (a < b)
        return -1;
    else
        return 0;
}

void RegisterOgreRadian(AsObjectRegProxy& proxy)
{
    // Register the object constructors
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f()",                 asFUNCTION(RadianDefaultConstructor), asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(float)",            asFUNCTION(RadianInitConstructor),    asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(const Radian &in)", asFUNCTION(RadianCopyConstructor),    asCALL_CDECL_OBJLAST);

    // Register other object behaviours
    proxy.AddBehavior(asBEHAVE_IMPLICIT_VALUE_CAST, "float f() const",  asMETHOD(Radian,valueRadians), asCALL_THISCALL);
    proxy.AddBehavior(asBEHAVE_IMPLICIT_VALUE_CAST, "double f() const", asMETHOD(Radian,valueRadians), asCALL_THISCALL);

    // Register the object operators
    proxy.AddMethod("Radian &opAssign(const Radian &in)",    asMETHODPR(Radian, operator= ,(const Radian &), Radian&),     asCALL_THISCALL);
    proxy.AddMethod("Radian &opAssign(const float)",         asMETHODPR(Radian, operator= ,(const float &), Radian&),      asCALL_THISCALL);
    proxy.AddMethod("Radian &opAssign(const Degree &in)",    asMETHODPR(Radian, operator= ,(const Degree &), Radian&),     asCALL_THISCALL);
    proxy.AddMethod("Radian opAdd() const",                  asMETHODPR(Radian, operator+ ,() const, const Radian&),       asCALL_THISCALL);
    proxy.AddMethod("Radian opAdd(const Radian &in) const",  asMETHODPR(Radian, operator+ ,(const Radian&) const, Radian), asCALL_THISCALL);
    proxy.AddMethod("Radian opAdd(const Degree &in) const",  asMETHODPR(Radian, operator+ ,(const Degree&) const, Radian), asCALL_THISCALL);
    proxy.AddMethod("Radian &opAddAssign(const Radian &in)", asMETHODPR(Radian, operator+=,(const Radian &),Radian&),      asCALL_THISCALL);
    proxy.AddMethod("Radian &opAddAssign(const Degree &in)", asMETHODPR(Radian, operator+=,(const Degree &),Radian&),      asCALL_THISCALL);
    proxy.AddMethod("Radian opSub() const",                  asMETHODPR(Radian, operator- ,() const, Radian),              asCALL_THISCALL);
    proxy.AddMethod("Radian opSub(const Radian &in) const",  asMETHODPR(Radian, operator- ,(const Radian&) const, Radian), asCALL_THISCALL);
    proxy.AddMethod("Radian opSub(const Degree &in) const",  asMETHODPR(Radian, operator- ,(const Degree&) const, Radian), asCALL_THISCALL);
    proxy.AddMethod("Radian &opSubAssign(const Radian &in)", asMETHODPR(Radian, operator-=,(const Radian &),Radian&),      asCALL_THISCALL);
    proxy.AddMethod("Radian &opSubAssign(const Degree &in)", asMETHODPR(Radian, operator-=,(const Degree &),Radian&),      asCALL_THISCALL);
    proxy.AddMethod("Radian opMul(float) const",             asMETHODPR(Radian, operator* ,(float) const, Radian),         asCALL_THISCALL);
    proxy.AddMethod("Radian opMul(const Radian &in) const",  asMETHODPR(Radian, operator* ,(const Radian&) const, Radian), asCALL_THISCALL);
    proxy.AddMethod("Radian &opMulAssign(float)",            asMETHODPR(Radian, operator*=,(float),Radian&),               asCALL_THISCALL);
    proxy.AddMethod("Radian opDiv(float) const",             asMETHODPR(Radian, operator/ ,(float) const, Radian),         asCALL_THISCALL);
    proxy.AddMethod("Radian &opDivAssign(float)",            asMETHODPR(Radian, operator*=,(float),Radian&),               asCALL_THISCALL);
    proxy.AddMethod("bool opEquals(const Radian &in) const", asMETHODPR(Radian, operator==,(const Radian&) const, bool),   asCALL_THISCALL);
    proxy.AddMethod("int opCmp(const Radian &in) const",     asFUNCTION(RadianCmp),                                  asCALL_CDECL_OBJFIRST);

    // Register the object methods
    proxy.AddMethod("float valueDegrees() const",     asMETHOD(Radian,valueDegrees),     asCALL_THISCALL);
    proxy.AddMethod("float valueRadians() const",     asMETHOD(Radian,valueRadians),     asCALL_THISCALL);
    proxy.AddMethod("float valueAngleUnits() const",  asMETHOD(Radian,valueAngleUnits),  asCALL_THISCALL);
}

// ============================= Ogre::Degree ============================= //

static void DegreeDefaultConstructor(Degree* self)
{
    new(self) Degree();
}

static void DegreeCopyConstructor(const Degree& other, Degree* self)
{
    new(self) Degree(other);
}

static void DegreeInitConstructor(float r, Degree* self)
{
    new(self) Degree(r);
}

static int DegreeCmp(const Degree& a, const Degree& b)
{
    if (a > b)
        return 1;
    else if (a < b)
        return -1;
    else
        return 0;
}

void RegisterOgreDegree(AsObjectRegProxy& proxy)
{
    // Register the object constructors
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f()",                 asFUNCTION(DegreeDefaultConstructor), asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(float)",            asFUNCTION(DegreeInitConstructor),    asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(const Degree &in)", asFUNCTION(DegreeCopyConstructor),    asCALL_CDECL_OBJLAST);

    // Register other object behaviours
    proxy.AddBehavior(asBEHAVE_IMPLICIT_VALUE_CAST, "float f() const",  asMETHOD(Degree,valueDegrees), asCALL_THISCALL);
    proxy.AddBehavior(asBEHAVE_IMPLICIT_VALUE_CAST, "double f() const", asMETHOD(Degree,valueDegrees), asCALL_THISCALL);

    // Register the object operators
    proxy.AddMethod("Degree &opAssign(const Degree &in)",    asMETHODPR(Degree, operator= ,(const Degree &), Degree&),     asCALL_THISCALL);
    proxy.AddMethod("Degree &opAssign(float)",               asMETHODPR(Degree, operator= ,(const float &), Degree&),      asCALL_THISCALL);
    proxy.AddMethod("Degree &opAssign(const Radian &in)",    asMETHODPR(Degree, operator= ,(const Radian &), Degree&),     asCALL_THISCALL);
    proxy.AddMethod("Degree opAdd() const",                  asMETHODPR(Degree, operator+ ,() const, const Degree&),       asCALL_THISCALL);
    proxy.AddMethod("Degree opAdd(const Degree &in) const",  asMETHODPR(Degree, operator+ ,(const Degree&) const, Degree), asCALL_THISCALL);
    proxy.AddMethod("Degree opAdd(const Radian &in) const",  asMETHODPR(Degree, operator+ ,(const Radian&) const, Degree), asCALL_THISCALL);
    proxy.AddMethod("Degree &opAddAssign(const Degree &in)", asMETHODPR(Degree, operator+=,(const Degree &),Degree&),      asCALL_THISCALL);
    proxy.AddMethod("Degree &opAddAssign(const Radian &in)", asMETHODPR(Degree, operator+=,(const Radian &),Degree&),      asCALL_THISCALL);
    proxy.AddMethod("Degree opSub() const",                  asMETHODPR(Degree, operator- ,() const, Degree),              asCALL_THISCALL);
    proxy.AddMethod("Degree opSub(const Degree &in) const",  asMETHODPR(Degree, operator- ,(const Degree&) const, Degree), asCALL_THISCALL);
    proxy.AddMethod("Degree opSub(const Radian &in) const",  asMETHODPR(Degree, operator- ,(const Radian&) const, Degree), asCALL_THISCALL);
    proxy.AddMethod("Degree &opSubAssign(const Degree &in)", asMETHODPR(Degree, operator-=,(const Degree &),Degree&),      asCALL_THISCALL);
    proxy.AddMethod("Degree &opSubAssign(const Radian &in)", asMETHODPR(Degree, operator-=,(const Radian &),Degree&),      asCALL_THISCALL);
    proxy.AddMethod("Degree opMul(float) const",             asMETHODPR(Degree, operator* ,(float) const, Degree),         asCALL_THISCALL);
    proxy.AddMethod("Degree opMul(const Degree &in) const",  asMETHODPR(Degree, operator* ,(const Degree&) const, Degree), asCALL_THISCALL);
    proxy.AddMethod("Degree &opMulAssign(float)",            asMETHODPR(Degree, operator*=,(float),Degree&),               asCALL_THISCALL);
    proxy.AddMethod("Degree opDiv(float) const",             asMETHODPR(Degree, operator/ ,(float) const, Degree),         asCALL_THISCALL);
    proxy.AddMethod("Degree &opDivAssign(float)",            asMETHODPR(Degree, operator*=,(float),Degree&),               asCALL_THISCALL);
    proxy.AddMethod("bool opEquals(const Degree &in) const", asMETHODPR(Degree, operator==,(const Degree&) const, bool),   asCALL_THISCALL);
    proxy.AddMethod("int opCmp(const Degree &in) const",     asFUNCTION(DegreeCmp),                                  asCALL_CDECL_OBJFIRST);

    // Register the object methods
    proxy.AddMethod("float valueRadians() const",    asMETHOD(Degree,valueRadians),    asCALL_THISCALL);
    proxy.AddMethod("float valueDegrees() const",    asMETHOD(Degree,valueDegrees),    asCALL_THISCALL);
    proxy.AddMethod("float valueAngleUnits() const", asMETHOD(Degree,valueAngleUnits), asCALL_THISCALL);

}

// ============================= Ogre::Quaternion ============================= //

static void QuaternionDefaultConstructor(Quaternion* self)
{
    new(self) Quaternion();
}

static void QuaternionCopyConstructor(const Quaternion& other, Quaternion* self)
{
    new(self) Quaternion(other.w, other.x, other.y, other.z);
}

static void QuaternionInitConstructor1(const Radian& rfAngle, const Vector3& rkAxis, Quaternion* self)
{
    new(self) Quaternion(rfAngle, rkAxis);
}

static void QuaternionInitConstructor2(float w, float x, float y, float z, Quaternion* self)
{
    new(self) Quaternion(w, x, y, z);
}

static void QuaternionInitConstructor3(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis, Quaternion* self)
{
    new(self) Quaternion(xaxis, yaxis, zaxis);
}

static void QuaternionInitConstructorScaler(float s, Quaternion* self)
{
    new(self) Quaternion(s, s, s, s);
}

void RegisterOgreQuaternion(AsObjectRegProxy& proxy, AsSetupHelper* helper)
{
    // Register the object properties
    proxy.AddProperty("float w", offsetof(Quaternion, w));
    proxy.AddProperty("float x", offsetof(Quaternion, x));
    proxy.AddProperty("float y", offsetof(Quaternion, y));
    proxy.AddProperty("float z", offsetof(Quaternion, z));

    // Register the object constructors
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f()",                                                        asFUNCTION(QuaternionDefaultConstructor),    asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(const Radian &in, const Vector3 &in)",                     asFUNCTION(QuaternionInitConstructor1),      asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(float, float, float, float)",                              asFUNCTION(QuaternionInitConstructor2),      asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(const Vector3 &in, const Vector3 &in, const Vector3 &in)", asFUNCTION(QuaternionInitConstructor3),      asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(float)",                                                   asFUNCTION(QuaternionInitConstructorScaler), asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f(const Quaternion &in)",                                    asFUNCTION(QuaternionCopyConstructor),       asCALL_CDECL_OBJLAST);

    // Register the object operators
    proxy.AddMethod("float opIndex(int) const",                     asMETHODPR(Quaternion, operator[], (size_t)            const,      float), asCALL_THISCALL);
    proxy.AddMethod("Quaternion &opAssign(const Quaternion &in)",   asMETHODPR(Quaternion, operator =, (const Quaternion &),     Quaternion&), asCALL_THISCALL);
    proxy.AddMethod("Quaternion opAdd(const Quaternion &in) const", asMETHODPR(Quaternion, operator+ , (const Quaternion&) const, Quaternion), asCALL_THISCALL);
    proxy.AddMethod("Quaternion opSub(const Quaternion &in) const", asMETHODPR(Quaternion, operator- , (const Quaternion&) const, Quaternion), asCALL_THISCALL);
    proxy.AddMethod("Quaternion opMul(const Quaternion &in) const", asMETHODPR(Quaternion, operator* , (const Quaternion&) const, Quaternion), asCALL_THISCALL);
    proxy.AddMethod("Quaternion opMul(float) const",                asMETHODPR(Quaternion, operator* , (float)             const, Quaternion), asCALL_THISCALL);
    proxy.AddMethod("Quaternion opSub() const",                     asMETHODPR(Quaternion, operator- , ()                  const, Quaternion), asCALL_THISCALL);
    proxy.AddMethod("bool opEquals(const Quaternion &in) const",    asMETHODPR(Quaternion, operator==, (const Quaternion&) const,       bool), asCALL_THISCALL);
    proxy.AddMethod("Vector3 opMul(const Vector3 &in) const",       asMETHODPR(Quaternion, operator* , (const Vector3&)    const,    Vector3), asCALL_THISCALL);

    // Register the object methods
    proxy.AddMethod("float Dot(const Quaternion &in) const", asMETHOD(Quaternion,Dot),          asCALL_THISCALL);
    proxy.AddMethod("float Norm() const",                    asMETHOD(Quaternion,Norm),         asCALL_THISCALL);
    proxy.AddMethod("float normalise()",                     asMETHOD(Quaternion,normalise),    asCALL_THISCALL);
    proxy.AddMethod("Quaternion Inverse() const",            asMETHOD(Quaternion,Inverse),      asCALL_THISCALL);
    proxy.AddMethod("Quaternion UnitInverse() const",        asMETHOD(Quaternion,UnitInverse),  asCALL_THISCALL);
    proxy.AddMethod("Quaternion Exp() const",                asMETHOD(Quaternion,Exp),          asCALL_THISCALL);
    proxy.AddMethod("Quaternion Log() const",                asMETHOD(Quaternion,Log),          asCALL_THISCALL);
    proxy.AddMethod("Radian getRoll(bool) const",            asMETHOD(Quaternion,getRoll),      asCALL_THISCALL);
    proxy.AddMethod("Radian getPitch(bool) const",           asMETHOD(Quaternion,getPitch),     asCALL_THISCALL);
    proxy.AddMethod("Radian getYaw(bool) const",             asMETHOD(Quaternion,getYaw),       asCALL_THISCALL);
    proxy.AddMethod("bool equals(const Quaternion &in, const Radian &in) const", asMETHOD(Quaternion,equals), asCALL_THISCALL);
    proxy.AddMethod("bool isNaN() const",                    asMETHOD(Quaternion,isNaN),                      asCALL_THISCALL);

    // Register some static methods
    helper->RegisterGlobalFn("Quaternion Slerp(float, const Quaternion &in, const Quaternion &in, bool &in)", asFUNCTIONPR(Quaternion::Slerp,(Real fT, const Quaternion&, const Quaternion&, bool), Quaternion), asCALL_CDECL);
    helper->RegisterGlobalFn("Quaternion SlerpExtraSpins(float, const Quaternion &in, const Quaternion &in, int &in)", asFUNCTION(Quaternion::SlerpExtraSpins), asCALL_CDECL);
    helper->RegisterGlobalFn("void Intermediate(const Quaternion &in, const Quaternion &in, const Quaternion &in, const Quaternion &in, const Quaternion &in)", asFUNCTION(Quaternion::Intermediate), asCALL_CDECL);
    helper->RegisterGlobalFn("Quaternion Squad(float, const Quaternion &in, const Quaternion &in, const Quaternion &in, const Quaternion &in, bool &in)", asFUNCTION(Quaternion::Squad), asCALL_CDECL);
    helper->RegisterGlobalFn("Quaternion nlerp(float, const Quaternion &in, const Quaternion &in, bool &in)", asFUNCTION(Quaternion::nlerp), asCALL_CDECL);

}

// ============================= Ogre::ColourValue ============================= //
// Source:
//  project OGITOR (https://bitbucket.org/ogitor/ogitor)
//  file Plugins/OgAngelScript/Bindings_Ogre.cpp

static void ColourValueDefaultConstructor(Ogre::ColourValue *self)
{
    new(self) Ogre::ColourValue();
}

static void ColourValueDefaultConstructor2(float r, float g, float b, float a, Ogre::ColourValue *self)
{
    new(self) Ogre::ColourValue(r, g, b, a);
}

static void CopyConstructColourValue(const Ogre::ColourValue &other, Ogre::ColourValue *thisPointer)
{
    new(thisPointer) Ogre::ColourValue(other);
}

static void DestructColourValue(Ogre::ColourValue *thisPointer)
{
    thisPointer->~ColourValue();
}

static Ogre::ColourValue &ColourValueAssignment(Ogre::ColourValue *other, Ogre::ColourValue *self)
{
    return *self = *other;
}

void RegisterColourValue(AsObjectRegProxy& proxy)
{
    proxy.AddBehavior(asBEHAVE_CONSTRUCT,  "void f()",                           asFUNCTION(ColourValueDefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT,  "void f(float, float, float, float)", asFUNCTION(ColourValueDefaultConstructor2),	asCALL_CDECL_OBJLAST); assert(r >= 0);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT,  "void f(const ColourValue &in)",      asFUNCTION(CopyConstructColourValue), asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_DESTRUCT,   "void f()",                           asFUNCTION(DestructColourValue),  asCALL_CDECL_OBJLAST);

    proxy.AddProperty("float r", offsetof(Ogre::ColourValue, r));
    proxy.AddProperty("float g", offsetof(Ogre::ColourValue, g));
    proxy.AddProperty("float b", offsetof(Ogre::ColourValue, b));
    proxy.AddProperty("float a", offsetof(Ogre::ColourValue, a));

    proxy.AddMethod("ColourValue &opAssign(ColourValue&in)", asFUNCTION(ColourValueAssignment), asCALL_CDECL_OBJLAST);
}

// ============================= Ogre::Sphere ============================= //
// Source:
//  project OGITOR (https://bitbucket.org/ogitor/ogitor)
//  file Plugins/OgAngelScript/Bindings_Ogre.cpp

static void SphereDefaultConstructor(const Ogre::Vector3 origin, Ogre::Real radius, Ogre::AxisAlignedBox *self)
{
    new(self) Ogre::Sphere(origin, radius);
}

static void CopyConstructSphere(const Ogre::Sphere &other, Ogre::Sphere *thisPointer)
{
    new(thisPointer) Ogre::Sphere(other);
}

static void DestructSphere(Ogre::Sphere *thisPointer)
{
    thisPointer->~Sphere();
}

static Ogre::Sphere &SphereAssignment(Ogre::Sphere *other, Ogre::Sphere *self)
{
    return *self = *other;
}

void RegisterSphere(AsObjectRegProxy& proxy)
{
    proxy.AddBehavior(asBEHAVE_CONSTRUCT,  "void f(const Vector3 &in, float)",  asFUNCTION(SphereDefaultConstructor),  asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT,  "void f(const Sphere &in)",          asFUNCTION(CopyConstructSphere),       asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_DESTRUCT,   "void f()",                          asFUNCTION(DestructSphere),            asCALL_CDECL_OBJLAST);

    proxy.AddMethod("Sphere &opAssign(Sphere&in)", asFUNCTION(SphereAssignment), asCALL_CDECL_OBJLAST);
}

// ============================= Ogre::AxisAlignedBox ============================= //
// Source:
//  project OGITOR (https://bitbucket.org/ogitor/ogitor)
//  file Plugins/OgAngelScript/Bindings_Ogre.cpp

static void AxisAlignedBoxDefaultConstructor(Ogre::AxisAlignedBox *self)
{
    new(self) Ogre::AxisAlignedBox();
}

static void AxisAlignedBoxDefaultConstructor2(const Ogre::Vector3 min, const Ogre::Vector3 max, Ogre::AxisAlignedBox *self)
{
    new(self) Ogre::AxisAlignedBox(min, max);
}

static void CopyConstructAxisAlignedBox(const Ogre::AxisAlignedBox &other, Ogre::AxisAlignedBox *thisPointer)
{
    new(thisPointer) Ogre::AxisAlignedBox(other);
}

static void DestructAxisAlignedBox(Ogre::AxisAlignedBox *thisPointer)
{
    thisPointer->~AxisAlignedBox();
}

static Ogre::AxisAlignedBox &AxisAlignedBoxAssignment(Ogre::AxisAlignedBox *other, Ogre::AxisAlignedBox *self)
{
    return *self = *other;
}

void RegisterAxisAlignedBox(AsObjectRegProxy& proxy)
{
    proxy.AddBehavior(asBEHAVE_CONSTRUCT,  "void f()",                                     asFUNCTION(AxisAlignedBoxDefaultConstructor),    asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT,  "void f(const Vector3 &in, const Vector3 &in)", asFUNCTION(AxisAlignedBoxDefaultConstructor2),   asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_CONSTRUCT,  "void f(const AxisAlignedBox &in)",             asFUNCTION(CopyConstructAxisAlignedBox),         asCALL_CDECL_OBJLAST);
    proxy.AddBehavior(asBEHAVE_DESTRUCT,   "void f()",                                     asFUNCTION(DestructAxisAlignedBox),              asCALL_CDECL_OBJLAST);

    proxy.AddMethod("const Vector3& getMinimum()",           asMETHODPR(AxisAlignedBox, getMinimum, (void) const, const Ogre::Vector3&),      asCALL_THISCALL);
    proxy.AddMethod("const Vector3& getMaximum()",           asMETHODPR(AxisAlignedBox, getMaximum, (void) const, const Ogre::Vector3&),      asCALL_THISCALL);
    proxy.AddMethod("void setMinimum(const Vector3 &in)",    asMETHODPR(AxisAlignedBox, setMinimum, (const Ogre::Vector3&), void),            asCALL_THISCALL);
    proxy.AddMethod("void setMinimum(float, float, float)",  asMETHODPR(AxisAlignedBox, setMinimum, (float, float, float), void),             asCALL_THISCALL);
    proxy.AddMethod("void setMaximum(const Vector3 &in)",    asMETHODPR(AxisAlignedBox, setMaximum, (const Ogre::Vector3&), void),            asCALL_THISCALL);
    proxy.AddMethod("void setMaximum(float, float, float)",  asMETHODPR(AxisAlignedBox, setMaximum, (float, float, float), void),             asCALL_THISCALL);
    proxy.AddMethod("void merge(const AxisAlignedBox &in)",  asMETHODPR(AxisAlignedBox, merge,      (const Ogre::AxisAlignedBox&), void),     asCALL_THISCALL);
    proxy.AddMethod("void merge(const Vector3 &in)",         asMETHODPR(AxisAlignedBox, merge,      (const Ogre::Vector3&), void),            asCALL_THISCALL);
    proxy.AddMethod("void setNull()",                        asMETHOD(  AxisAlignedBox, setNull),                                             asCALL_THISCALL);
    proxy.AddMethod("bool isNull()",                         asMETHOD(  AxisAlignedBox, isNull),                                              asCALL_THISCALL);
    proxy.AddMethod("bool isFinite()",                       asMETHOD(  AxisAlignedBox, isFinite),                                            asCALL_THISCALL);
    proxy.AddMethod("bool isInfinite()",                     asMETHOD(  AxisAlignedBox, isInfinite),                                          asCALL_THISCALL);
    proxy.AddMethod("void setInfinite()",                    asMETHOD(  AxisAlignedBox, setInfinite),                                         asCALL_THISCALL);
    proxy.AddMethod("bool intersects(const Sphere &in)",     asMETHODPR(AxisAlignedBox, intersects, (const Ogre::Sphere&) const, bool),       asCALL_THISCALL);
    proxy.AddMethod("bool intersects(const Vector3 &in)",    asMETHODPR(AxisAlignedBox, intersects, (const Ogre::Vector3&) const, bool),      asCALL_THISCALL);
    proxy.AddMethod("float volume()",                        asMETHOD(  AxisAlignedBox, volume),                                              asCALL_THISCALL);
    proxy.AddMethod("void scale(const Vector3 &in)",         asMETHOD(  AxisAlignedBox, scale),                                               asCALL_THISCALL);
    proxy.AddMethod("Vector3 getCenter()",                   asMETHOD(  AxisAlignedBox, getCenter),                                           asCALL_THISCALL);
    proxy.AddMethod("Vector3 getSize()",                     asMETHOD(  AxisAlignedBox, getSize),                                             asCALL_THISCALL);
    proxy.AddMethod("Vector3 getHalfSize()",                 asMETHOD(  AxisAlignedBox, getHalfSize),                                         asCALL_THISCALL);
    proxy.AddMethod("bool contains(const Vector3 &in)",      asMETHODPR(AxisAlignedBox, contains, (const Ogre::Vector3&) const, bool),        asCALL_THISCALL);
    proxy.AddMethod("bool contains(const AxisAlignedBox &in)",               asMETHODPR(AxisAlignedBox, contains, (const Ogre::AxisAlignedBox&) const, bool), asCALL_THISCALL);
    proxy.AddMethod("bool intersects(const AxisAlignedBox &in)",             asMETHODPR(AxisAlignedBox, intersects,(const Ogre::AxisAlignedBox&) const, bool),asCALL_THISCALL);
    proxy.AddMethod("AxisAlignedBox intersection(const AxisAlignedBox &in)", asMETHOD(  AxisAlignedBox, intersection), asCALL_THISCALL);
    proxy.AddMethod("AxisAlignedBox &opAssign(AxisAlignedBox&in)",           asFUNCTION(AxisAlignedBoxAssignment),                                             asCALL_CDECL_OBJLAST);
}

