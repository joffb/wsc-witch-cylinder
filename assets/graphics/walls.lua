local process = require("wf.api.v1.process")
local superfamiconv = require("wf.api.v1.process.tools.superfamiconv")

process.emit_symbol("gfx_walls", superfamiconv.convert_tilemap(
	"walls.png",
	superfamiconv.config()
		:mode("wsc")
        :bpp(4)
        :no_remap()
		:tile_base(256)
		:palette_base(1)
))