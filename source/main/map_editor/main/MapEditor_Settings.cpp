#include "MapEditor_Global.h"
#include "MapEditor_StringUtil.h"
#include "MapEditor_Settings.h"
#include "tracksurface.h"

#include "MapEditor_StringUtil.h"
#include <stdio.h>


void SETcom::SerializeCommon(bool w, CONFIGFILE & c)
{
	//  menu
	Param(c,w, "game.start_in_main", startInMain);
	Param(c,w, "game.in_menu", inMenu);				Param(c,w, "game.in_main", isMain);
	
	//  misc
	Param(c,w, "misc.autostart", autostart);		Param(c,w, "misc.ogredialog", ogre_dialog);
	Param(c,w, "misc.escquit", escquit);
	
	Param(c,w, "misc.language", language);			Param(c,w, "misc.mouse_capture", mouse_capture);
	Param(c,w, "misc.screenshot_png", screen_png);

	//  video
	Param(c,w, "video.windowx", windowx);			Param(c,w, "video.windowy", windowy);
	Param(c,w, "video.fullscreen", fullscreen);		Param(c,w, "video.vsync", vsync);

	Param(c,w, "video.fsaa", fsaa);
	Param(c,w, "video.buffer", buffer);				Param(c,w, "video.rendersystem", rendersystem);

	Param(c,w, "video.limit_fps", limit_fps);
	Param(c,w, "video.limit_fps_val", limit_fps_val);	Param(c,w, "video.limit_sleep", limit_sleep);


	//  graphics  ----
	Param(c,w, "graph_detail.preset", preset);
	Param(c,w, "graph_shadow.shader_mode", shader_mode);	Param(c,w, "graph_shadow.lightmap_size", lightmap_size);
	
	Param(c,w, "graph_detail.anisotropy", anisotropy);
	Param(c,w, "graph_detail.tex_filter", tex_filt);		Param(c,w, "graph_detail.tex_size", tex_size);
	Param(c,w, "graph_detail.ter_mtr", ter_mtr);			Param(c,w, "graph_detail.ter_tripl", ter_tripl);
	
	Param(c,w, "graph_detail.view_dist", view_distance);	Param(c,w, "graph_detail.ter_detail", terdetail);
	Param(c,w, "graph_detail.ter_dist", terdist);			Param(c,w, "graph_detail.road_dist", road_dist);
	
	//  water
	Param(c,w, "graph_reflect.water_reflect", water_reflect); Param(c,w, "graph_reflect.water_refract", water_refract);
	Param(c,w, "graph_reflect.water_rttsize", water_rttsize);
	
	//  shadow
	Param(c,w, "graph_shadow.dist", shadow_dist);			Param(c,w, "graph_shadow.size", shadow_size);
	Param(c,w, "graph_shadow.count",shadow_count);			Param(c,w, "graph_shadow.type", shadow_type);

	//  veget
	Param(c,w, "graph_veget.grass", grass);
	Param(c,w, "graph_veget.trees_dist", trees_dist);		Param(c,w, "graph_veget.grass_dist", grass_dist);
	Param(c,w, "graph_veget.use_imposters", use_imposters); Param(c,w, "graph_veget.imposters_only", imposters_only);


	//  hud
	Param(c,w, "hud_show.fps", show_fps);

	//  gui tracks  ---
	Param(c,w, "gui_tracks.view", tracks_view);		Param(c,w, "gui_tracks.filter", tracks_filter);
	Param(c,w, "gui_tracks.sort", tracks_sort);		Param(c,w, "gui_tracks.sortup", tracks_sortup);

	//  columns, filters
	std::string s;
	int i,v,ii,a;
	
	if (w)	//  write
		for (v=0; v < 2; ++v)
		{
			s = "";  ii = COL_VIS;
			for (i=0; i < ii; ++i)
			{
				s += iToStr(col_vis[v][i]);
				if (i < ii-1)  s += " ";
			}
			Param(c,w, "gui_tracks.columns"+iToStr(v), s);

			s = "";  ii = COL_FIL;
			for (i=0; i < ii; ++i)
			{
				s += iToStr(col_fil[v][i]);
				if (i < ii-1)  s += " ";
			}
			Param(c,w, "gui_tracks.filters"+iToStr(v), s);
		}
	else	//  read
		for (v=0; v < 2; ++v)
		{
			if (Param(c,w, "gui_tracks.columns"+iToStr(v), s))
			{	std::stringstream sc(s);
				for (i=0; i < COL_VIS; ++i)
				{	sc >> a;  col_vis[v][i] = a > 0;  }
			}

			if (Param(c,w, "gui_tracks.filters"+iToStr(v), s))
			{	std::stringstream sf(s);
				for (i=0; i < COL_FIL; ++i)
				{	sf >> a;  col_fil[v][i] = a;  }
		}	}
}


