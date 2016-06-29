
-- NUMEROR = RoR's 3D math module for Lua
-- Written by Petr Ohlidal "only_a_ptr", 11/2015
-- Licensed MIT (Not covered by RoR's GPL3+)

-----------------------------------------------------------------------------
-- VECTOR 3
-- Internal arrangement: { 1 = x, 2 = y, 3 = z }
-- Access through x/y/z letters is possible due to metamethod __index.
-- Reading on Lua performance: http://www.lua.org/gems/sample.pdf

-- Forward decl.
local V3_new


local V3_meta_mul = function(lhs, rhs)
	if type(lhs) == "table" and type(rhs) == "table" then
		-- Assume 'table' a Vector3
		local x = lhs[1] * rhs[1]
		local y = lhs[2] * rhs[2]
		local z = lhs[3] * rhs[3]
		return V3_new(x, y, z)
	else
		local vec, num
		if type (lhs) == "table" then
			vec = lhs
			num = rhs
		else
			vec = rhs
			num = lhs
		end
		-- Assume it's a number
		local x = vec[1] * num
		local y = vec[2] * num
		local z = vec[3] * num
		return V3_new(x, y, z)
	end
end


local V3_meta_add = function(lhs, rhs)
	if type(lhs) == "table" and type(rhs) == "table" then
		-- Assume 'table' a Vector3
		local x = lhs[1] + rhs[1]
		local y = lhs[2] + rhs[2]
		local z = lhs[3] + rhs[3]
		return V3_new(x, y, z)
	else
		local vec, num
		if type (lhs) == "table" then
			vec = lhs
			num = rhs
		else
			vec = rhs
			num = lhs
		end
		-- Assume it's a number
		local x = vec[1] + num
		local y = vec[2] + num
		local z = vec[3] + num
		return V3_new(x, y, z)
	end
end


local V3_meta_sub = function(lhs, rhs)
	if type(lhs) == "table" and type(rhs) == "table" then
		-- Assume 'table' a Vector3
		local x = lhs[1] - rhs[1]
		local y = lhs[2] - rhs[2]
		local z = lhs[3] - rhs[3]
		return V3_new(x, y, z)
	else
		local vec, num
		if type (lhs) == "table" then
			vec = lhs
			num = rhs
		else
			vec = rhs
			num = lhs
		end
		-- Assume it's a number
		local x = vec[1] - num
		local y = vec[2] - num
		local z = vec[3] - num
		return V3_new(x, y, z)
	end
end


local V3_meta_index = function(self, field)
	if field == "x" then return self[1] end
	if field == "y" then return self[2] end
	if field == "z" then return self[3] end
	return nil
end


local V3_meta_unaryminus = function(self, rhs)
	return V3_new(-self[1], -self[2], -self[3])
end


local V3_meta_tostring = function(self)
	return "Vector3(" .. self[1] .. " " .. self[2] .. " " .. self[3] .. ")"
end


local V3_meta_concat = function(lhs, rhs)
	return tostring(lhs) .. tostring(rhs)
end


local V3_meta = {
	-- See http://lua-users.org/wiki/MetatableEvents
	__index    = V3_meta_index,
	__add      = V3_meta_add,
	__mul      = V3_meta_mul,
	__unm      = V3_meta_unaryminus,
	__concat   = V3_meta_concat,
	__tostring = V3_meta_tostring
}


-- Declared "local" above
V3_new = function(x, y, z)
	local vec3 = { x, y, z }
	setmetatable(vec3, V3_meta)
	return vec3
end


-----------------------------------------------------------------------------
-- EXPORT

NumeRoR = {
	Vector3 = {
		new = V3_new
	}
}



