package.cpath = package.cpath .. ';./?.so;?.dylib;?.dll'

-- Draw a point through the center of the display
local mlcd = require 'mlcd'
local x = 0
mlcd.draw("/dev/mlcd0.0", function ()
	mlcd.clear()
	mlcd.point(x, mlcd.HEIGHT / 2)
	x = x + 1
	if x == mlcd.WIDTH then
		x = 0
	end
end)
