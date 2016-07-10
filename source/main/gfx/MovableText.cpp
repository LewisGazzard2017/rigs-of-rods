/*
This source file is part of Rigs of Rods
Copyright 2005-2012 Pierre-Michel Ricordel
Copyright 2007-2012 Thomas Fischer

For more information, see http://www.rigsofrods.com/

Rigs of Rods is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3, as
published by the Free Software Foundation.

Rigs of Rods is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Rigs of Rods.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
* File: MovableText.cpp
*
* description: This create create a billboarding object that display a text.
*
* @author  2003 by cTh see gavocanov@rambler.ru
* @update  2006 by barraq see nospam@barraquand.com
*/

#include "MovableText.h"

#include <Ogre.h>
#include <OgreFontManager.h>

using namespace Ogre;

#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1

MovableText::MovableText(Ogre::IdType id, ObjectMemoryManager *objectMemoryManager, Ogre::SceneManager* ogreSceneMgr, const NameValuePairList* params)
: MovableObject(id, objectMemoryManager, ogreSceneMgr, 1)
, mpCam(NULL)
, mpWin(NULL)
, mpFont(NULL)
, mName("")
, mCaption("")
, mFontName("BlueHighway-8")
, mCharHeight(1.0f)
, mColor(ColourValue::White)
, mType("MovableText")
, mTimeUntilNextToggle(0)
, mSpaceWidth(0)
, mUpdateColors(true)
, mOnTop(false)
, mHorizontalAlignment(H_LEFT)
, mVerticalAlignment(V_BELOW)
, mAdditionalHeight(0.0)
{
	if (params != 0)
	{
		NameValuePairList::const_iterator ni;

		ni = params->find("name");
		if (ni != params->end())
		{
			mName = ni->second;
		}

		ni = params->find("caption");
		if (ni != params->end())
		{
			mCaption = ni->second;
		}

		ni = params->find("fontName");
		if (ni != params->end())
		{
			mFontName = ni->second;
		}
	}

	assert(mName != "" && "Trying to create MovableText without name");

	mRenderOp.vertexData = NULL;
	this->setFontName(mFontName);
	this->_setupGeometry();
	Root::getSingletonPtr()->addFrameListener(this);
}

MovableText::~MovableText()
{
	if (mRenderOp.vertexData)
		delete mRenderOp.vertexData;
}

void MovableText::setFontName(const UTFString &fontName)
{
	if ((Ogre::MaterialManager::getSingletonPtr()->resourceExists(mName + "Material")))
	{
		Ogre::MaterialManager::getSingleton().remove(mName + "Material");
	}

	if (mFontName != fontName || mpMaterial.isNull() || !mpFont)
	{
		mFontName = fontName;
#ifdef ROR_USE_OGRE_1_9
		mpFont = (Font *)FontManager::getSingleton().getResourceByName(mFontName).getPointer();
#else
		mpFont = (Font *)FontManager::getSingleton().getByName(mFontName).getPointer();
#endif
		if (!mpFont)
			throw Exception(Exception::ERR_ITEM_NOT_FOUND, "Could not find font " + fontName, "MovableText::setFontName");

		mpFont->load();
		if (!mpMaterial.isNull())
		{
			MaterialManager::getSingletonPtr()->remove(mpMaterial->getName());
			mpMaterial.setNull();
		}

		mpMaterial = mpFont->getMaterial()->clone(mName + "Material");
		if (!mpMaterial->isLoaded())
			mpMaterial->load();

		/* FIXME ogre21
        mpMaterial->setDepthCheckEnabled(!mOnTop);
		mpMaterial->setDepthBias(1.0,1.0);
        */
		mpMaterial->setFog(true);
        /* FIXME ogre21
		mpMaterial->setDepthWriteEnabled(mOnTop);
		mpMaterial->setLightingEnabled(false);
        */
		mNeedUpdate = true;
	}
}

void MovableText::setCaption(const UTFString &caption)
{
	if (caption != mCaption)
	{
		mCaption = caption;
		mNeedUpdate = true;
	}
}

void MovableText::setColor(const ColourValue &color)
{
	if (color != mColor)
	{
		mColor = color;
		mUpdateColors = true;
	}
}

void MovableText::setCharacterHeight(Real height)
{
	if (fabs(height - mCharHeight) > 0.00001f)
	{
		mCharHeight = height;
		mNeedUpdate = true;
	}
}

