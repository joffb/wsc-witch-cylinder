// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023
#include <wonderful.h>
#include <ws.h>
#include "sine.h"
#include "cylinder.h"
#include "graphics/bg.h"
#include "graphics/walls.h"
#include "graphics/witch.h"
#include "graphics/sparkle.h"


#ifdef IRAM_IMPLEMENTATION
#define IRAM_EXTERN
#else
#define IRAM_EXTERN extern
#endif

#define screen_1 ((ws_scr_entry_t __wf_iram*) 0x1000)
#define screen_2 ((ws_scr_entry_t __wf_iram*) 0x1800)
#define sprites ((ws_sprite_t __wf_iram*) 0x2e00)

#define SPARKLE_TIC_DELAY 4

uint16_t tic;
uint16_t sparkle_tic;

uint8_t vblank_fired;

uint8_t scroll_bg;
uint8_t scroll_x;
uint8_t scroll_y_values[256];

uint8_t witch_x, witch_y;

typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t tile;
} sparkle_t;

uint8_t sparkle;
sparkle_t sparkles[16];

const ws_sprite_t witch_metasprite[16] = {
	{ 
		{.tile = 384, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = -16, .y = -16,  
	},
	{ 
		{.tile = 385, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = -8, .y = -16,  
	},
	{ 
		{.tile = 386, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = 0, .y = -16,  
	},
	{ 
		{.tile = 387, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = 8, .y = -16,  
	},
	{ 
		{.tile = 388, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = -16, .y = -8,  
	},
	{ 
		{.tile = 389, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = -8, .y = -8,  
	},
	{ 
		{.tile = 390, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = 0, .y = -8,  
	},
	{ 
		{.tile = 391, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = 8, .y = -8,  
	},
	{ 
		{.tile = 392, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = -16, .y = 0,  
	},
	{ 
		{.tile = 393, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = -8, .y = 0,  
	},
	{ 
		{.tile = 394, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = 0, .y = 0,  
	},
	{ 
		{.tile = 395, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = 8, .y = 0,  
	},
	{ 
		{.tile = 396, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = -16, .y = 8,  
	},
	{ 
		{.tile = 397, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = -8, .y = 8,  
	},
	{ 
		{.tile = 398, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = 0, .y = 8,  
	},
	{ 
		{.tile = 399, .palette = 0, .inside = 1, .priority = 1, .flip_h = 0, .flip_v = 0 }, 
		.x = 8, .y = 8,  
	},
};

__attribute__((interrupt)) void hblank(void) __far
{
	static uint8_t line;

	line = inportb(IO_LCD_LINE) + 1;

	// update x and y scroll from table
	outportb(IO_SCR2_SCRL_Y, scroll_y_values[line]);
	outportb(IO_SCR2_SCRL_X, cylinder_edge[line] + scroll_x);

	ws_hwint_ack(HWINT_HBLANK_TIMER);
}

__attribute__((interrupt)) void vblank(void) __far
{
	static uint8_t i;
	static uint8_t *y_value_ptr;

	vblank_fired = 1;

	// y scroll values for lines
	y_value_ptr = scroll_y_values;
	
	for (i = 0; i < 144; i++)
	{
		// sine values scaled to 0-63
		*y_value_ptr = cylinder[i] + tic - i;
		y_value_ptr++;
	}

	scroll_y_values[159] = scroll_y_values[0];

	// foreground scroll
	scroll_x = tic;

	// background scroll every two frames
	scroll_bg = (tic & 1) ? (scroll_bg + 1) : scroll_bg;
	outportb(IO_SCR1_SCRL_X, scroll_bg);	

	// sprites
	outportb(IO_SPR_COUNT, 32);	

	// update witch position
	// lissajous curve
	witch_x = (sine[((tic >> 1) + 64) & 0xff] >> 2) + 80;
	witch_y = (sine[tic & 0xff] >> 2) + 40;

	// update witch sprites
	for (i = 0; i < 16; i++ )
	{
		sprites[i] = witch_metasprite[i];
		sprites[i].x += witch_x;
		sprites[i].y += witch_y;
	}

	// if the sparkle timer has been reached
	// re-initialise one spark with the latest witch position
	if (tic >= sparkle_tic)
	{
		sparkles[sparkle].x = witch_x - 16;
		sparkles[sparkle].y = witch_y + (((181 + witch_x) ^ (witch_y + 179)) & 15);
		sparkles[sparkle].tile = ((tic >> 1) & 3);

		// move to the next sparkle in the array
		sparkle = (sparkle + 1) & 15;
		// set up timer for next sparkle update
		sparkle_tic = tic + SPARKLE_TIC_DELAY;
	}

	// update sparkle sprites
	for (i = 0; i < 16; i++)
	{
		// if a sparkle is offscreen
		// keep it there and don't update it
		if (sparkles[i].x < 224 || sparkles[i].x > 248)
		{
			if ((tic & 3) == 0)
			{
				sparkles[i].tile = (sparkles[i].tile + 1) & 3;
			}

			sparkles[i].x = sparkles[i].x - 2;
			sprites[i + 16].x = sparkles[i].x;
			sprites[i + 16].y = sparkles[i].y;
			sprites[i + 16].tile = 400 + sparkles[i].tile;
			sprites[i + 16].palette = 1;
			sprites[i + 16].priority = 1;
		}
	}

	// update tic count
	tic++;

	ws_hwint_ack(HWINT_VBLANK);
}

void disable_interrupts()
{
	// disable cpu interrupts
	cpu_irq_disable();

	// disable wonderswan hardware interrupts
	ws_hwint_disable_all();
}

void enable_interrupts()
{
	// acknowledge interrupt
	outportb(IO_HWINT_ACK, 0xFF);

	// set interrupt handler which only acknowledges the vblank interrupt
	//ws_hwint_set_default_handler_vblank();
	//ws_hwint_set_default_handler_hblank_timer();	
	ws_hwint_set_handler(HWINT_IDX_VBLANK, vblank);
	ws_hwint_set_handler(HWINT_IDX_HBLANK_TIMER, hblank);
	
	outportb(IO_LCD_INTERRUPT, 0);

	outportw(IO_HBLANK_TIMER, 1);
	outportw(IO_TIMER_CTRL, HBLANK_TIMER_ENABLE | HBLANK_TIMER_REPEAT);

	// enable wonderswan vblank interrupt
	ws_hwint_enable(HWINT_VBLANK | HWINT_HBLANK_TIMER);

	vblank_fired = 1;

	// enable cpu interrupts
	cpu_irq_enable();
}

void main(void)
{
	disable_interrupts();

	ws_mode_set(WS_MODE_COLOR_4BPP);

	// disable all video output for now
	outportw(IO_DISPLAY_CTRL, 0);

	// set base addresses for screens 1 and 2
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));

	// set sprite base address
	outportb(IO_SPR_BASE, SPR_BASE(sprites));

	// reset scroll registers to 0
	outportb(IO_SCR1_SCRL_X, 16);
	outportb(IO_SCR1_SCRL_Y, 0);
	outportb(IO_SCR2_SCRL_X, 0);
	outportb(IO_SCR2_SCRL_Y, 0);
	
	// don't render any sprites for now
	outportb(IO_SPR_COUNT, 0);

	// load palettes
	ws_dma_copy_words(MEM_COLOR_PALETTE(0), gfx_bg_palette, gfx_bg_palette_size);
	ws_dma_copy_words(MEM_COLOR_PALETTE(1), gfx_walls_palette, gfx_walls_palette_size);
	ws_dma_copy_words(MEM_COLOR_PALETTE(8), gfx_witch_palette, gfx_witch_palette_size);
	ws_dma_copy_words(MEM_COLOR_PALETTE(9), gfx_sparkle_palette, gfx_sparkle_palette_size);

	// load gfx
	ws_dma_copy_words(MEM_TILE_4BPP(0), gfx_bg_tiles, gfx_bg_tiles_size);
	ws_dma_copy_words(MEM_TILE_4BPP(256), gfx_walls_tiles, gfx_walls_tiles_size);
	ws_dma_copy_words(MEM_TILE_4BPP(384), gfx_witch_tiles, gfx_witch_tiles_size);
	ws_dma_copy_words(MEM_TILE_4BPP(400), gfx_sparkle_tiles, gfx_sparkle_tiles_size);

	// load tilemaps
	ws_dma_copy_words(screen_1, gfx_bg_map, gfx_bg_map_size);
	ws_dma_copy_words(screen_2, gfx_walls_map, gfx_walls_map_size);

	// enable just screen_1
	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE | DISPLAY_SCR2_ENABLE | DISPLAY_SPR_ENABLE);

	tic = 0;

	sparkle = 0;
	sparkle_tic = SPARKLE_TIC_DELAY;

	vblank_fired = 0;

	witch_x = 112;
	witch_y = 72;

	enable_interrupts();

	while(1)
	{
		while (vblank_fired == 0)
		{
			cpu_halt();
		}

		vblank_fired = tic & 0;		
	}
}
