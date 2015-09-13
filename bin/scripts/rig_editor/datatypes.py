
# ------------------------------------------------------------------------------

class Vector3:
    '3D vector'
    x = 0
    y = 0
    z = 0
    
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z  

# ------------------------------------------------------------------------------

class Color:
    'RGBA color'
    r = 0;
    g = 0;
    b = 0;
    a = 1;
    
    def __init__(self, r, g, b, a):
        self.r = r;
        self.g = g;
        self.b = b;
        self.a = a;
        
    def __init__(self, r, g, b):
        self.r = r;
        self.g = g;
        self.b = b;
        self.a = 1;

# ------------------------------------------------------------------------------ 