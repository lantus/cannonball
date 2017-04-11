/***************************************************************************
    SDL Software Video Rendering.

    Known Bugs:
    - Scanlines don't work when Endian changed?

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

#include <iostream>

#include "rendersw.hpp"
#include "frontend/config.hpp"

RenderSW::RenderSW()
{
    scan_pixels = NULL;
    pix         = NULL;
}

RenderSW::~RenderSW()
{
    if (scan_pixels)
        delete[] scan_pixels;

    if (pix)
        delete[] pix;
}

bool RenderSW::init(int src_width, int src_height,
                    int scale,
                    int video_mode,
                    int scanlines)
{
    this->src_width  = src_width;
    this->src_height = src_height;
    this->video_mode = video_mode;
    this->scanlines  = scanlines;

    // Setup SDL Screen size
    if (!RenderBase::sdl_screen_size())
        return false;

    int flags = SDL_FLAGS  ;

    // --------------------------------------------------------------------------------------------
    // Full Screen Mode
    // When using full-screen mode, we attempt to keep the current resolution.
    // This is because for LCD monitors, I suspect it's what we want to remain in
    // and we don't want to risk upsetting the aspect ratio.
    // --------------------------------------------------------------------------------------------
    if (video_mode == video_settings_t::MODE_FULL || video_mode == video_settings_t::MODE_STRETCH)
    {
        scn_width  = orig_width;
        scn_height = orig_height;

        scale_factor = 0; // Use scaling code

        if (video_mode == video_settings_t::MODE_STRETCH)
        {
            dst_width  = scn_width;
            dst_height = scn_height;
            scanlines = 0; // Disable scanlines in stretch mode
        }
        else
        {
            // With scanlines, only allow a proportional scale
            if (scanlines)
            {
                scale_factor = std::min(scn_width / src_width, scn_height / src_height);
                dst_width    = src_width  * scale_factor;
                dst_height   = src_height * scale_factor;
            }
            else
            {
                // Calculate how much to scale screen from its original resolution
                uint32_t w = (scn_width  << 16)  / src_width;
                uint32_t h = (scn_height << 16)  / src_height;
                dst_width  = (src_width  * std::min(w, h)) >> 16;
                dst_height = (src_height * std::min(w, h)) >> 16;
            }
        }
        flags |= SDL_FULLSCREEN; // Set SDL flag
        SDL_ShowCursor(false);   // Don't show mouse cursor in full-screen mode
    }
    // --------------------------------------------------------------------------------------------
    // Windowed Mode
    // --------------------------------------------------------------------------------------------
    else
    {
        this->video_mode = video_settings_t::MODE_WINDOW;

        scale_factor  = scale;

        scn_width  = src_width  * scale_factor;
        scn_height = src_height * scale_factor;

        // As we're windowed this is just the same
        dst_width  = scn_width;
        dst_height = scn_height;

        SDL_ShowCursor(true);
    }

    // If we're not stretching the screen, centre the image
    if (video_mode != video_settings_t::MODE_STRETCH)
    {
        screen_xoff = scn_width - dst_width;
        if (screen_xoff)
            screen_xoff = (screen_xoff / 2);

        screen_yoff = scn_height - dst_height;
        if (screen_yoff)
            screen_yoff = (screen_yoff / 2) * scn_width;
    }
    // Otherwise set to the top-left corner
    else
    {
        screen_xoff = 0;
        screen_yoff = 0;
    }

    //int bpp = info->vfmt->BitsPerPixel;
    const int bpp = 32;
    const int available = SDL_VideoModeOK(scn_width, scn_height, bpp, flags);

    // Frees (Deletes) existing surface
    if (surface)
        SDL_FreeSurface(surface);

    // Set the video mode


    surface = SDL_SetVideoMode(scn_width, scn_height, bpp, flags );

    if (!surface || !available)
    {
        std::cerr << "Video mode set failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Convert the SDL pixel surface to 32 bit.
    // This is potentially a larger surface area than the internal pixel array.
    screen_pixels = (uint32_t*)surface->pixels;


    // SDL Pixel Format Information
    Rshift = surface->format->Rshift;
    Gshift = surface->format->Gshift;
    Bshift = surface->format->Bshift;
    Rmask  = surface->format->Rmask;
    Gmask  = surface->format->Gmask;
    Bmask  = surface->format->Bmask;
    
 


    return true;
}

void RenderSW::disable()
{

}

bool RenderSW::start_frame()
{
    return true;
}

bool RenderSW::finalize_frame()
{

    SDL_Flip(surface);

    return true;
}

void RenderSW::draw_frame(uint16_t* pixels)
{
        uint32_t* spix = screen_pixels;

        // Lookup real RGB value from rgb array for backbuffer
        for (int i = 0; i < (0x11800); i+=16)
        {
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];

        }
}
    // Example: Set the pixel at 10,10 to red

    // --


