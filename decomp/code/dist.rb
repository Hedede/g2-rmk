def distapprox(x1,y1,z1,x2,y2,z2)
	xx = (x1-x2).abs
	yy = (y1-y2).abs
	zz = (z1-z2).abs
	xx,yy = yy,xx unless xx > yy
	xx,zz = zz,xx unless xx > zz
	xx - xx * 0.0625 + (yy+zz) * 0.25 + (yy+zz)  * 0.125
end

def dist(x,y,z,a,b,c)
	xx = x-a
	yy = y-b
	zz = z-c
	Math.sqrt(xx*xx + yy*yy + zz*zz)
end

