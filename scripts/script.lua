
file = io.open("script_output/output.txt", "w")

io.output(file)
io.write("script.lua loaded\n")

local vec1 = vector3:new(1, 2, 3)
local vec2 = vector3:new(2, 4, 6)

function dot(v1, v2)
	return v1:dot(v2)
end

function write( arg )
	io.write(arg)
end

function naked_func( )
	write("empty function\n")
end

function close_io( )
	io.close(file)
end


-- io.write(vec1:dot(vec2))

--local trans = transform:new()
--print(trans.pos)

