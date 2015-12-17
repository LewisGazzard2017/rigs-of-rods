
-- Double ended queue

-- Source: "Programming in Lua" book, http://www.lua.org/pil/11.4.html

-- Notation "left/right" changed to "front/back" for better
-- familiarity with C++ std::deque

Deque = {} -- Namespace
Deque.__index = Deque -- Metatable

function Deque.new ()
	local self = {first = 0, last = -1}
	setmetatable(self, Deque)
	return self
end

-- Now, we can insert or remove an element at both ends in constant time:

function Deque.push_front (list, value)
	local first = list.first - 1
	list.first = first
	list[first] = value
end

function Deque.push_back (list, value)
	local last = list.last + 1
	list.last = last
	list[last] = value
end

function Deque.pop_front (list)
	local first = list.first
	if first > list.last then error("Deque.pop_front(): list is empty") end
	local value = list[first]
	list[first] = nil        -- to allow garbage collection
	list.first = first + 1
	return value
end

function Deque.pop_back (list)
	local last = list.last
	if list.first > last then error("Deque.pop_back(): list is empty") end
	local value = list[last]
	list[last] = nil         -- to allow garbage collection
	list.last = last - 1
	return value
end

function Deque.size (list)
	return (list.first - list.last) - 1
end
