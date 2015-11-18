--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2015 Michael Fink
-- file LuaQuickReference.lua - Demo script for Lua scripting
--

-- this is a comment
i = 3;                          -- assignment to global variable
local text = "Hello World!";    -- local variable inside a function
print(text .. "\n");


-- Lua functions

function transmogrify(a, b) -- specifies a function
  local resultSum = a + b;
  return resultSum, a * b; -- Lua supports multiple return values
end

local transform = function(a, b) -- stores function in local var
  return a * b + 1;
end

local result = transform(5, 4); -- calls stored function

print("result = " .. result .. "\n");


-- Lua tables

Transmogrifier = { -- specifies a table with that name
	magicFactor = 4.0;
	objectToModify = { -- a nested table
		name = "Calvin";
	};
	transmogrify = function(self)
		print("magicFactor = " .. self.magicFactor .. "!\n");
		return 42;
	end;
}

-- syntactic sugar for calling a function inside a table,
-- with passing the table as first argument
Transmogrifier:transmogrify();
print("\n");


-- Lua control structures

while i > 0 do
	print("number: " .. i .. "\n"); -- the dot-dot operator concatenates strings
	i = i - 1;
end
print("\n");

local i = 5;
repeat
	print("modulo: " .. (i % 3) .. "\n");
	i = i - 1;
until i == 0;
print("\n");

for i = 1, 100 do
	local text = "";
	if i % 3 == 0 then text = text .. "Fizz"; end;
	if i % 5 == 0 then text = text .. "Buzz"; end;
	if text == "" then text = text .. i; end;
	print(text .. ", ");
end
print("\n");


-- RemotePhotoTool App table-object

App = {
	run = function()
		print("App:run() called\n");
	end;
}
