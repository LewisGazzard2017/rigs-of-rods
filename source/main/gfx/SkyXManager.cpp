/*
	This source file is part of Rigs of Rods
	Copyright 2005-2012 Pierre-Michel Ricordel
	Copyright 2007-2012 Thomas Fischer
	Copyright 2013-2014 Petr Ohlidal

	For more information, see http://www.rigsofrods.com/

	Rigs of Rods is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 3, as
	published by the Free Software Foundation.

	Rigs of Rods is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

#include "SkyXManager.h"

#include "Application.h"
#include "OgreSubsystem.h"
#include "Settings.h"
#include "TerrainManager.h"
#include "TerrainGeometryManager.h"

using namespace Ogre;

SkyXManager::SkyXManager()
{
	InitLight();

	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\resource\\SkyX\\","FileSystem", "SkyX",true); //Temp

	mBasicController = new SkyX::BasicController();
	mSkyX = new SkyX::SkyX(gEnv->sceneManager, mBasicController);

	setDefaultPreset();

	mSkyX->create();

	mSkyX->getVCloudsManager()->getVClouds()->setDistanceFallingParams(Ogre::Vector2(2,-1));

	RoR::Application::GetOgreSubsystem()->GetOgreRoot()->addFrameListener(mSkyX);
	RoR::Application::GetOgreSubsystem()->GetRenderWindow()->addListener(mSkyX);
}

SkyXManager::~SkyXManager()
{
	//TODO
	delete mSkyX;
	delete mBasicController;
}

//Predefined presets from the SkyX demo
void SkyXManager::setPreDefinedPreset(int id)
{
	switch(id)
	{
	case -1: // Default (Clear)
		setDefaultPreset(); 
		break;
	case 0: // Sunset
		setPreset(SkyXSettings(Ogre::Vector3(8.85f, 7.5f, 20.5f),  -0.08f, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.0022f, 0.000675f, 30, Ogre::Vector3(0.57f, 0.52f, 0.44f), -0.991f, 3, 4), false, true, 300, false, Ogre::Radian(270), Ogre::Vector3(0.63f,0.63f,0.7f), Ogre::Vector4(0.35, 0.2, 0.92, 0.1), Ogre::Vector4(0.4, 0.7, 0, 0), Ogre::Vector2(0.8,1)));
		break;
	case 1: // Clear
		setPreset(SkyXSettings(Ogre::Vector3(17.16f, 7.5f, 20.5f), 0, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.0017f, 0.000675f, 30, Ogre::Vector3(0.57f, 0.54f, 0.44f), -0.991f, 2.5f, 4), false));
		break;
	case 2: // Thunderstorm 1
		setPreset(SkyXSettings(Ogre::Vector3(12.23, 7.5f, 20.5f),  0, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.00545f, 0.000375f, 30, Ogre::Vector3(0.55f, 0.54f, 0.52f), -0.991f, 1, 4), false, true, 300, false, Ogre::Radian(0), Ogre::Vector3(0.63f,0.63f,0.7f), Ogre::Vector4(0.25, 0.4, 0.5, 0.1), Ogre::Vector4(0.45, 0.3, 0.6, 0.1), Ogre::Vector2(1,1), true, 0.5, Ogre::Vector3(1,0.976,0.92), 2));
		break;
	case 3: // Thunderstorm 2
		setPreset(SkyXSettings(Ogre::Vector3(10.23, 7.5f, 20.5f),  0, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.00545f, 0.000375f, 30, Ogre::Vector3(0.55f, 0.54f, 0.52f), -0.991f, 0.5, 4), false, true, 300, false, Ogre::Radian(0), Ogre::Vector3(0.63f,0.63f,0.7f), Ogre::Vector4(0, 0.02, 0.34, 0.24), Ogre::Vector4(0.29, 0.3, 0.6, 1), Ogre::Vector2(1,1), true, 0.5, Ogre::Vector3(0.95,1,1), 2));
		break;
	case 4: // Desert
		setPreset(SkyXSettings(Ogre::Vector3(7.59f, 7.5f, 20.5f), 0, -0.8f, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.0072f, 0.000925f, 30, Ogre::Vector3(0.71f, 0.59f, 0.53f), -0.997f, 2.5f, 1), true));
		break;
	case 5: // Night
		setPreset(SkyXSettings(Ogre::Vector3(21.5f, 7.5, 20.5), 0.03, -0.25, SkyX::AtmosphereManager::Options(), true));
		break;
	default:
		setDefaultPreset(); // Default (Clear)
		break;
	}
}

void SkyXManager::setDefaultPreset()
{
	SkyX::AtmosphereManager::Options atOpt = mSkyX->getAtmosphereManager()->getOptions();
	atOpt.RayleighMultiplier = 0.003075f;
	atOpt.MieMultiplier = 0.00125f;
	atOpt.InnerRadius = 9.92f;
	atOpt.OuterRadius = 10.3311f;
	mSkyX->getAtmosphereManager()->setOptions(atOpt);
}

void SkyXManager::setPreset(const SkyXSettings& preset)
{
	mSkyX->setTimeMultiplier(preset.timeMultiplier);
	mBasicController->setTime(preset.time);
	mBasicController->setMoonPhase(preset.moonPhase);
	mSkyX->getAtmosphereManager()->setOptions(preset.atmosphereOpt);

	// Layered clouds
	if (preset.layeredClouds)
	{
		// Create layer cloud
		if (mSkyX->getCloudsManager()->getCloudLayers().empty())
		{
			mSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));
		}
	}
	else
	{
		// Remove layer cloud
		if (!mSkyX->getCloudsManager()->getCloudLayers().empty())
		{
			mSkyX->getCloudsManager()->removeAll();
		}
	}

	mSkyX->getVCloudsManager()->setWindSpeed(preset.vcWindSpeed);
	mSkyX->getVCloudsManager()->setAutoupdate(preset.vcAutoupdate);

	SkyX::VClouds::VClouds* vclouds = mSkyX->getVCloudsManager()->getVClouds();

	vclouds->setWindDirection(preset.vcWindDir);
	vclouds->setAmbientColor(preset.vcAmbientColor);
	vclouds->setLightResponse(preset.vcLightResponse);
	vclouds->setAmbientFactors(preset.vcAmbientFactors);
	vclouds->setWheater(preset.vcWheater.x, preset.vcWheater.y, false);

	if (preset.volumetricClouds)
	{
		// Create VClouds
		if (!mSkyX->getVCloudsManager()->isCreated())
		{
			// SkyX::MeshManager::getSkydomeRadius(...) works for both finite and infinite(=0) camera far clip distances
			mSkyX->getVCloudsManager()->create(mSkyX->getMeshManager()->getSkydomeRadius(gEnv->mainCamera));
		}
	}
	else
	{
		// Remove VClouds
		if (mSkyX->getVCloudsManager()->isCreated())
		{
			mSkyX->getVCloudsManager()->remove();
		}
	}

	vclouds->getLightningManager()->setEnabled(preset.vcLightnings);
	vclouds->getLightningManager()->setAverageLightningApparitionTime(preset.vcLightningsAT);
	vclouds->getLightningManager()->setLightningColor(preset.vcLightningsColor);
	vclouds->getLightningManager()->setLightningTimeMultiplier(preset.vcLightningsTM);

	mSkyX->update(0);

}

Vector3 SkyXManager::getMainLightDirection()
{
	if (mBasicController != nullptr)
		return mBasicController->getSunDirection();
	return Ogre::Vector3(0.0,0.0,0.0);
}

Light *SkyXManager::getMainLight()
{
	return mLight1;
}

bool SkyXManager::update(float dt)
{
	UpdateSkyLight();
	mSkyX->update(dt);
	return true;
}


bool SkyXManager::UpdateSkyLight()
{
	Ogre::Vector3 lightDir = -getMainLightDirection();
	Ogre::Vector3 sunPos = gEnv->mainCamera->getDerivedPosition() - lightDir*mSkyX->getMeshManager()->getSkydomeRadius(gEnv->mainCamera);

	// Calculate current color gradients point
	float point = (-lightDir.y + 1.0f) / 2.0f;

	if (gEnv->terrainManager->getHydraxManager())
		gEnv->terrainManager->getHydraxManager()->GetHydrax()->setWaterColor(mWaterGradient.getColor(point));

	mLight0 = gEnv->sceneManager->getLight("Light0");
	mLight1 = gEnv->sceneManager->getLight("Light1");

	mLight0->setPosition(sunPos*0.02);
	mLight1->setDirection(lightDir);
	if (gEnv->terrainManager->getWater()) gEnv->terrainManager->getWater()->setSunPosition(sunPos*0.1);

	Ogre::Vector3 sunCol = mSunGradient.getColor(point);
	mLight0->setSpecularColour(sunCol.x, sunCol.y, sunCol.z);
	if (gEnv->terrainManager->getWater()) gEnv->terrainManager->getWater()->setFadeColour(Ogre::ColourValue(sunCol.x, sunCol.y, sunCol.z));

	Ogre::Vector3 ambientCol = mAmbientGradient.getColor(point);
	mLight1->setDiffuseColour(ambientCol.x, ambientCol.y, ambientCol.z);
	mLight1->setPosition(100,100,100);

	if (mBasicController->getTime().x > 12)
	{
		if (mBasicController->getTime().x > mBasicController->getTime().z)
			mLight0->setVisible(false);
		else
			mLight0->setVisible(true);
	} else
	if (mBasicController->getTime().x < mBasicController->getTime().z)
		mLight0->setVisible(false);
	else
		mLight0->setVisible(true);


	return true;
}

bool SkyXManager::InitLight()
{
	// Water
	mWaterGradient = SkyX::ColorGradient();
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.779105)*0.4, 1));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.729105)*0.3, 0.8));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.25, 0.6));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.2, 0.5));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.1, 0.45));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.025, 0));
	// Sun
	mSunGradient = SkyX::ColorGradient();
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.5, 1.0f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.4, 0.75f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.3, 0.5625f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.6,0.5,0.2)*1.5, 0.5f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.25, 0.45f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.01, 0.0f));
	// Ambient
	mAmbientGradient = SkyX::ColorGradient();
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*1, 1.0f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*1, 0.6f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.6, 0.5f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.3, 0.45f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.1, 0.35f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.05, 0.0f));

	gEnv->sceneManager->setAmbientLight(ColourValue(0.35,0.35,0.35)); //Not needed because terrn2 has ambientlight settings

	// Light
	mLight0 = gEnv->sceneManager->createLight("Light0");
	mLight0->setDiffuseColour(1, 1, 1);
	mLight0->setCastShadows(false);

	mLight1 = gEnv->sceneManager->createLight("Light1");
	mLight1->setType(Ogre::Light::LT_DIRECTIONAL);

	return true;
}

size_t SkyXManager::getMemoryUsage()
{
	//TODO
	return 0;
}

void SkyXManager::freeResources()
{
	//TODO
}
