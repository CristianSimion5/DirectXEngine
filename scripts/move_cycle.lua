
function update( position, dir, deltaTime )
	if (position.z < -3) 
	then
		position.z = -3
		dir = dir * -1
	elseif (position.z > 3)
	then
		position.z = 3
		dir = dir * -1
	end
	
	position.z = position.z + dir * deltaTime
	return dir
end
