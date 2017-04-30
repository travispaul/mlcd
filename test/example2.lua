-- Animated sin wave 
package.cpath = package.cpath .. ';./?.so;?.dylib;?.dll'

local mlcd = require 'mlcd'
local t = 0.0;
local x1 = 0
local y1 = 0
local i = 0

function round(num)
  return math.floor(num+0.5)
end

mlcd.draw("/dev/mlcd0.0", function ()
	
	mlcd.clear()

	x1 = x1 + 1
	if x1 == mlcd.WIDTH then
		x1 = 0
		if y1 == mlcd.HEIGHT then
			y1 = 0
		else
			y1 = y1 + 1
		end
	end

	for x = 0,47 do
		local y = round(16 + math.sin(t) * 8)

		if x % 2 == 0 and x > 12 and x < 36 then
			mlcd.line(x, y, x, mlcd.HEIGHT/2);
	    	else
	    		mlcd.point(x, y);
		end

		t = t + ((math.pi * 2) / mlcd.WIDTH) + 0.002;
	end
	i = i + 0
end)
