// ConsoleApplication1.cpp : Defines the entry point for the console application.

#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include "shared_file_out.h"
#include <stdint.h>

#include "acc_arduino_types.h"
#include "acc_arduino_math.h"
#include "acc_arduino_draw.h"
#include "acc_arduino_memory.h"

#define KILOBYTES(size) (         (size) * 1024LL)
#define MEGABYTES(size) (KILOBYTES(size) * 1024LL)
#define GIGABYTES(size) (MEGABYTES(size) * 1024LL)
#define TERABYTES(size) (GIGABYTES(size) * 1024LL)


struct button_state
{
    b32 EndedDown;
    u32 HalfTransitionCount;
};

struct app_input
{
    button_state ConnectArduino; 
    button_state SpawnGraph;
};


struct ui_graph
{
    v2s32       Pos;
    v2s32       Size;
    
    void       *SampleBuffer;
    u32         SampleMaxCount;
    u32         SampleCount;
    u32         SampleHead;
    
    u32         SampleElementCount;
    u32         SampleStride;
    
    rect_v2s32  Range;
    v2f32       Origin;
    
    v2s32       GridResolution;
    u32         GridLineThickness;
    
    u32         UnitInPixels;
};


struct app_state
{
    ui_graph   Graphs[128];
    u32        GraphCount;
    u32        GraphMaxCount;
};

void
UI_PushGraph(app_state *AppState, 
             v2s32 Size,
             v2s32 Pos,
             void  *SampleBuffer,
             u32    SampleMaxCount,
             u32    SampleCount,
             u32    SampleHead,
             u32    SampleElementCount,
             u32    SampleStride,
             s32 RangeMinX,
             s32 RangeMaxX,
             s32 RangeMinY,
             s32 RangeMaxY,
             u32 GridResolutionX,
             u32 GridResolutionY,
             u32 GridLineThickness,
             u32 UnitInPixels)
{
    if(AppState->GraphCount < AppState->GraphMaxCount)
    {
        ui_graph *Graph = AppState->Graphs + AppState->GraphCount++;
        
        Pos.y -== (u32)(Size.y * 0.5f) * AppState->GraphCount;
        Graph->Pos  = Pos;
        
        
        Graph->Size = Size;
        
        /// BUFFER BINDING
        Graph->SampleBuffer   = SampleBuffer;
        Graph->SampleMaxCount = SampleMaxCount;
        Graph->SampleCount    = SampleCount;
        Graph->SampleHead     = SampleHead;
        
        Graph->SampleElementCount = SampleElementCount;
        Graph->SampleStride       = SampleStride;
        
        Graph->Range.min.x  = RangeMinX;
        Graph->Range.max.x  = RangeMaxX;
        Graph->Range.min.y  = RangeMinY;
        Graph->Range.max.y  = RangeMaxX;
        
        Graph->Origin       = {0.5f , 0.5f};
        
        Graph->GridResolution.x    = GridResolutionX;
        Graph->GridResolution.y    = GridResolutionY;
        Graph->GridLineThickness   = GridLineThickness;
        
        Graph->UnitInPixels      = UnitInPixels;
    }
    
    return;
}


struct sm_element
{
	HANDLE MappedFile;
	u8*    FileBuffer;
};

sm_element g_Graphics;
sm_element g_Physics;
sm_element g_Static;


struct bitmap
{
    s32   Width;
    s32   Height;
    s32   Pitch;
    void *Data;
    void *FreedMemory;
};

#define BITMAP_BYTES_PER_PIXEL 4

