
# Module "project_file"

# Loading and saving rig in JSON "RigProject" format

import json
import copy    
import rig
import ror_truckfile
import euclid3
from datatypes import Color



class ProjectSerializer:

    def __init__(self, project_rig):
        self.rig = project_rig
        self.json_dict = {}
        
    def write_out(self, file_path):
        stream = open(file_path, "w")
        json.dump(self.json_dict, stream, indent=2)
        stream.close()
        
    def process_metadata(self):
        self.json_dict["name"] = self.rig.name
        
    def process_nodes(self):
        json_list = []
        for node in self.rig.loop_node_objects():
            node_dict = copy.copy(node.__dict__)
            del node_dict["linked_beams"]
            del node_dict["curr_screen_pos"]
            del node_dict["curr_screen_color"]
            del node_dict["curr_pos"]
            del node_dict["is_selected"]
            del node_dict["is_hovered"]
            self.do_transforms(node_dict)
            json_list.append(node_dict)
        self.json_dict["nodes"] = json_list
        
    def process_node_buffers(self):
        json_list = []
        for node_buffer in self.rig.loop_node_buffer_objects():
            print("DBG buff")
            buff_dict = copy.copy(node_buffer.__dict__)
            del buff_dict["parent_rig"]
            self.do_transforms(buff_dict)
            json_list.append(buff_dict)
        self.json_dict["node_buffers"] = json_list
        
    def do_transforms(self, _dict):
        ''' Transforms all complex object into JSON-serializable objects '''
        #print("DBG do_transforms:")
        for key in _dict:
            val = _dict[key]
            t = type(val)
            #print("DBG {0:>30}: {1:<30} [{2:>15}]".format(key, '---', str(t)))
            if   t is euclid3.Vector3:
                _dict[key] = [val.x, val.y, val.z]
            elif t is euclid3.Vector2:
                _dict[key] = [val.x, val.y]
            elif t is Color:
                _dict[key] = [val.r, val.g, val.b, val.a]
            elif t is rig.NodeBuffer:
                _dict[key] = val.name
        #print(" >>> AFTER >>>")
        #for key in _dict:
        #    val = _dict[key]
        #    t = type(val)
        #    print("DBG {0:>30}: {1:<30} [{2:>15}]".format(key, str(val), str(t)))



class ProjectLoader:

    def __init__(self, json_dict):
        self.json_dict = json_dict
        self.rig = rig.Rig()        
        
    def process_color(self, rgba_list):
        '''
        Process [r,g,b,a] -> :class:`datatypes.Color`
        :returns:`datatypes.Color`
        '''
        return Color(rgba_list[0], rgba_list[1], rgba_list[2], rgba_list[3])
        
    def dict_process_color(self, _dict, key):
        '''
        Process [r,g,b,a] -> :class:`datatypes.Color`
        '''
        _dict[key] = self.process_color(_dict[key])
        
    def process_vector3(self, xyz_list):
        '''
        Transforms [x,y,z] -> :class:`euclid3.Vector3`
        :returns: :class:`euclid3.Vector3`
        '''
        return euclid3.Vector3(xyz_list[0], xyz_list[1], xyz_list[2])
        
    def dict_process_vector3(self, _dict, key):
        '''
        Transforms [x,y,z] -> :class:`euclid3.Vector3`
        '''
        _dict[key] = self.process_vector3(_dict[key])
        
    def process_node_buffers(self):
        ''' '''
        for buff_dict in self.json_dict["node_buffers"]:
            buff = self.rig.create_node_buffer(buff_dict["name"])
            self.dict_process_color(buff, "editor_color")
            buff.__dict__ = buff_dict
            
    def process_nodes(self):
        ''' '''
        for node_dict in self.json_dict["nodes"]:
            self.dict_process_vector3(node_dict, "position")
            buff_name = node_dict["node_buffer"]
            buff = self.rig.get_node_buffer(buff_name)
            node_dict["node_buffer"] = buff
            node = buff.create_node(node_dict["name"], node_dict["position"])
            node.__dict__ = node_dict
            
    def load():
        self.process_node_buffers()
        self.process_nodes()
        return self.rig            
            
            
    
def save(project_rig, directory, filename):
    s = ProjectSerializer(project_rig)
    s.process_metadata()
    s.process_nodes()
    s.process_node_buffers()
    s.write_out(directory + "/" + filename)
    
    
def load(directory, filename):
    '''
    :returns: The loaded :class:`rig.Rig`
    '''    
    stream = open(directory + "/" + filename)
    json_dict = json.load(stream)    
    stream.close()
    loader = ProjectLoader(json_dict)
    return loader.load()
    
        