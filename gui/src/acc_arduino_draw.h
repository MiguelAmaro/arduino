/* date = September 23rd 2021 9:27 pm */

#ifndef ACC_ARDUINO_DRAW_H
#define ACC_ARDUINO_DRAW_H

#include "acc_arduino_types.h"
#include "acc_arduino_math.h"

struct draw_buffer
{
    void      *Data;
    BITMAPINFO Info;
    uint32_t   Width;
    uint32_t   Height;
    uint32_t   Pitch;
    uint32_t   BytesPerPixel;
};


void
FillScreen(draw_buffer *Buffer, v4f32 color)
{
    u32 *Pixel = (u32 *)Buffer->Data;
    
    for(u32 Y = 0; Y < Buffer->Height; Y++)
    {
        for(u32 X = 0; X < Buffer->Width; X++)
        {
            *Pixel = (((u8)(255.0f * color.a)  << 24) |
                      ((u8)(255.0f * color.r)  << 16) |
                      ((u8)(255.0f * color.g)  <<  8) |
                      ((u8)(255.0f * color.b)  <<  0));
            
            Pixel++;
        }
    }
    
    return;
}


void
DrawFilledRect(draw_buffer *Buffer,
               v2s32 Pos,
               v2s32 Size,
               v4f32 Color)
{
    /// BASES CHANGE
    v2s32 ScreenPos;
    ScreenPos.x = Pos.x;
    ScreenPos.y = Buffer->Height - Pos.y;
    
    /// Clipping
    rect_v2s32 DrawArea = { 0 };
    DrawArea.min.x = ScreenPos.x - (Size.x / 2);
    DrawArea.min.y = ScreenPos.y - (Size.y / 2);
    DrawArea.max.x = ScreenPos.x + (Size.x / 2);
    DrawArea.max.y = ScreenPos.y + (Size.y / 2);
    
    if(DrawArea.min.x < 0)
    {
        DrawArea.min.x = 0;
    }
    if(DrawArea.min.y < 0)
    {
        DrawArea.min.y = 0;
    }
    if(DrawArea.min.x > Buffer->Width)
    {
        DrawArea.min.x = Buffer->Width;
    }
    if(DrawArea.min.y > Buffer->Height)
    {
        DrawArea.min.y = Buffer->Height;
    }
    
    
    if(DrawArea.max.x < 0)
    {
        DrawArea.max.x = 0;
    }
    if(DrawArea.max.y < 0)
    {
        DrawArea.max.y = 0;
    }
    if(DrawArea.max.x > Buffer->Width)
    {
        DrawArea.max.x = Buffer->Width;
    }
    if(DrawArea.max.y > Buffer->Height)
    {
        DrawArea.max.y = Buffer->Height;
    }
    
    u8  *Dest     = (u8  *)Buffer->Data;
    
    u32 *DestLine = (u32 *)(Dest   + (DrawArea.min.y * Buffer->Pitch));
    
    for(u32 Y = DrawArea.min.y; Y < DrawArea.max.y; Y++)
    {
        u32 *DestPixel   = DestLine + DrawArea.min.x;
        
        for(u32 X = DrawArea.min.x; X < DrawArea.max.x; X++)
        {
            f32 sa = Color.a;
            f32 sr = Color.r * 255.0f;
            f32 sg = Color.g * 255.0f;
            f32 sb = Color.b * 255.0f;
            
            f32 dr = (f32)((*DestPixel   >> 16) & 0xff);
            f32 dg = (f32)((*DestPixel   >>  8) & 0xff);
            f32 db = (f32)((*DestPixel   >>  0) & 0xff);
            
            u8 r = dr * (1.0f - sa) + (sr * sa);
            u8 g = dg * (1.0f - sa) + (sg * sa);
            u8 b = db * (1.0f - sa) + (sb * sa);
            
            *DestPixel = ((0xff << 24) |
                          (r    << 16) |
                          (g    <<  8) |
                          (b    <<  0));
            
            DestPixel++;
        }
        
        DestLine   += Buffer->Width;
        //SourceLine -= Bitmap->Width;
    }
    
    return;
}


void
DrawUnFilledRect(draw_buffer *Buffer,
                 v2s32 Pos,
                 v2s32 Size,
                 f32   Outline,
                 v4f32 Color)
{
    v2s32 BorderSize = { 0 };
    v2s32 BorderPos  = { 0 };
    
    /// TOP
    BorderSize.x = Size.x + (2 * Outline);
    BorderSize.y = Outline;
    BorderPos.x  = Pos.x;
    BorderPos.y  = Pos.y - (Size.y * 0.5f) - (Outline * 0.5f);
    DrawFilledRect(Buffer, BorderPos, BorderSize, Color);
    
    /// BOTTOM
    BorderSize.x = Size.x + (2 * Outline);
    BorderSize.y = Outline;
    BorderPos.x  = Pos.x;
    BorderPos.y  = Pos.y + (Size.y * 0.5f) + (Outline * 0.5f);
    DrawFilledRect(Buffer, BorderPos, BorderSize, Color);
    
    /// RIGHT
    BorderSize.x = Outline;
    BorderSize.y = Size.y + (2 * Outline);
    BorderPos.x  = Pos.x + (Size.x * 0.5f) + (Outline * 0.5f);
    BorderPos.y  = Pos.y;
    DrawFilledRect(Buffer, BorderPos, BorderSize, Color);
    
    /// LEFT
    BorderSize.x = Outline;
    BorderSize.y = Size.y + (2 * Outline);
    BorderPos.x  = Pos.x - (Size.x * 0.5f) - (Outline * 0.5f);
    BorderPos.y  = Pos.y;
    DrawFilledRect(Buffer, BorderPos, BorderSize, Color);
    
    return;
}


#endif //ACC_ARDUINO_DRAW_H