bitmap
LoadGlyphBitmap(const char *FileName, const char *FontName, u32 CodePoint)
{
    bitmap Result = {};
    
    static HDC DeviceContext = 0;
    
    if(!DeviceContext)
    {
        AddFontResourceExA(FileName, FR_PRIVATE, 0);
        
        int Height = 128; // TODO(casey): Figure out how to specify pixels properly here
        
        HFONT Font = CreateFontA(Height, 0, 0, 0,
                                 FW_NORMAL, // NOTE(casey): Weight
                                 TRUE, // NOTE(casey): Italic
                                 FALSE, // NOTE(casey): Underline
                                 FALSE, // NOTE(casey): Strikeout
                                 DEFAULT_CHARSET, 
                                 OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, 
                                 ANTIALIASED_QUALITY,
                                 DEFAULT_PITCH|FF_DONTCARE,
                                 FontName);
        
        DeviceContext = CreateCompatibleDC(0);
        HBITMAP Bitmap = CreateCompatibleBitmap(DeviceContext, 1024, 1024);
        SelectObject(DeviceContext, Bitmap);
        SelectObject(DeviceContext, Font);
        
        SetBkColor(DeviceContext, RGB(0, 0, 0));
        
        TEXTMETRIC TextMetric;
        GetTextMetrics(DeviceContext, &TextMetric);
    }
    
    wchar_t CheesePoint = (wchar_t)CodePoint;
    
    SIZE Size;
    GetTextExtentPoint32W(DeviceContext, &CheesePoint, 1, &Size);
    
    u32 Width  = Size.cx;
    u32 Height = Size.cy;
    
    SetTextColor(DeviceContext, RGB(255, 255, 255));
    
    TextOutW(DeviceContext,
             0, 0,
             &CheesePoint, 1);
    
    s32 MinX =  10000;
    s32 MinY =  10000;
    s32 MaxX = -10000;
    s32 MaxY = -10000;
    
    for(s32 Y = 0;
        Y < Height;
        ++Y)
    {
        for(s32 X = 0;
            X < Width;
            ++X)
        {
            COLORREF Pixel = GetPixel(DeviceContext, X, Y);
            if(Pixel != 0)
            {
                if(MinX > X)
                {
                    MinX = X;                    
                }
                
                if(MinY > Y)
                {
                    MinY = Y;                    
                }
                
                if(MaxX < X)
                {
                    MaxX = X;                    
                }
                
                if(MaxY < Y)
                {
                    MaxY = Y;                    
                }
            }
        }
    }
    
    if(MinX <= MaxX)
    {
        --MinX;
        --MinY;
        ++MaxX;
        ++MaxY;
        
        Width = (MaxX - MinX) + 1;
        Height = (MaxY - MinY) + 1;
        
        Result.Width  = Width;
        Result.Height = Height;
        Result.Pitch  = Result.Width * BITMAP_BYTES_PER_PIXEL;
        Result.Data   = VirtualAlloc(0, 
                                     Result.Height * Result.Pitch,
                                     MEM_COMMIT | MEM_RESERVE,
                                     PAGE_READWRITE);
        
        Result.FreedMemory = Result.Data;
        
        u8 *DestRow = (u8 *)Result.Data + (Height - 1)*Result.Pitch;
        for(s32 Y = MinY;
            Y <= MaxY;
            ++Y)
        {
            u32 *Dest = (u32 *)DestRow;
            for(s32 X = MinX;
                X <= MaxX;
                ++X)
            {
                COLORREF Pixel = GetPixel(DeviceContext, X, Y);
                u8 Gray = (u8)(Pixel & 0xFF);
                u8 Alpha = 0xFF;
                *Dest++ = ((Gray << 24) |
                           (Gray << 16) |
                           (Gray <<  8) |
                           (Gray <<  0));
            }
            
            DestRow -= Result.Pitch;
        }
    }
    
    return Result;
}


void
DrawBitmap(draw_buffer *Buffer, bitmap *Bitmap, v2s32 Pos)
{
    /// Clipping
    rect_v2s32 DrawArea = { 0 };
    DrawArea.min.x = Pos.x;
    DrawArea.min.y = Pos.y;
    DrawArea.max.x = Pos.x + Bitmap->Width;
    DrawArea.max.y = Pos.y + Bitmap->Height;
    
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
    
    u8 *Source = (u8 *)Bitmap->Data;
    u8 *Dest   = (u8 *)Buffer->Data;
    
    u32 *SourceLine = (u32 *)(Source + (Bitmap->Pitch * (Bitmap->Height - 1)));
    u32 *DestLine   = (u32 *)(Dest   + (DrawArea.min.y * Buffer->Pitch));
    
    for(u32 Y = DrawArea.min.y; Y < DrawArea.max.y; Y++)
    {
        u32 *SourcePixel = SourceLine;
        u32 *DestPixel   = DestLine + DrawArea.min.x;
        
        for(u32 X = DrawArea.min.x; X < DrawArea.max.x; X++)
        {
            f32 sa = (f32)((*SourcePixel >> 24) & 0xff) / 255.0f;
            f32 sr = (f32)((*SourcePixel >> 16) & 0xff);
            f32 sg = (f32)((*SourcePixel >>  8) & 0xff);
            f32 sb = (f32)((*SourcePixel >>  0) & 0xff);
            
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
            SourcePixel++;
        }
        
        DestLine   += Buffer->Width;
        SourceLine -= Bitmap->Width;
    }
    
    return;
}


void Print(u8 *String, u32 Length, bitmap *Glyphs, draw_buffer *Buffer)
{
    u32 NullBitmapData[4 * 4] = { 0xFFFF0000 };
    bitmap NullBitmap = { 0 };
    NullBitmap.Width  = 4;
    NullBitmap.Height = 4;
    NullBitmap.Pitch  = NullBitmap.Width * sizeof(NullBitmapData[0]);
    NullBitmap.Data        = NullBitmapData;
    NullBitmap.FreedMemory = NullBitmapData;
    
    bitmap *UpperAlphaGlyph = Glyphs;
    bitmap *LowerAlphaGlyph = UpperAlphaGlyph + 26;
    bitmap *NumericGlyph    = LowerAlphaGlyph + 26;
    
    v2s32 Pos = {0 , 0};
    
    for(u32 StringIndex = 0; StringIndex < Length; StringIndex++)
    {
        u8 Char = String[StringIndex];
        
        bitmap *GlyphBitmap = 
            ((Char >= 'A') && (Char <= 'Z'))? UpperAlphaGlyph + (Char - 'A'):
        ((Char >= 'a') && (Char <= 'z'))? LowerAlphaGlyph + (Char - 'a'):
        ((Char >= '0') && (Char <= '9'))? NumericGlyph    + (Char - '0'):
        &NullBitmap;
        
        Pos.x = StringIndex * 67;
        DrawBitmap(Buffer, GlyphBitmap, Pos);
    }
    
    return;
}



