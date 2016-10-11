#pragma once
#include "configfile.h"
#include "MapEditor_ConfigFile.h"


#define SET_VER  2600  // 2.6


enum eShadowType  {  Sh_None=0, Sh_Depth, Sh_Soft  };

// Originally in separate header "settings_com.h" (com for 'common')
class SETcom
{
public:
//------------------------------------------

	//  misc
	std::string language;
	bool isMain;  int inMenu;  // last menu id

	//  startup, other
	bool autostart, escquit, startInMain;
	bool ogre_dialog, mouse_capture, screen_png;

	//  screen
	int windowx, windowy, fsaa;
	bool fullscreen, vsync;
	std::string buffer, rendersystem;
	//  limit
	bool limit_fps;  float limit_fps_val;  int limit_sleep;
	
	//  hud
	bool show_fps;


	//  graphics  ----
	int anisotropy,  tex_filt, tex_size;  // textures
	int ter_mtr, ter_tripl;  // terrain
	float view_distance, terdetail,  terdist, road_dist;  // detail
	
	bool water_reflect, water_refract;  int water_rttsize;  // water
	float shadow_dist;  int shadow_size, lightmap_size, shadow_count,  shadow_type; //eShadowType

	bool use_imposters, imposters_only;  //  veget
	float grass, trees_dist, grass_dist;  // trees in gui.

	//  graphics other
	int preset;  // last set, info only
	bool horizon;
	std::string shader_mode;


	//  track
	int tracks_view, tracks_sort;
	bool tracks_sortup, tracks_filter;

	#define COL_VIS 18
	#define COL_FIL 13
	
	bool col_vis[2][COL_VIS];  // visible columns for track views
	int  col_fil[2][COL_FIL];  // filtering range for columns 0min-1max

	const static bool colVis[2][COL_VIS];
	const static char colFil[2][COL_FIL];
	

//------------------------------------------
	SETcom();

	template <typename T>
	bool Param(CONFIGFILE & conf, bool write, std::string pname, T & value)
	{
		if (write)
		{	conf.SetParam(pname, value);
			return true;
		}else
			return conf.GetParam(pname, value);
	}
	void SerializeCommon(bool write, CONFIGFILE & config);
};

class SETTINGS : public SETcom
{
public:
///  params
//------------------------------------------
	int version;  // file version
	
	//  show
	bool trackmap, brush_prv;  int num_mini;
	float size_minimap;

	class GameSet
	{
	public:
		std::string track;  bool track_user;
		float trees;  // common
	} gui;
	
	//  misc
	bool allow_save, inputBar, camPos;
	bool check_load, check_save;

	//  settings
	bool bFog, bTrees, bWeather;
	int ter_skip, mini_skip;  float road_sphr;
	float cam_speed, cam_inert, cam_x,cam_y,cam_z, cam_dx,cam_dy,cam_dz;
	
	//  ter generate
	float gen_scale, gen_ofsx,gen_ofsy, gen_freq, gen_persist, gen_pow;  int gen_oct;
	float gen_mul, gen_ofsh, gen_roadsm;
	float gen_terMinA,gen_terMaxA,gen_terSmA, gen_terMinH,gen_terMaxH,gen_terSmH;

	//  align ter
	float al_w_mul, al_w_add, al_smooth;
	
	//  pacenotes
	int pace_show;  float pace_dist, pace_size, pace_near, pace_alpha;
	bool trk_reverse, show_mph;

	//  tweak
	std::string tweak_mtr;
	//  pick
	bool pick_setpar;

	
//------------------------------------------
	SETTINGS();

	template <typename T>
	bool Param(CONFIGFILE & conf, bool write, std::string pname, T & value)
	{
		if (write)
		{	conf.SetParam(pname, value);
			return true;
		}else
			return conf.GetParam(pname, value);
	}
	void Serialize(bool write, CONFIGFILE & config);
	void Load(std::string sfile), Save(std::string sfile);
};
