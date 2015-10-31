
# ------------------------------------------------------------------------------

class Color:
    'RGBA color'
    def __init__(self):
        self.r = 0;
        self.g = 0;
        self.b = 0;
        self.a = 1;
        
    def from_rgba(r, g, b, a):
        c = Color()
        c.r = r
        c.g = g
        c.b = b
        c.a = a
        return c
        
    def from_rgb(r, g, b):
        c = Color()
        c.r = r
        c.g = g
        c.b = b
        return c

# ------------------------------------------------------------------------------ 