void Display(draw_buffer *Buffer,
             HDC          DeviceContext)
{
    uint32_t OffsetX = 0;
    uint32_t OffsetY = 0;
    
    StretchDIBits(DeviceContext,
                  OffsetX, OffsetY,
                  Buffer->Width, Buffer->Height,
                  0, 0,
                  Buffer->Width,  Buffer->Height,
                  Buffer->Data , &Buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);
    
    return;
}



void
DrawGraph(draw_buffer *Buffer, bitmap *Glyphs, ui_graph *Graph)
{
    /// PALLETTE
    v4f32 Blue   = {0.0f, 0.0f, 1.0f, 1.0f};
    v4f32 Red    = {1.0f, 0.0f, 0.0f, 1.0f};
    v4f32 Yellow = {1.0f, 1.0f, 0.0f, 1.0f};
    v4f32 Cyan   = {0.0f, 1.0f, 1.0f, 1.0f};
    v4f32 Green  = {0.0f, 1.0f, 0.0f, 1.0f};
    v4f32 GridLineGray = {0.4f, 0.4f, 0.4f, 1.0f};
    
    
    Print((u8 *)"Current Velocity", sizeof("velocity"), Glyphs, Buffer);
    
    
    v2s32 StartDrawPos =
    {
        Graph->Pos.x - (Graph->Size.x / 2),
        Graph->Pos.y
    };
    
    /// GRID DRAW
    u32 GW = (Graph->GridResolution.x * Graph->GridLineThickness);
    u32 GH = (Graph->GridResolution.y * Graph->GridLineThickness);
    
    ASSERT((Graph->GridResolution.x * Graph->GridLineThickness) <= Graph->Size.x);
    ASSERT((Graph->GridResolution.y * Graph->GridLineThickness) <= Graph->Size.y);
    
    u32 NumPixelsForGridSpacingX = Graph->Size.x - GW;
    u32 NumPixelsForGridSpacingY = Graph->Size.y - GH;
    
    u32 GridSpacingX = NumPixelsForGridSpacingX / Graph->GridResolution.x;
    u32 GridSpacingY = NumPixelsForGridSpacingY / Graph->GridResolution.y;
    
    u32 SpaceBudgetX = NumPixelsForGridSpacingX;
    for(u32 GridLineIndex = 0; SpaceBudgetX > 0; SpaceBudgetX -= GridSpacingX, GridLineIndex++)
    {
        v2s32 GridLinePos;
        GridLinePos.x = StartDrawPos.x + ((GridLineIndex + 1) * GridSpacingX); 
        GridLinePos.y = Graph->Pos.y; 
        v2s32 GridLineSize;
        GridLineSize.x = Graph->GridLineThickness; 
        GridLineSize.y = Graph->Size.y; 
        
        DrawFilledRect(Buffer, GridLinePos, GridLineSize, GridLineGray);
    }
    u32 SpaceBudgetY = NumPixelsForGridSpacingY;
    for(u32 GridLineIndex = 0; SpaceBudgetY > 0; SpaceBudgetY -= GridSpacingY, GridLineIndex++)
    {
        v2s32 GridLinePos;
        GridLinePos.x = Graph->Pos.x; 
        GridLinePos.y = StartDrawPos.y + ((GridLineIndex + 1) * GridSpacingY); 
        v2s32 GridLineSize;
        GridLineSize.x = Graph->Size.x; 
        GridLineSize.y = Graph->GridLineThickness; 
        
        DrawFilledRect(Buffer, GridLinePos, GridLineSize, GridLineGray);
    }
    
    
    ASSERT((Graph->SampleCount * 2) < Graph->Size.x);
    u32 SampleSpacing = Graph->Size.x / Graph->SampleCount;
    /// SAMPLE DRAW
    for(u32 SampleIndex = 0; SampleIndex < Graph->SampleCount; SampleIndex++)
    {
        u32 RollingIndex = (Graph->SampleHead + SampleIndex) % Graph->SampleCount;
        
        v3f32 Sample = ((v3f32 *)Graph->SampleBuffer)[RollingIndex];
        
        v2s32 SamplePos;
        v2s32 SampleSize;
        
        SamplePos.x = StartDrawPos.x + (SampleSpacing * SampleIndex); 
        SamplePos.y = StartDrawPos.y + (Sample.x * Graph->UnitInPixels); 
        SampleSize.x = 3; 
        SampleSize.y = 3; 
        
        DrawFilledRect(Buffer, SamplePos, SampleSize, Cyan);
        
        SamplePos.x = StartDrawPos.x + (SampleSpacing * SampleIndex); 
        SamplePos.y = StartDrawPos.y + (Sample.y * Graph->UnitInPixels); 
        SampleSize.x = 3; 
        SampleSize.y = 3; 
        
        DrawFilledRect(Buffer, SamplePos, SampleSize, Yellow);
        
        
        SamplePos.x = StartDrawPos.x + (SampleSpacing * SampleIndex); 
        SamplePos.y = StartDrawPos.y + (Sample.z * Graph->UnitInPixels); 
        SampleSize.x = 3; 
        SampleSize.y = 3; 
        
        DrawFilledRect(Buffer, SamplePos, SampleSize, Green);
    }
    
    DrawUnFilledRect(Buffer, Graph->Pos, Graph->Size, 2, Red);
    
    return;
}


