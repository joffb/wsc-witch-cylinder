local process = require("wf.api.v1.process")
local superfamiconv = require("wf.api.v1.process.tools.superfamiconv")

process.emit_symbol("gfx_sparkle", superfamiconv.convert_tilemap(
	"sparkle.png",
	superfamiconv.config()
		:mode("wsc")
        :bpp(4)
        :no_remap()
		:tile_base(400)
		:palette_base(1)
))