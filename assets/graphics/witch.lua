local process = require("wf.api.v1.process")
local superfamiconv = require("wf.api.v1.process.tools.superfamiconv")

process.emit_symbol("gfx_witch", superfamiconv.convert_tilemap(
	"witch.png",
	superfamiconv.config()
		:mode("wsc")
        :bpp(4)
        :no_remap()
		:tile_base(384)
		:palette_base(0)
))