draw_buffer g_DrawBuffer;
uint32_t g_Running = true;


static LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    
    switch (Message)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } break;
        
        case WM_CHAR:
        case WM_SIZE:
        {
            //PostThreadMessage(RenderThreadID, Message, WParam, LParam);
        } break;
        
        default:
        {
            Result = DefWindowProcW(Window, Message, WParam, LParam);
        } break;
    }
    
    return Result;
}

#define DEFAULT_WINDOW_COORDX  10
#define DEFAULT_WINDOW_COORDY  10
#define DEFAULT_WINDOW_WIDTH  800
#define DEFAULT_WINDOW_HEIGHT 1000

static HWND CreateOutputWindow()
{
    WNDCLASSEXW WindowClass = {};
    
    WindowClass.cbSize = sizeof(WindowClass);
    WindowClass.lpfnWndProc = &WindowProc;
    WindowClass.hInstance = GetModuleHandleW(NULL);
    WindowClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    WindowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WindowClass.lpszClassName = L"accarduinoclass";
    
    
    HWND Result = {0};
    if(RegisterClassExW(&WindowClass))
    {
        // NOTE(casey): Martins says WS_EX_NOREDIRECTIONBITMAP is necessary to make
        // DXGI_SWAP_EFFECT_FLIP_DISCARD "not glitch on window resizing", and since
        // I don't normally program DirectX and have no idea, we're just going to
        // leave it here :)
        DWORD ExStyle = 0;//WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP;
        
        Result = CreateWindowExW(ExStyle,
                                 WindowClass.lpszClassName,
                                 L"Asseto Arduino",
                                 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                 DEFAULT_WINDOW_COORDX, DEFAULT_WINDOW_COORDY,
                                 DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
                                 0, 0, WindowClass.hInstance, 0);
    }
    
    return Result;
}

void InitPhysics()
{
	CHAR ExistingMappedObjectName[] = "Local\\acpmf_physics";
    
	g_Physics.MappedFile = CreateFileMapping(INVALID_HANDLE_VALUE,
                                             NULL,
                                             PAGE_READWRITE,
                                             0,
                                             sizeof(SPageFilePhysics),
                                             ExistingMappedObjectName);
	if (!g_Physics.MappedFile)
	{
		MessageBoxA(GetActiveWindow(), "CreateFileMapping failed", "ACCS", MB_OK);
	}
    
	g_Physics.FileBuffer = (unsigned char*)MapViewOfFile(g_Physics.MappedFile,
                                                         FILE_MAP_READ,
                                                         0, 0,
                                                         sizeof(SPageFilePhysics));
	if (!g_Physics.FileBuffer)
	{
		MessageBoxA(GetActiveWindow(), "MapViewOfFile failed", "ACCS", MB_OK);
	}
    
    return;
}

void InitGraphics()
{
	CHAR ExistingMappedObjectName[] = "Local\\acpmf_graphics";
	
    g_Graphics.MappedFile = CreateFileMapping(INVALID_HANDLE_VALUE,
                                              NULL,
                                              PAGE_READWRITE,
                                              0,
                                              sizeof(SPageFileGraphic),
                                              ExistingMappedObjectName);
    
	if (!g_Graphics.MappedFile)
	{
		MessageBoxA(GetActiveWindow(), "CreateFileMapping failed", "ACCS", MB_OK);
	}
	
    g_Graphics.FileBuffer = (unsigned char*)MapViewOfFile(g_Graphics.MappedFile,
                                                          FILE_MAP_READ,
                                                          0, 0,
                                                          sizeof(SPageFileGraphic));
    
	if (!g_Graphics.FileBuffer)
	{
		MessageBoxA(GetActiveWindow(), "MapViewOfFile failed", "ACCS", MB_OK);
	}
    
    return;
}

