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

#ifndef __SkyXManager_H_
#define __SkyXManager_H_

#include "RoRPrerequisites.h"

//#include "CaelumPrerequisites.h"
#include "IManager.h"
#include <SkyX.h>

struct SkyXSettings
{
	/** Constructor
	    @remarks Skydome + vol. clouds + lightning settings
	 */
	SkyXSettings(const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt,
		const bool& lc, const bool& vc, const Ogre::Real& vcws, const bool& vcauto, const Ogre::Radian& vcwd, 
		const Ogre::Vector3& vcac, const Ogre::Vector4& vclr,  const Ogre::Vector4& vcaf, const Ogre::Vector2& vcw,
		const bool& vcl, const Ogre::Real& vclat, const Ogre::Vector3& vclc, const Ogre::Real& vcltm)
		: time(t), timeMultiplier(tm), moonPhase(mp), atmosphereOpt(atmOpt), layeredClouds(lc), volumetricClouds(vc), vcWindSpeed(vcws)
		, vcAutoupdate(vcauto), vcWindDir(vcwd), vcAmbientColor(vcac), vcLightResponse(vclr), vcAmbientFactors(vcaf), vcWheater(vcw)
		, vcLightnings(vcl), vcLightningsAT(vclat), vcLightningsColor(vclc), vcLightningsTM(vcltm)
	{}

	/** Constructor
	    @remarks Skydome + vol. clouds
	 */
	SkyXSettings(const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt,
		const bool& lc, const bool& vc, const Ogre::Real& vcws, const bool& vcauto, const Ogre::Radian& vcwd, 
		const Ogre::Vector3& vcac, const Ogre::Vector4& vclr,  const Ogre::Vector4& vcaf, const Ogre::Vector2& vcw)
		: time(t), timeMultiplier(tm), moonPhase(mp), atmosphereOpt(atmOpt), layeredClouds(lc), volumetricClouds(vc), vcWindSpeed(vcws)
		, vcAutoupdate(vcauto), vcWindDir(vcwd), vcAmbientColor(vcac), vcLightResponse(vclr), vcAmbientFactors(vcaf), vcWheater(vcw), vcLightnings(false)
	{}

	/** Constructor
	    @remarks Skydome settings
	 */
	SkyXSettings(const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt, const bool& lc)
		: time(t), timeMultiplier(tm), moonPhase(mp), atmosphereOpt(atmOpt), layeredClouds(lc), volumetricClouds(false), vcLightnings(false)
	{}

	/// Time
	Ogre::Vector3 time;
	/// Time multiplier
	Ogre::Real timeMultiplier;
	/// Moon phase
	Ogre::Real moonPhase;
	/// Atmosphere options
	SkyX::AtmosphereManager::Options atmosphereOpt;
	/// Layered clouds?
	bool layeredClouds;
	/// Volumetric clouds?
	bool volumetricClouds;
	/// VClouds wind speed
	Ogre::Real vcWindSpeed;
	/// VClouds autoupdate
	bool vcAutoupdate;
	/// VClouds wind direction
	Ogre::Radian vcWindDir;
	/// VClouds ambient color
	Ogre::Vector3 vcAmbientColor;
	/// VClouds light response
	Ogre::Vector4 vcLightResponse;
	/// VClouds ambient factors
	Ogre::Vector4 vcAmbientFactors;
	/// VClouds wheater
	Ogre::Vector2 vcWheater;
	/// VClouds lightnings?
	bool vcLightnings;
	/// VClouds lightnings average aparition time
	Ogre::Real vcLightningsAT;
	/// VClouds lightnings color
	Ogre::Vector3 vcLightningsColor;
	/// VClouds lightnings time multiplier
	Ogre::Real vcLightningsTM;
};

class SkyXManager : public IManager
{
public:
	SkyXManager();
	~SkyXManager();
	
	/// change the time scale
	void setPreset(const SkyXSettings& preset);

	Ogre::Vector3 getMainLightDirection();

	Ogre::Light* getMainLight();

	bool update( float dt );

	bool InitLight();

	size_t getMemoryUsage();

	void freeResources();

	bool UpdateSkyLight();

	void setPreDefinedPreset(int id);

	void setDefaultPreset();

	SkyX::SkyX* GetSkyX() { return mSkyX; }

protected:
	Ogre::Light *mLight0;
	Ogre::Light *mLight1;
	
	SkyX::SkyX* mSkyX;
	SkyX::BasicController* mBasicController;

	SkyX::ColorGradient mWaterGradient, //No water for the moment, but will be usefull for hydrax later
		                mSunGradient, 
						mAmbientGradient;

};

#endif // __SkyXManager_H_