SETcom::SETcom()   ///  Defaults
	//  menu
	:isMain(1), startInMain(1), inMenu(0)

	//  misc
	,autostart(0), ogre_dialog(0), escquit(0)
	,language("")  // "" = autodetect lang
	,mouse_capture(true), screen_png(0)

	//  video
	,windowx(800), windowy(600)
	,fullscreen(false), vsync(false)

	,buffer("FBO"), fsaa(0)
	,rendersystem("Default")

	,limit_fps(0), limit_fps_val(60.f), limit_sleep(-1)

	//  graphics
	,preset(4)
	,shader_mode(""), horizon(0), lightmap_size(0) //-

	,tex_filt(2), anisotropy(4), view_distance(2000.f)
	,terdetail(1.f), terdist(300.f), road_dist(1.f)
	,tex_size(1), ter_mtr(2), ter_tripl(0)

	,water_reflect(0), water_refract(0), water_rttsize(0)
	,shadow_type(Sh_Depth), shadow_size(2), shadow_count(3), shadow_dist(1000.f)
	,grass(1.f), trees_dist(1.f), grass_dist(1.f), use_imposters(true), imposters_only(false)

	//  hud
	,show_fps(0)

	//  gui tracks
	,tracks_view(0), tracks_filter(0)
	,tracks_sort(2), tracks_sortup(1)
{

	int i,v;
	for (v=0; v < 2; ++v)
	{	for (i=0; i < COL_FIL; ++i)  col_fil[v][i] = colFil[v][i];
		for (i=0; i < COL_VIS; ++i)  col_vis[v][i] = colVis[v][i];
	}
}

	//  tracks list columns  --
const bool SETcom::colVis[2][COL_VIS] =
	{{0,0,1, 0,0,0, 1,1, 0,0,0,0,0,0,0,0,0,0},
	 {1,0,1, 1,1,1, 1,1, 1,1,1,1,1,1,1,1,1,1}};
	
const char SETcom::colFil[2][COL_FIL] =
	{{01, 0,0, 0,0,0,0,0,0,0,0,0,0},
	 {26, 6,5, 4,3,5,5,4,5,4,5,5,9}};
	///^ up in next ver, also in *default.cfg


void SETTINGS::Load(std::string sfile)
{
	CONFIGFILE c;  c.Load(sfile);
	Serialize(false, c);
}
void SETTINGS::Save(std::string sfile)
{
	CONFIGFILE c;  c.Load(sfile);  version = SET_VER;
	Serialize(true, c);  c.Write();
}