void InitStatic()
{
	CHAR ExistingMappedObjectName[] = "Local\\acpmf_static";
	
    g_Static.MappedFile = CreateFileMappingA(INVALID_HANDLE_VALUE,
                                             NULL,
                                             PAGE_READWRITE,
                                             0,
                                             sizeof(SPageFileStatic),
                                             ExistingMappedObjectName);
    
	if (!g_Static.MappedFile)
	{
		MessageBoxA(GetActiveWindow(), "CreateFileMapping failed", "ACCS", MB_OK);
	}
    
	g_Static.FileBuffer = (unsigned char*)MapViewOfFile(g_Static.MappedFile,
                                                        FILE_MAP_READ,
                                                        0, 0,
                                                        sizeof(SPageFileStatic));
    
	if (!g_Static.FileBuffer)
	{
		MessageBoxA(GetActiveWindow(), "MapViewOfFile failed", "ACCS", MB_OK);
	}
    
    return;
}

void Dismiss(sm_element element)
{
	UnmapViewOfFile(element.FileBuffer);
	CloseHandle(element.MappedFile);
}


// NOTE(MIGUEL): Should This code go in the windows platfom layer?? and i just keep abastractions and generics here

typedef struct
{
    HANDLE CommHandle;
    //RingBuffer transmit_queue;
    //RingBuffer recieve_queue;
} serial_device;

static serial_device g_SerialDevice; 
static u8            g_SerialCommBuffer[256];

b32 win32_SerialPort_device_init(void);


// NOTE(MIGUEL): should take params to decide a device
b32
SerialDeviceInit(serial_device *SerialDevice,
                 const char *CommNumber,
                 u32 BaudRate,
                 u32 ByteSize)
{
    b32 Result = false;
    
    // NOTE(MIGUEL): How do I know which com it is??
    // TODO(MIGUEL): Find out why comm get initialize with garbage
    SerialDevice->CommHandle = CreateFile(CommNumber,
                                          GENERIC_READ | GENERIC_WRITE,
                                          0,
                                          NULL,
                                          OPEN_EXISTING,
                                          0,
                                          NULL);
    
    
    if(SerialDevice->CommHandle != INVALID_HANDLE_VALUE)
    {
        DCB SerialDeviceParams       = { 0 };
        SerialDeviceParams.DCBlength = sizeof(SerialDeviceParams);
        SerialDeviceParams.BaudRate  = BaudRate  ;    
        SerialDeviceParams.ByteSize  = ByteSize  ;         
        SerialDeviceParams.StopBits  = ONESTOPBIT;
        SerialDeviceParams.Parity    = NOPARITY  ;
        
        SetCommState(SerialDevice->CommHandle, &SerialDeviceParams);
        
        COMMTIMEOUTS TimeoutsComm                = { 0 };
        TimeoutsComm.ReadIntervalTimeout         = 50; // in milliseconds
        TimeoutsComm.ReadTotalTimeoutConstant    = 50; // in milliseconds
        TimeoutsComm.ReadTotalTimeoutMultiplier  = 10; // in milliseconds
        TimeoutsComm.WriteTotalTimeoutConstant   = 50; // in milliseconds
        TimeoutsComm.WriteTotalTimeoutMultiplier = 10; // in milliseconds
        
        // TODO(MIGUEL): Read about this!!!
        SetCommTimeouts(SerialDevice->CommHandle, &TimeoutsComm);
        
        DWORD EventMask;
        
        if(!SetCommMask(SerialDevice->CommHandle, EV_RXCHAR))
        {
            Result = false;
        }
        if(!WaitCommEvent(SerialDevice->CommHandle, &EventMask, 0))
        {
            Result = false;
        }
    }
    
    return Result;
}

void
SerialPortTransmitData(serial_device *SerialDevice, u8 *Data, u32 Size)
{
    b32   Result       = 0;
    DWORD BytesWritten = 0;
    
    if(SerialDevice->CommHandle != INVALID_HANDLE_VALUE)
    {
        Result = WriteFile(SerialDevice->CommHandle,
                           Data,
                           Size,
                           &BytesWritten,
                           0);
    }
    
    //ASSERT(Result);
    
    return;
}

void
SerialPortRecieveData(serial_device *SerialDevice,
                      u8 *SerialPortBuffer,
                      u32 SerialPortBufferSize)
{
    b32 Result = false;
    
    DWORD PayloadSize = SerialPortBufferSize;  
    DWORD BytesToRead = PayloadSize;
    DWORD BytesRead   = 0;
    
    if(SerialDevice->CommHandle != INVALID_HANDLE_VALUE)
    {
#if 1
        u8 Data;
        do
        {
            Result = ReadFile(SerialDevice->CommHandle,
                              &Data,
                              sizeof(Data),
                              &BytesRead,
                              0);
            
            //ASSERT(Result);
            
            BytesToRead -= BytesRead;
            
            SerialPortBuffer[(PayloadSize - BytesToRead)] = Data;
        }
        while ((BytesToRead > 0) && (BytesRead < PayloadSize));
#else
        do
        {
            ReadFile(SerialDevice->CommHandle,
                     &SerialPortBuffer,
                     BytesToRead,
                     &BytesRead,
                     0);
            
            BytesToRead      -= BytesRead;
            SerialPortBuffer += BytesRead;
        }
        while ((BytesToRead > 0) && (BytesRead < PayloadSize));
#endif
    }
    
    return;
}

