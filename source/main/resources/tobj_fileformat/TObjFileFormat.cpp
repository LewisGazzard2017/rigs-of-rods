
#include "TObjFileFormat.h"

using namespace RoR;

#define LINE_BUF_LEN 4000

void TObjParser::Prepare()
{
    m_cur_line           = nullptr;
    m_line_number        = 0;
    m_in_procedural_road = false;
    m_in_procedural_road = false;
    m_road2_use_old_mode = false;
    
    m_def = std::shared_ptr<TObjFile>(new TObjFile());
}

bool TObjParser::ProcessLine(const char* line)
{
    if ((line != nullptr) && (strlen(line) != 0) && (line[0] != '/') || (line[0] != ';'))
    {
        // No trimming by design.
        if (strcmp(line, "end") == 0)
        {
            return false;
        }
        m_cur_line = line; 
        this->ProcessCurrentLine();
    }
    m_line_number++;
    return true;
}

void TObjParser::ProcessOgreDataStream(Ogre::DataStreamPtr ds)
{
	char line_buf[LINE_BUF_LEN];
    while (!ds->eof())
	{
        ds->readLine(line_buf, LINE_BUF_LEN);
        if (! this->ProcessLine(line_buf))
        {
            break;
        }
    }
}

    /*
    Vector3 r2lastpos=Vector3::ZERO;
	Quaternion r2lastrot=Quaternion::IDENTITY;
	int r2counter=0;
    
    		char oname[1024] = {};
		char type[256] = {};
		char name[256] = {};
		Vector3 pos(Vector3::ZERO);
		Vector3 rot(Vector3::ZERO);
    */