void SETTINGS::Serialize(bool w, CONFIGFILE & c)
{
	c.bFltFull = false;
	
	SerializeCommon(w,c);
	
	//  game common
	Param(c,w, "game.track", gui.track);				Param(c,w, "game.track_user", gui.track_user);
	Param(c,w, "graph_veget.trees", gui.trees);


	Param(c,w, "hud_show.trackmap", trackmap);			Param(c,w, "hud_size.minimap", size_minimap);
	Param(c,w, "hud_show.mini_num", num_mini);			Param(c,w, "hud_show.brushpreview", brush_prv);
	
	Param(c,w, "misc.allow_save", allow_save);
	Param(c,w, "misc.inputBar", inputBar);			Param(c,w, "misc.camPos", camPos);
	Param(c,w, "misc.version", version);
	Param(c,w, "misc.check_load", check_load);		Param(c,w, "misc.check_save", check_save);
	
	Param(c,w, "set_cam.px",cam_x);  Param(c,w, "set_cam.py",cam_y);  Param(c,w, "set_cam.pz",cam_z);
	Param(c,w, "set_cam.dx",cam_dx); Param(c,w, "set_cam.dy",cam_dy); Param(c,w, "set_cam.dz",cam_dz);

	Param(c,w, "set.fog", bFog);					Param(c,w, "set.trees", bTrees);
	Param(c,w, "set.weather", bWeather);
	Param(c,w, "set.cam_speed", cam_speed);			Param(c,w, "set.cam_inert", cam_inert);
	Param(c,w, "set.ter_skip", ter_skip);			Param(c,w, "set.road_sphr", road_sphr);
	Param(c,w, "set.mini_skip", mini_skip);
		
	Param(c,w, "ter_gen.scale", gen_scale);
	Param(c,w, "ter_gen.ofsx", gen_ofsx);			Param(c,w, "ter_gen.ofsy", gen_ofsy);
	Param(c,w, "ter_gen.freq", gen_freq);			Param(c,w, "ter_gen.persist", gen_persist);
	Param(c,w, "ter_gen.pow", gen_pow);				Param(c,w, "ter_gen.oct", gen_oct);
	Param(c,w, "ter_gen.mul", gen_mul);				Param(c,w, "ter_gen.ofsh", gen_ofsh);
	Param(c,w, "ter_gen.roadsm", gen_roadsm);
	Param(c,w, "ter_gen.terMinA", gen_terMinA);		Param(c,w, "ter_gen.terMaxA",gen_terMaxA);
	Param(c,w, "ter_gen.terSmA", gen_terSmA);		Param(c,w, "ter_gen.terSmH",gen_terSmH);
	Param(c,w, "ter_gen.terMinH", gen_terMinH);		Param(c,w, "ter_gen.terMaxH",gen_terMaxH);	

	Param(c,w, "teralign.w_mul", al_w_mul);			Param(c,w, "teralign.smooth", al_smooth);
	Param(c,w, "teralign.w_add", al_w_add);

	Param(c,w, "pacenotes.show", pace_show);		Param(c,w, "pacenotes.dist", pace_dist);
	Param(c,w, "pacenotes.size", pace_size);
	Param(c,w, "pacenotes.near", pace_near);		Param(c,w, "pacenotes.alpha", pace_alpha);
	Param(c,w, "pacenotes.trk_reverse", trk_reverse);

	Param(c,w, "tweak.mtr", tweak_mtr);
	Param(c,w, "pick.set_par", pick_setpar);
}

SETTINGS::SETTINGS()  ///  Defaults
	:version(100)  // old
	//  show
	,trackmap(1), size_minimap(0.5), num_mini(0), brush_prv(1)
	//  misc
	,allow_save(0)
	,check_load(0), check_save(1)
	,inputBar(0), camPos(0)
	//  settings
	,cam_x(0), cam_y(50),cam_z(-120),  cam_dx(0), cam_dy(0), cam_dz(1)
	,bFog(0), bTrees(0), bWeather(0)
	,cam_speed(1.f), cam_inert(1.f)
	,ter_skip(4), road_sphr(2.f), mini_skip(4)
	//  ter gen
	,gen_scale(20.f), gen_freq(0.73f), gen_oct(4), gen_persist(0.4f)
	,gen_pow(1.0f), gen_ofsx(0.f), gen_ofsy(0.f)
	,gen_mul(1.f), gen_ofsh(0.f), gen_roadsm(0.f)
	,gen_terMinA(0.f),gen_terMaxA(90.f),gen_terSmA(10.f)
	,gen_terMinH(-300.f),gen_terMaxH(300.f),gen_terSmH(10.f)
	//  align ter
	,al_w_mul(1.f), al_w_add(8.f), al_smooth(2.f)
	//  pacenotes
	,pace_show(3), pace_dist(1000.f), pace_size(1.f)
	,pace_near(1.f), pace_alpha(1.f)
	,trk_reverse(0), show_mph(0)
	//  tweak
	,tweak_mtr("")
	//  pick
	,pick_setpar(1)
{

	//  track common
	gui.track = "Isl6-Flooded";  gui.track_user = false;
	gui.trees = 1.f;
}