void
SerialPortCloseDevice(serial_device *SerialDevice)
{
    CloseHandle(SerialDevice->CommHandle);
    
    return;
}


void
ProcessKeyboardMessage(button_state *NewState, b32 IsDown)
{
    if(NewState->EndedDown != IsDown)
    {
        NewState->EndedDown = IsDown;
        ++NewState->HalfTransitionCount;
    }
    
    return;
}


void
ProcessPendingMessages(app_input *Input)
{
    MSG Message = {};
    
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_QUIT:
            {
                g_Running = false;
            }  break;
            /*
            case WM_MOUSEWHEEL:
            {
                input->mouse_wheel_delta = ((s16)(Message.wParam >> 16) / 120.0f);
                input->mouse_wheel_integral += input->mouse_wheel_delta;
            } break;
            */
            case WM_SYSKEYUP:
            
            case WM_SYSKEYDOWN:
            
            case WM_KEYDOWN:
            
            case WM_KEYUP:
            {
                uint32_t VKCode          = (uint32_t)Message.wParam;
                uint32_t WasDown         = ((Message.lParam & (1 << 30)) != 0);
                uint32_t IsDown          = ((Message.lParam & (1 << 31)) == 0);
                
                if(WasDown != IsDown)
                {
                    switch(VKCode)
                    {
                        case 'C':
                        {
                            ProcessKeyboardMessage(&Input->ConnectArduino, IsDown);
                        } break;
                        
                        case 'E':
                        {
                            ProcessKeyboardMessage(&Input->SpawnGraph, IsDown);
                        } break;
                        
                        case VK_UP:
                        {
                        } break;
                        
                        case VK_LEFT:
                        {
                        } break;
                        
                        case VK_DOWN:
                        {
                        } break;
                        
                        case VK_RIGHT:
                        {
                        } break;
                        
                        case VK_ESCAPE:
                        {
                        } break;
                        
                        case VK_SPACE: 
                        {
                        } break;
                    }
                    
                    if(IsDown)
                    {
                        
                        u32 AltKeyWasDown = ( Message.lParam & (1 << 29));
                        if((VKCode == VK_F4) && AltKeyWasDown)
                        {
                            g_Running = false;
                        }
                        if((VKCode == VK_RETURN) && AltKeyWasDown)
                        {
                            if(Message.hwnd)
                            {
                                //win32_toggle_fullscreen(Message.hwnd );
                            }
                        }
                    }
                }
            } break;
            
            default:
            {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            } break;
        }
    }
    
    return;
}

