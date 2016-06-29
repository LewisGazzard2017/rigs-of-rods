
-- Catmull-rom spline implementation
-- Ported from http://www.habrador.com/tutorials/catmull-rom-splines/
-------------------------------------------------------------------------------------

-- Returns a position between 4 Vector3 with Catmull-Rom Spline algorithm
--Vector3 ReturnCatmullRom(float t, Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3) {
local CatRom_evaluate = function(t, p0, p1, p2, p3)
	local a = 0.5 * (2 * p1)   --Vector3 a = 0.5f * (2f * p1);
	local b = 0.5 * (p2 - p0)  --Vector3 b = 0.5f * (p2 - p0);

	local c = 0.5 * (2 * p0 - 5 * p1 + 4 * p2 - p3)
	--Vector3 c = 0.5f * (2f * p0 - 5f * p1 + 4f * p2 - p3);
	local d = 0.5f * (-p0 + 3f * p1 - 3f * p2 + p3)
	--Vector3 d = 0.5f * (-p0 + 3f * p1 - 3f * p2 + p3);

	local pos = a + (b * t) + (c * t * t) + (d * t * t * t)
	--Vector3 pos = a + (b * t) + (c * t * t) + (d * t * t * t);

	return pos
end

CatmullRomSpline = {} -- Namespace

