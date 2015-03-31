#pragma once

#include "libserg/serg_io.h"

typedef int32_t bool32;

typedef struct
{
    int x;
    int y;
} MiltonBrush;

typedef struct
{
    int32_t     full_width;             // Dimensions of the raster
    int32_t     full_height;
    uint8_t     bytes_per_pixel;
    uint8_t*    raster_buffer;

    int32_t     screen_width;           // Screen dimensions.
    int32_t     screen_height;

    // Heap
    Arena*      root_arena;     // Persistent memory.
    Arena*      frame_arena;    // Gets reset after every call to milton_update().
    // Debug:
} MiltonState;

typedef struct
{
    bool32 full_refresh;
    MiltonBrush* brush;
} MiltonInput;

static void milton_init(MiltonState* milton_state)
{
    // Allocate enough memory for the maximum possible supported resolution. As
    // of now, it seems like future 8k displays will adopt this resolution.
    milton_state->full_width             = 7680;
    milton_state->full_height            = 4320;
    milton_state->bytes_per_pixel   = 4;
    int closest_power_of_two = (1 << 27);  // Ceiling of log2(width * height * bpp)
    milton_state->raster_buffer = arena_alloc_array(milton_state->root_arena,
            closest_power_of_two, uint8_t);
}

// Returns non-zero if the raster buffer was modified by this update.
static bool32 milton_update(MiltonState* milton_state, MiltonInput* input)
{
    bool32 updated = 0;
    // Do a complete re-rasterization.
    if (input->full_refresh)
    {
        uint32_t* pixels = (uint32_t*)milton_state->raster_buffer;
        for (int y = 0; y < milton_state->screen_width; ++y)
        {
            for (int x = 0; x < milton_state->screen_height; ++x)
            {
                *pixels++ = 0xffffffff;
            }
        }
        updated = 1;
    }
    if (input->brush)
    {
        uint32_t* pixels = (uint32_t*)milton_state->raster_buffer;
        MiltonBrush* brush = input->brush;
        pixels[brush->y * milton_state->screen_width + brush->x] = 0xffff0000;
        updated = 1;
    }
    return updated;
}