void TObjParser::ProcessCurrentLine()
{
    if (strncmp(m_cur_line, "collision-tris", 14) == 0)
    {
        sscanf(m_cur_line, "collision-tris %ld", &m_def->num_collision_triangles);
    }
    else if (strncmp(m_cur_line, "grid", 4) == 0)
    {        
        Ogre::Vector3 & pos = m_def->grid_position;
        sscanf(m_cur_line, "grid %f, %f, %f", &pos.x, &pos.y, &pos.z); // No error check by design
        m_def->grid_enabled = true;
    }
    else if (strncmp(m_cur_line, "trees", 5) == 0)
    {
        TObjTree tree;
        sscanf(m_cur_line, "trees %f, %f, %f, %f, %f, %d, %d, %s %s %s %f %s",
            &tree.yaw_from, &tree.yaw_to, &tree.scale_from, &tree.scale_to, &tree.high_density,
            &tree.min_distance, &tree.max_distance, tree.tree_mesh, tree.color_map, tree.density_map
            &tree.grid_spacing, tree.collision_mesh);
        m_trees.push_back(tree); 
    }
    else if (strncmp(m_cur_line, "grass", 5) == 0 || strncmp(m_cur_line, "grass2", 6) == 0)
    {
        TObjGrass grass;
        if (strncmp(m_cur_line, "grass2", 6) == 0)
        {
            sscanf(m_cur_line, "grass2 %d, %f, %f, %f, %f, %f, %f, %f, %f, %d, %f, %f, %d, %s %s %s",
                &grass.range, &grass.sway_speed, &grass.sway_length, &grass.sway_distribution,
                &grass.density, &grass.min_x, &grass.min_y, &grass.max_x, &grass.max_y,
                &grass.grow_techniq, &grass.min_h, &grass.max_h, &grass.technique,
                grass.material_name, grass.color_map_filename, grass.density_map_filename);
        }
        else
        {
            // Same as 'grass2', except without 'technique' parameter
            sscanf(m_cur_line, "grass2 %d, %f, %f, %f, %f, %f, %f, %f, %f, %d, %f, %f, %s %s %s",
                &grass.range, &grass.sway_speed, &grass.sway_length, &grass.sway_distribution,
                &grass.density, &grass.min_x, &grass.min_y, &grass.max_x, &grass.max_y,
                &grass.grow_techniq, &grass.min_h, &grass.max_h,
                grass.material_name, grass.color_map_filename, grass.density_map_filename);
        }
        m_grass.push_back(grass);
    }    
    


	

		{ // ugly stuff to parse procedural roads
			if (!strncmp("begin_procedural_roads", line, 22))
			{
				po = ProceduralObject();
				po.loadingState = 1;
				r2oldmode = 1;
				proroad = true;
				continue;
			}
			if (!strncmp("end_procedural_roads", line, 20))
			{
				if (r2oldmode)
				{
					if (proceduralManager) proceduralManager->addObject(po);
					po = ProceduralObject();
				}
				proroad = false;
				continue;
			}
			if (proroad)
			{
				float rwidth, bwidth, bheight;
				//position x,y,z rotation rx,ry,rz, width, border width, border height, type
				int r = sscanf(line, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %s",&pos.x,&pos.y,&pos.z, &rot.x, &rot.y, &rot.z, &rwidth, &bwidth, &bheight, oname);
				Quaternion rotation = Quaternion(Degree(rot.x), Vector3::UNIT_X)*Quaternion(Degree(rot.y), Vector3::UNIT_Y)*Quaternion(Degree(rot.z), Vector3::UNIT_Z);
				int roadtype=Road2::ROAD_AUTOMATIC;
				int pillartype = 0;
				if (!strcmp(oname, "flat")) roadtype=Road2::ROAD_FLAT;
				if (!strcmp(oname, "left")) roadtype=Road2::ROAD_LEFT;
				if (!strcmp(oname, "right")) roadtype=Road2::ROAD_RIGHT;
				if (!strcmp(oname, "both")) roadtype=Road2::ROAD_BOTH;
				if (!strcmp(oname, "bridge")) {roadtype=Road2::ROAD_BRIDGE;pillartype=1;}
				if (!strcmp(oname, "monorail")) {roadtype=Road2::ROAD_MONORAIL;pillartype=2;}
				if (!strcmp(oname, "monorail2")) {roadtype=Road2::ROAD_MONORAIL;pillartype=0;}
				if (!strcmp(oname, "bridge_no_pillars")) {roadtype=Road2::ROAD_BRIDGE;pillartype=0;}

				if (r2oldmode)
				{
					//fill object
					ProceduralPoint pp;
					pp.bheight = bheight;
					pp.bwidth = bwidth;
					pp.pillartype = pillartype;
					pp.position = pos;
					pp.rotation = rotation;
					pp.type = roadtype;
					pp.width = rwidth;

					po.points.push_back(pp);
				}
				continue;
			}
		} //end of the ugly (somewhat)

		strcpy(name, "generic");
		memset(oname, 0, 255);
		memset(type, 0, 255);
		memset(name, 0, 255);
		int r = sscanf(line, "%f, %f, %f, %f, %f, %f, %s %s %s",&pos.x,&pos.y,&pos.z, &rot.x, &rot.y, &rot.z, oname, type, name);
		if (r < 6) continue;
		if ((!strcmp(oname, "truck")) || (!strcmp(oname, "load") || (!strcmp(oname, "machine")) || (!strcmp(oname, "boat")) || (!strcmp(oname, "truck2")) ))
		{
			if (!strcmp(oname, "boat") && !terrainManager->getWater())
			{
				// no water so do not load boats!
				continue;
			}
			String group = "";
			String truckname(type);

			if (!RoR::Application::GetCacheSystem()->checkResourceLoaded(truckname, group))
			{
				LOG("Error while loading Terrain: truck " + String(type) + " not found. ignoring.");
				continue;
			}

			truck_prepare_t tempTruckPreload;
			//this is a truck or load declaration
			tempTruckPreload.px = pos.x;
			tempTruckPreload.py = pos.y;
			tempTruckPreload.pz = pos.z;
			tempTruckPreload.freePosition = (!strcmp(oname, "truck2"));
			tempTruckPreload.ismachine = (!strcmp(oname, "machine"));
			tempTruckPreload.rotation = Quaternion(Degree(rot.x), Vector3::UNIT_X)*Quaternion(Degree(rot.y), Vector3::UNIT_Y)*Quaternion(Degree(rot.z), Vector3::UNIT_Z);
			strcpy(tempTruckPreload.name, truckname.c_str());
			truck_preload.push_back(tempTruckPreload);

			continue;
		}
		if (   !strcmp(oname, "road")
			|| !strcmp(oname, "roadborderleft")
			|| !strcmp(oname, "roadborderright")
			|| !strcmp(oname, "roadborderboth")
			|| !strcmp(oname, "roadbridgenopillar")
			|| !strcmp(oname, "roadbridge"))
		{
			int pillartype = !(strcmp(oname, "roadbridgenopillar") == 0);
			// okay, this is a job for roads2
			int roadtype=Road2::ROAD_AUTOMATIC;
			if (!strcmp(oname, "road")) roadtype=Road2::ROAD_FLAT;
			Quaternion rotation;
			rotation = Quaternion(Degree(rot.x), Vector3::UNIT_X)*Quaternion(Degree(rot.y), Vector3::UNIT_Y)*Quaternion(Degree(rot.z), Vector3::UNIT_Z);
			if (pos.distance(r2lastpos) > 20.0f)
			{
				// break the road
				if (r2oldmode != 0)
				{
					// fill object
					ProceduralPoint pp;
					pp.bheight = 0.2;
					pp.bwidth = 1.4;
					pp.pillartype = pillartype;
					pp.position = r2lastpos + r2lastrot * Vector3(10.0f, 0.0f, 0.9f);
					pp.rotation = r2lastrot;
					pp.type = roadtype;
					pp.width = 8;
					po.points.push_back(pp);

					// finish it and start new object
					if (proceduralManager) proceduralManager->addObject(po);
					po = ProceduralObject();
					r2oldmode = 1;
				}
				r2oldmode = 1;
				// beginning of new
				ProceduralPoint pp;
				pp.bheight = 0.2;
				pp.bwidth = 1.4;
				pp.pillartype = pillartype;
				pp.position = pos;
				pp.rotation = rotation;
				pp.type = roadtype;
				pp.width = 8;
				po.points.push_back(pp);
			} else
			{
				// fill object
				ProceduralPoint pp;
				pp.bheight = 0.2;
				pp.bwidth = 1.4;
				pp.pillartype = pillartype;
				pp.position = pos;
				pp.rotation = rotation;
				pp.type = roadtype;
				pp.width = 8;
				po.points.push_back(pp);
			}
			r2lastpos=pos;
			r2lastrot=rotation;

			continue;
		}
		loadObject(oname, pos, rot, bakeNode, name, type);
	}

	// ds closes automatically, so do not close it explicitly here: ds->close();

	// finish the last road
	if (r2oldmode != 0)
	{
		// fill object
		ProceduralPoint pp;
		pp.bheight = 0.2;
		pp.bwidth = 1.4;
		pp.pillartype = 1;
		pp.position = r2lastpos+r2lastrot*Vector3(10.0,0,0);
		pp.rotation = r2lastrot;
		pp.type = Road2::ROAD_AUTOMATIC;
		pp.width = 8;
		po.points.push_back(pp);

		// finish it and start new object
		if (proceduralManager) proceduralManager->addObject(po);
	}
}
    */
}