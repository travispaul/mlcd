-- lua example.lua | fold -bw48
package.cpath = package.cpath .. ';?.so;?.dylib;?.dll'

local mlcd = require 'mlcd'
local t = 0.0;

function round(num)
  return math.floor(num+0.5)
end

mlcd.triangle(1, 6, 12, 1, 23, 6);

mlcd.point(42, 2);

mlcd.point(46, 2);

mlcd.line(42, 4, 46, 4);

mlcd.quad(3, 8, 12, 8, 12, 12, 3, 12);

mlcd.quad(16, 9, 19, 9, 19, 11, 16, 11);

mlcd.circle(32, 24, 5);

for x = 0,47 do
	local y = round(16 + math.sin(t) * 8)
	if x % 2 == 0 and x > 12 and x < 36 then
		mlcd.line(x, y, x, 16);
    else
    	mlcd.point(x, y);
	end
	t = t + ((math.pi * 2) / mlcd.WIDTH) + 0.002;
end

--mlcd.save("preview.bmp");
mlcd.dump();