void WinMainCRTStartup()
{
	InitPhysics();
	InitGraphics();
	InitStatic();
    
    HWND Window = CreateOutputWindow();
    
    app_memory AppMemory = { 0 };
    
    LPVOID BaseAddress = 0;
    
    AppMemory.PermanentStorageSize = PERMANENT_STORAGE_SIZE;
    AppMemory.TransientStorageSize = TRANSIENT_STORAGE_SIZE;
    
    AppMemory.MainBlockSize = (AppMemory.PermanentStorageSize +
                               AppMemory.TransientStorageSize);
    
    AppMemory.MainBlock = VirtualAlloc(BaseAddress, 
                                       (size_t)AppMemory.MainBlockSize,
                                       MEM_COMMIT | MEM_RESERVE,
                                       PAGE_READWRITE);
    
    AppMemory.PermanentStorage = ((uint8_t *)AppMemory.MainBlock);
    
    AppMemory.TransientStorage = ((uint8_t *)AppMemory.PermanentStorage +
                                  AppMemory.PermanentStorageSize);
    
    
    memory_arena RenderArena = {};
    
    app_state *AppState = (app_state *)AppMemory.PermanentStorage;
    AppState->GraphMaxCount = 256;
    
    MemoryArenaInit(&RenderArena,
                    AppMemory.TransientStorageSize,
                    AppMemory.TransientStorage);
    
    g_DrawBuffer.Width                        =          DEFAULT_WINDOW_WIDTH;
    g_DrawBuffer.Height                       =         DEFAULT_WINDOW_HEIGHT;
    g_DrawBuffer.BytesPerPixel                =                             4;
    g_DrawBuffer.Pitch =  (g_DrawBuffer.Width * g_DrawBuffer.BytesPerPixel);
    
    g_DrawBuffer.Info.bmiHeader.biSize        =      sizeof(BITMAPINFOHEADER);
    g_DrawBuffer.Info.bmiHeader.biWidth       =         g_DrawBuffer.Width   ; 
    g_DrawBuffer.Info.bmiHeader.biHeight      = -((s32)(g_DrawBuffer.Height)); 
    g_DrawBuffer.Info.bmiHeader.biPlanes      =                             1;
    g_DrawBuffer.Info.bmiHeader.biBitCount    =                            32;
    g_DrawBuffer.Info.bmiHeader.biCompression =                        BI_RGB;
    
#if 0
    g_DrawBuffer.Data = MEMORY_ARENA_PUSH_ARRAY(&RenderArena,
                                                g_DrawBuffer.Width * g_DrawBuffer.Height,
                                                u32);
#else
    g_DrawBuffer.Data = VirtualAlloc(0, 
                                     g_DrawBuffer.Height * g_DrawBuffer.Pitch,
                                     MEM_COMMIT | MEM_RESERVE,
                                     PAGE_READWRITE);
#endif
    
    serial_device Arduino = { 0 };
    SerialDeviceInit(&Arduino, "\\\\.\\COM5", 9600, 8);
    
    // TODO(MIGUEL): Implement a simple asset system for this shit
    bitmap Glyphs[26 * 26 * 10];
    bitmap *UpperAlphaGlyph = Glyphs;
    bitmap *LowerAlphaGlyph = UpperAlphaGlyph + 26;
    bitmap *NumericGlyph    = LowerAlphaGlyph + 26;
    
    for(u8 alpha = 'a'; alpha < 'z'; alpha++)
    {
        LowerAlphaGlyph[alpha - 'a'] = LoadGlyphBitmap("C:\\Windows\\Fonts\\Ariel.ttf", "Ariel", alpha);
    }
    for(u8 alpha = 'A'; alpha < 'Z'; alpha++)
    {
        UpperAlphaGlyph[alpha - 'A'] = LoadGlyphBitmap("C:\\Windows\\Fonts\\Ariel.ttf", "Ariel", alpha);
    }
    for(u8 numeric = '0'; numeric < '9'; numeric++)
    {
        NumericGlyph[numeric - '0'] = LoadGlyphBitmap("C:\\Windows\\Fonts\\Ariel.ttf", "Ariel", numeric);
    }
    
    b32 KeyStates[3];
    
    app_input Input;
    
    
	while (g_Running)
	{
        ProcessPendingMessages(&Input);
        
        /// INPUT
        SPageFilePhysics* AssettoPhysics  = (SPageFilePhysics *)g_Physics.FileBuffer;
        SPageFileGraphic* AssettoGraphics = (SPageFileGraphic *)g_Graphics.FileBuffer;
        SPageFileStatic*  AssettoStatics  = (SPageFileStatic  *)g_Static.FileBuffer;
        
        KeyStates[0] = GetAsyncKeyState(0x31);
        KeyStates[1] = GetAsyncKeyState(0x32);
        KeyStates[2] = GetAsyncKeyState(0x33);
        
        u8 ShittyBuffer[256];
        SerialPortRecieveData(&Arduino, ShittyBuffer, sizeof(ShittyBuffer));
        
        /// RENDER
        v4f32 gray = {0.15f, 0.15f, 0.15f, 1.0f};
        FillScreen(&g_DrawBuffer, gray);
        
        Print(ShittyBuffer, sizeof(ShittyBuffer), Glyphs, &g_DrawBuffer);
        Print((u8 *)"acc G", sizeof("acc G"), Glyphs, &g_DrawBuffer);
        
        v2s32 Pos  = { 400, 600};
        v2s32 Size = { 600, 400};
        v4f32 Red  = {1.0f, 0.0f, 0.0f, 1.0f};
        v4f32 Blue = {0.0f, 0.0f, 1.0f, 1.0f};
        
        
        static u32   VelocitySampleCount    = 0;
        static u32   VelocityMaxSampleCount = 256;
        static v3f32 VelocitySamples[256];
        static u32   VelocitySampleHead = 0;
        static u32   VelocitySampleTail = 0;
        
        if(VelocitySampleCount < VelocityMaxSampleCount)
        {
            VelocitySamples[VelocitySampleCount].x = AssettoPhysics->velocity[0];
            VelocitySamples[VelocitySampleCount].y = AssettoPhysics->velocity[1];
            VelocitySamples[VelocitySampleCount].z = AssettoPhysics->velocity[2];
            VelocitySampleCount++;
            VelocitySampleTail++;
        }
        else
        {
            VelocitySampleTail = VelocitySampleTail++ % VelocityMaxSampleCount;
            
            VelocitySamples[VelocitySampleTail].x = AssettoPhysics->velocity[0];
            VelocitySamples[VelocitySampleTail].y = AssettoPhysics->velocity[1];
            VelocitySamples[VelocitySampleTail].z = AssettoPhysics->velocity[2];
            
            VelocitySampleHead = VelocitySampleHead++ % VelocityMaxSampleCount;
        }
        
        s32 KilometersInPixels = 4; 
        
        
        if(Input.SpawnGraph.EndedDown)
        {
            // TODO(MIGUEL): create lister of graph sources
            
            // TODO(MIGUEL): create a path for the srcs. i think
            
            UI_PushGraph(AppState,
                         Size,
                         Pos,
                         VelocitySamples,
                         VelocityMaxSampleCount,
                         VelocitySampleCount,
                         VelocitySampleHead,
                         3,
                         3 * sizeof(f32),
                         0, 100,
                         0, 100,
                         10, 4, 2, KilometersInPixels);
        }
        
        ui_graph *Graph = AppState->Graphs;
        for(u32 GraphIndex = 0;
            GraphIndex < AppState->GraphCount;
            GraphIndex++, Graph++)
        {
            DrawGraph(&g_DrawBuffer, Glyphs, Graph);
        }
        
#if 0
		if (KeyStates[0]) // user pressed 1
		{
			//wcout << "---------------PHYSICS INFO---------------" << endl;
            Print("acc G", pf->accG);
			Print("brake", pf->brake);
			Print("camber rad", pf->camberRAD);
			Print("damage", pf->carDamage);
			Print("car height", pf->cgHeight);
			Print("drs", pf->drs);
			Print("tc", pf->tc);
			Print("fuel", pf->fuel);
			Print("gas", pf->gas);
			Print("gear", pf->gear);
			Print("number of tyres out", pf->numberOfTyresOut);
			Print("packet id", pf->packetId);
			Print("heading", pf->heading);
			Print("pitch", pf->pitch);
			Print("roll", pf->roll);
			Print("rpms", pf->rpms);
			Print("speed kmh", pf->speedKmh);
            //Print2("contact point", pf->tyreContactPoint);
            //Print2("contact normal", pf->tyreContactNormal);
            //Print2("contact heading", pf->tyreContactHeading);
			Print("steer ", pf->steerAngle);
			Print("suspension travel", pf->suspensionTravel);
			Print("tyre core temp", pf->tyreCoreTemperature);
			Print("tyre dirty level", pf->tyreDirtyLevel);
			Print("tyre wear", pf->tyreWear);
			Print("velocity", pf->velocity);
			Print("wheel angular speed", pf->wheelAngularSpeed);
			Print("wheel load", pf->wheelLoad);
			Print("wheel slip", pf->wheelSlip);
			Print("wheel pressure", pf->wheelsPressure);
		}
        
		if (KeyStates[1]) // user pressed 2
		{
			//wcout << "---------------GRAPHICS INFO---------------" << endl;
			Print("packetID ", pf->packetId);
			Print("STATUS ", pf->status);
			Print("session", pf->session);
			Print("completed laps", pf->completedLaps);
			Print("position", pf->position);
			Print("current time s", pf->currentTime);
			Print("current time", pf->iCurrentTime);
            Print("last time s", pf->lastTime);
            Print("last time ", pf->iLastTime);
			Print("best time s", pf->bestTime);
			Print("best time", pf->iBestTime);
			Print("sessionTimeLeft", pf->sessionTimeLeft);
			Print("distanceTraveled", pf->distanceTraveled);
			Print("isInPit", pf->isInPit);
			Print("currentSectorIndex", pf->currentSectorIndex);
			Print("lastSectorTime", pf->lastSectorTime);
			Print("numberOfLaps", pf->numberOfLaps);
			//wcout << "TYRE COMPOUND : " << pf->tyreCompound << endl;
			Print("replayMult", pf->replayTimeMultiplier);
			Print("normalizedCarPosition", pf->normalizedCarPosition);
			//Print2("carCoordinates", pf->carCoordinates);
		}
        
        
		if (KeyStates[2]) // user pressed 3
		{
			//wcout << "---------------STATIC INFO---------------" << endl;
			//wcout << "SM VERSION " << pf->smVersion << endl;
			//wcout << "AC VERSION " << pf->acVersion << endl;
            
			Print("number of sessions ", pf->numberOfSessions);
			Print("numCars", pf->numCars);
			//wcout << "Car model " << pf->carModel << endl;
			//wcout << "Car track " << pf->track << endl;
			//wcout << "Player Name " << pf->playerName << endl;
			Print("sectorCount", pf->sectorCount);
            
			Print("maxTorque", pf->maxTorque);
			Print("maxPower", pf->maxPower);
			Print("maxRpm", pf->maxRpm);
			Print("maxFuel", pf->maxFuel);
			Print("suspensionMaxTravel", pf->suspensionMaxTravel);
			Print("tyreRadius", pf->tyreRadius);
            
		}
#endif
        
        HDC DeviceContext = GetDC(Window);
        Display(&g_DrawBuffer, DeviceContext);
    }
    
    Dismiss(g_Graphics);
    Dismiss(g_Physics);
    Dismiss(g_Static);
    
}

// CRT stuff

extern "C" int _fltused = 0x9875;

#pragma function(memset)
void *memset(void *DestInit, int Source, size_t Size)
{
    unsigned char *Dest = (unsigned char *)DestInit;
    while(Size--) *Dest++ = (unsigned char)Source;
    
    return(DestInit);
}

#pragma function(memcpy)
void *memcpy(void *DestInit, void const *SourceInit, size_t Size)
{
    unsigned char *Source = (unsigned char *)SourceInit;
    unsigned char *Dest = (unsigned char *)DestInit;
    while(Size--) *Dest++ = *Source++;
    
    return(DestInit);
}