void MovableText::setSpaceWidth(Real width)
{
	if (fabs(width - mSpaceWidth) > 0.00001f)
	{
		mSpaceWidth = width;
		mNeedUpdate = true;
	}
}

void MovableText::setTextAlignment(const HorizontalAlignment& horizontalAlignment, const VerticalAlignment& verticalAlignment)
{
	if (mHorizontalAlignment != horizontalAlignment)
	{
		mHorizontalAlignment = horizontalAlignment;
		mNeedUpdate = true;
	}
	if (mVerticalAlignment != verticalAlignment)
	{
		mVerticalAlignment = verticalAlignment;
		mNeedUpdate = true;
	}
}

void MovableText::setAdditionalHeight( Real height )
{
	if (fabs(mAdditionalHeight - height) > 0.00001f)
	{
		mAdditionalHeight = height;
		mNeedUpdate = true;
	}
}

void MovableText::showOnTop(bool show)
{
	if ( mOnTop != show && !mpMaterial.isNull() )
	{
		mOnTop = show;
        /* FIXME ogre21
		mpMaterial->setDepthBias(1.0,1.0);
		mpMaterial->setDepthCheckEnabled(!mOnTop);
		mpMaterial->setDepthWriteEnabled(mOnTop);
        */
	}
}

void MovableText::_setupGeometry()
{
	assert(mpFont);
	assert(!mpMaterial.isNull());

	uint vertexCount = static_cast<uint>(mCaption.size() * 6);

	if (mRenderOp.vertexData)
	{
		// Removed this test as it causes problems when replacing a caption
		// of the same size: replacing "Hello" with "hello"
		// as well as when changing the text alignment
		//if (mRenderOp.vertexData->vertexCount != vertexCount)
		{
			delete mRenderOp.vertexData;
			mRenderOp.vertexData = NULL;
			mUpdateColors = true;
		}
	}

	if (!mRenderOp.vertexData)
		mRenderOp.vertexData = new Ogre::v1::VertexData();

	mRenderOp.indexData = 0;
	mRenderOp.vertexData->vertexStart = 0;
	mRenderOp.vertexData->vertexCount = vertexCount;
	mRenderOp.operationType = Ogre::v1::RenderOperation::OT_TRIANGLE_LIST;
	mRenderOp.useIndexes = false;

    Ogre::v1::VertexDeclaration  *decl = mRenderOp.vertexData->vertexDeclaration;
    Ogre::v1::VertexBufferBinding   *bind = mRenderOp.vertexData->vertexBufferBinding;
	size_t offset = 0;

	// create/bind positions/tex.ccord. buffer
	if (!decl->findElementBySemantic(VES_POSITION))
		decl->addElement(POS_TEX_BINDING, offset, VET_FLOAT3, VES_POSITION);

	offset += Ogre::v1::VertexElement::getTypeSize(VET_FLOAT3);

	if (!decl->findElementBySemantic(VES_TEXTURE_COORDINATES))
		decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

	v1::HardwareVertexBufferSharedPtr ptbuf = Ogre::v1::HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(POS_TEX_BINDING),
		mRenderOp.vertexData->vertexCount,
        Ogre::v1::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	bind->setBinding(POS_TEX_BINDING, ptbuf);

	// Colours - store these in a separate buffer because they change less often
	if (!decl->findElementBySemantic(VES_DIFFUSE))
		decl->addElement(COLOUR_BINDING, 0, VET_COLOUR, VES_DIFFUSE);

	v1::HardwareVertexBufferSharedPtr cbuf = Ogre::v1::HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(COLOUR_BINDING),
		mRenderOp.vertexData->vertexCount,
        Ogre::v1::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	bind->setBinding(COLOUR_BINDING, cbuf);

	//Real *pPCBuff = static_cast<Real*>(ptbuf->lock(HardwareBuffer::HBL_NORMAL));
	Real *pPCBuff=(Real*)malloc(ptbuf->getSizeInBytes());
	Real *oPCBuff=pPCBuff;

	float largestWidth = 0;
	float left = 0 * 2.0 - 1.0;
	float top = -((0 * 2.0) - 1.0);

	// Derive space width from a capital A
	if (fabs(mSpaceWidth) < 0.00001f)
		mSpaceWidth = mpFont->getGlyphAspectRatio('A') * mCharHeight * 2.0;

	// for calculation of AABB
	Ogre::Vector3 min=Ogre::Vector3::ZERO, max=Ogre::Vector3::ZERO, currPos=Ogre::Vector3::ZERO;
	Ogre::Real maxSquaredRadius = 0.0f;
	bool first = true;

	// Use iterator
	UTFString::iterator i, iend;
	iend = mCaption.end();
	bool newLine = true;
	Real len = 0.0f;

	if (mVerticalAlignment == MovableText::V_ABOVE)
	{
		// Raise the first line of the caption
		top += mCharHeight;
		for (i = mCaption.begin(); i != iend; ++i)
		{
			if (*i == '\n')
				top += mCharHeight * 2.0;
		}
	}

	for (i = mCaption.begin(); i != iend; ++i)
	{
		if (newLine)
		{
			len = 0.0f;
			for (UTFString::iterator j = i; j != iend && *j != '\n'; j++)
			{
				if (*j == ' ')
					len += mSpaceWidth;
				else
					len += mpFont->getGlyphAspectRatio(*j) * mCharHeight * 2.0;
			}
			newLine = false;
		}

		if (*i == '\n')
		{
			left = 0 * 2.0 - 1.0;
			top -= mCharHeight * 2.0;
			newLine = true;
			continue;
		}

		if (*i == ' ')
		{
			// Just leave a gap, no tris
			left += mSpaceWidth;
			// Also reduce tri count
			mRenderOp.vertexData->vertexCount -= 6;
			continue;
		}

		Real horiz_height = mpFont->getGlyphAspectRatio(*i);
		Real u1, u2, v1, v2;
		Ogre::Font::UVRect utmp;
		utmp = mpFont->getGlyphTexCoords(*i);
		u1 = utmp.left;
		u2 = utmp.right;
		v1 = utmp.top;
		v2 = utmp.bottom;

		// each vert is (x, y, z, u, v)
		//-------------------------------------------------------------------------------------
		// First tri
		//
		// Upper left
		if (mHorizontalAlignment == MovableText::H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u1;
		*pPCBuff++ = v1;

		// Deal with bounds
		if (mHorizontalAlignment == MovableText::H_LEFT)
			currPos = Ogre::Vector3(left, top, -1.0);
		else
			currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
		if (first)
		{
			min = max = currPos;
			maxSquaredRadius = currPos.squaredLength();
			first = false;
		}
		else
		{
			min.makeFloor(currPos);
			max.makeCeil(currPos);
			maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
		}

		top -= mCharHeight * 2.0;

		// Bottom left
		if (mHorizontalAlignment == MovableText::H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u1;
		*pPCBuff++ = v2;

		// Deal with bounds
		if (mHorizontalAlignment == MovableText::H_LEFT)
			currPos = Ogre::Vector3(left, top, -1.0);
		else
			currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
		min.makeFloor(currPos);
		max.makeCeil(currPos);
		maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

		top += mCharHeight * 2.0;
		left += horiz_height * mCharHeight * 2.0;

		// Top right
		if (mHorizontalAlignment == MovableText::H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u2;
		*pPCBuff++ = v1;
		//-------------------------------------------------------------------------------------

		// Deal with bounds
		if (mHorizontalAlignment == MovableText::H_LEFT)
			currPos = Ogre::Vector3(left, top, -1.0);
		else
			currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
		min.makeFloor(currPos);
		max.makeCeil(currPos);
		maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

		//-------------------------------------------------------------------------------------
		// Second tri
		//
		// Top right (again)
		if (mHorizontalAlignment == MovableText::H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u2;
		*pPCBuff++ = v1;

		currPos = Ogre::Vector3(left, top, -1.0);
		min.makeFloor(currPos);
		max.makeCeil(currPos);
		maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

		top -= mCharHeight * 2.0;
		left -= horiz_height  * mCharHeight * 2.0;

		// Bottom left (again)
		if (mHorizontalAlignment == MovableText::H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u1;
		*pPCBuff++ = v2;

		currPos = Ogre::Vector3(left, top, -1.0);
		min.makeFloor(currPos);
		max.makeCeil(currPos);
		maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

		left += horiz_height  * mCharHeight * 2.0;

		// Bottom right
		if (mHorizontalAlignment == MovableText::H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u2;
		*pPCBuff++ = v2;
		//-------------------------------------------------------------------------------------

		currPos = Ogre::Vector3(left, top, -1.0);
		min.makeFloor(currPos);
		max.makeCeil(currPos);
		maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

		// Go back up with top
		top += mCharHeight * 2.0;

		float currentWidth = (left + 1)/2 - 0;
		if (currentWidth > largestWidth)
			largestWidth = currentWidth;
	}

	// Unlock vertex buffer
	//ptbuf->unlock();
	ptbuf->writeData(0, ptbuf->getSizeInBytes(), oPCBuff, true);
	free(oPCBuff);


	// update AABB/Sphere radius
	mAABB = Ogre::AxisAlignedBox(min, max);
	mRadius = Ogre::Math::Sqrt(maxSquaredRadius);

	if (mUpdateColors)
		this->_updateColors();

	mNeedUpdate = false;
}

void MovableText::_updateColors(void)
{
	assert(mpFont);
	assert(!mpMaterial.isNull());

	// Convert to system-specific
	RGBA color;
	Root::getSingleton().convertColourValue(mColor, &color);
	v1::HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);
	//RGBA *pDest = static_cast<RGBA*>(vbuf->lock(HardwareBuffer::HBL_NORMAL));
	RGBA* pDest=(RGBA*)malloc(vbuf->getSizeInBytes());
	RGBA* oDest=pDest;
	for (uint i = 0; i < mRenderOp.vertexData->vertexCount; ++i)
		*pDest++ = color;
	//vbuf->unlock();
	vbuf->writeData(0, vbuf->getSizeInBytes(), oDest, true);
	free(oDest);
	mUpdateColors = false;
}

const Quaternion& MovableText::getWorldOrientation(void) const
{
	assert(mpCam);
	return const_cast<Quaternion&>(mpCam->getDerivedOrientation());
}

// Add to build on Shoggoth:
#if OGRE_VERSION>0x010602
void MovableText::visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables) {};
#endif //OGRE_VERSION


const Vector3& MovableText::getWorldPosition(void) const
{
	assert(mParentNode);
	return mParentNode->_getDerivedPosition();
}

void MovableText::getWorldTransforms(Matrix4 *xform) const
{
	if (this->isVisible() && mpCam)
	{
		Matrix3 rot3x3, scale3x3 = Matrix3::IDENTITY;

		// store rotation in a matrix
		mpCam->getDerivedOrientation().ToRotationMatrix(rot3x3);

		// parent node position
		Vector3 ppos = mParentNode->_getDerivedPosition() + Vector3::UNIT_Y*mAdditionalHeight;

		// apply scale
		scale3x3[0][0] = mParentNode->_getDerivedScale().x / 2;
		scale3x3[1][1] = mParentNode->_getDerivedScale().y / 2;
		scale3x3[2][2] = mParentNode->_getDerivedScale().z / 2;

		// apply all transforms to xform
		*xform = (rot3x3 * scale3x3);
		xform->setTrans(ppos);
	}
}

void MovableText::getRenderOperation(Ogre::v1::RenderOperation &op, bool caster_pass)
{
	if (this->isVisible())
	{
		if (mNeedUpdate)
			this->_setupGeometry();
		if (mUpdateColors)
			this->_updateColors();
		op = mRenderOp;
	}
}

void MovableText::_notifyCurrentCamera(Camera *cam)
{
	mpCam = cam;
}

void MovableText::_updateRenderQueue(RenderQueue* queue, Camera *camera, const Camera *lodCamera)
{
	if (this->isVisible())
	{
		if (mNeedUpdate)
			this->_setupGeometry();
		if (mUpdateColors)
			this->_updateColors();

        // FIXME: ogre21
		queue->addRenderableV1(
            mRenderQueueID,
            false, // caster pass? bool
            this,
            nullptr // MovableObject* -> this will probably crash
        ); 
		//      queue->addRenderable(this, mRenderQueueID, RENDER_QUEUE_SKIES_LATE);
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
String MovableTextFactory::FACTORY_TYPE_NAME = "MovableText";
//-----------------------------------------------------------------------
const String& MovableTextFactory::getType(void) const
{
	return FACTORY_TYPE_NAME;
}
//-----------------------------------------------------------------------
MovableObject* MovableTextFactory::createInstanceImpl(IdType id,
	ObjectMemoryManager *objectMemoryManager,
    Ogre::SceneManager* ogreSceneMgr,
	const NameValuePairList* params)
{
	return OGRE_NEW MovableText(id, objectMemoryManager, ogreSceneMgr, params);
}
//-----------------------------------------------------------------------
void MovableTextFactory::destroyInstance(MovableObject* obj)
{
	OGRE_DELETE obj;
}
