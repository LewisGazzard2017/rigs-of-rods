
dofile ("numeror.lua")

a = NumeRoR.Vector3.new(1,2,3)
b = NumeRoR.Vector3.new(3,2,1)

c = a + b
print("a+b: ", c)

d = 1 + a + 1
local dx = d["x"]
print("a+1: ", -d, dx, d['y'], d.z, d.bar)


