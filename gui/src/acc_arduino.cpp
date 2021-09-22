// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include "shared_file_out.h"
#include <stdint.h>

#include "acc_arduino_types.h"
#include "acc_arduino_math.h"

#define KILOBYTES(size) (         (size) * 1024LL)
#define MEGABYTES(size) (KILOBYTES(size) * 1024LL)
#define GIGABYTES(size) (MEGABYTES(size) * 1024LL)
#define TERABYTES(size) (GIGABYTES(size) * 1024LL)


#define ASSERT(expression) if(!(expression)){ *(u32 *)0x00 = 0; }
//#if SLOW
//#else
//#define ASSERT(expression)
//#endif
//

struct SMElement
{
	HANDLE hMapFile;
	uint8_t* mapFileBuffer;
};

SMElement m_graphics;
SMElement m_physics;
SMElement m_static;


struct draw_buffer
{
    void      *Data;
    BITMAPINFO Info;
    uint32_t   Width;
    uint32_t   Height;
    uint32_t   Pitch;
    uint32_t   BytesPerPixel;
};

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
    
    s32 MinX = 10000;
    s32 MinY = 10000;
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


void FillBitmap(draw_buffer *Buffer, v4f32 color)
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

void DrawFilledRect(draw_buffer *Buffer, rect_v2f32 window, f32 outline)
{
    
    return;
}

void DrawUnfilledRect(draw_buffer *Buffer, rect_v2f32 bounds)
{
    
    return;
}

void DrawBitmap(draw_buffer *Buffer, bitmap *Bitmap, v2s32 Pos)
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
	TCHAR szName[] = TEXT("Local\\acpmf_physics");
    
	m_physics.hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,
                                           NULL,
                                           PAGE_READWRITE,
                                           0,
                                           sizeof(SPageFilePhysics),
                                           szName);
	if (!m_physics.hMapFile)
	{
		MessageBoxA(GetActiveWindow(), "CreateFileMapping failed", "ACCS", MB_OK);
	}
    
	m_physics.mapFileBuffer = (unsigned char*)MapViewOfFile(m_physics.hMapFile,
                                                            FILE_MAP_READ,
                                                            0, 0,
                                                            sizeof(SPageFilePhysics));
	if (!m_physics.mapFileBuffer)
	{
		MessageBoxA(GetActiveWindow(), "MapViewOfFile failed", "ACCS", MB_OK);
	}
    
    return;
}

void InitGraphics()
{
	TCHAR szName[] = TEXT("Local\\acpmf_graphics");
	
    m_graphics.hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,
                                            NULL,
                                            PAGE_READWRITE,
                                            0,
                                            sizeof(SPageFileGraphic),
                                            szName);
    
	if (!m_graphics.hMapFile)
	{
		MessageBoxA(GetActiveWindow(), "CreateFileMapping failed", "ACCS", MB_OK);
	}
	
    m_graphics.mapFileBuffer = (unsigned char*)MapViewOfFile(m_graphics.hMapFile,
                                                             FILE_MAP_READ,
                                                             0, 0,
                                                             sizeof(SPageFileGraphic));
    
	if (!m_graphics.mapFileBuffer)
	{
		MessageBoxA(GetActiveWindow(), "MapViewOfFile failed", "ACCS", MB_OK);
	}
    
    return;
}

void InitStatic()
{
	TCHAR szName[] = TEXT("Local\\acpmf_static");
	
    m_static.hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,
                                          NULL,
                                          PAGE_READWRITE,
                                          0,
                                          sizeof(SPageFileStatic),
                                          szName);
    
	if (!m_static.hMapFile)
	{
		MessageBoxA(GetActiveWindow(), "CreateFileMapping failed", "ACCS", MB_OK);
	}
    
	m_static.mapFileBuffer = (unsigned char*)MapViewOfFile(m_static.hMapFile,
                                                           FILE_MAP_READ,
                                                           0, 0,
                                                           sizeof(SPageFileStatic));
    
	if (!m_static.mapFileBuffer)
	{
		MessageBoxA(GetActiveWindow(), "MapViewOfFile failed", "ACCS", MB_OK);
	}
    
    return;
}

void Dismiss(SMElement element)
{
	UnmapViewOfFile(element.mapFileBuffer);
	CloseHandle(element.hMapFile);
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
    
    //ASSERT(Result1);
    
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
ProcessPendingMessages(void)
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
                    if     (VKCode == 'W')
                    {
                        //MoveUp
                    }
                    else if(VKCode == 'A')
                    {
                        //MoveLeft
                    }
                    else if(VKCode == 'S')
                    {
                        //MoveDown
                    }
                    else if(VKCode == 'D')
                    {
                        //MoveRight
                    }
                    
                    else if(VKCode == 'Q')
                    {
                    }
                    else if(VKCode == 'E')
                    {
                    }
                    else if(VKCode == VK_UP)
                    {
                    }
                    else if(VKCode == VK_LEFT)
                    {
                    }
                    else if(VKCode == VK_DOWN)
                    {
                    }
                    else if(VKCode == VK_RIGHT)
                    {
                    }
                    else if(VKCode == VK_ESCAPE)
                    {
                    }
                    else if(VKCode == VK_SPACE) 
                    {
                    }
                    
                    if(IsDown)
                    {
                        
                        uint32_t alt_key_WasDown = ( Message.lParam & (1 << 29));
                        if((VKCode == VK_F4) && alt_key_WasDown)
                        {
                            g_Running = false;
                        }
                        if((VKCode == VK_RETURN) && alt_key_WasDown)
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

#define PERMANENT_STORAGE_SIZE (MEGABYTES(256))
#define TRANSIENT_STORAGE_SIZE (GIGABYTES(  1))

struct app_memory
{
    size_t PermanentStorageSize;
    size_t TransientStorageSize;
    
    void *PermanentStorage;
    void *TransientStorage;
    
    void   *MainBlock;
    size_t  MainBlockSize;
};


struct memory_arena
{
    size_t  Size;
    size_t  Used;
    void   *BasePtr;
};

void
MemoryArenaInit(memory_arena *Arena, size_t Size, void *BasePtr)
{
    Arena->BasePtr = BasePtr;
    Arena->Size    = Size;
    Arena->Used    = 0;
    
    return;
}

#define MEMORY_ARENA_PUSH_STRUCT(Arena,        Type) (Type *)MemoryArenaPushBlock(Arena, sizeof(Type))
#define MEMORY_ARENA_PUSH_ARRAY( Arena, Count, Type) (Type *)MemoryArenaPushBlock(Arena, (Count) * sizeof(Type))
#define MEMORY_ARENA_ZERO_STRUCT(Instance          )         MemoryArenaZeroBlock(sizeof(Instance), &(Instance))
inline void *
MemoryArenaPushBlock(memory_arena *Arena, size_t Size)
{
    ASSERT((Arena->Used + Size) <= Arena->Size);
    
    void *NewArenaPartitionAdress  = (u8 *)Arena->BasePtr + Arena->Used;
    Arena->Used  += Size;
    
    return NewArenaPartitionAdress;
}
inline void
MemoryArenaZeroBlock(memory_index size, void *address)
{
    u8 *byte = (u8 *)address;
    
    while(size--)
    {
        *byte++ = 0;
    }
    
    return;
}

void WinMainCRTStartup()
{
	InitPhysics();
	InitGraphics();
	InitStatic();
    
    HWND Window = CreateOutputWindow();
    
    app_memory AppMemory = {};
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
    
	while (g_Running)
	{
        ProcessPendingMessages();
        
        /// INPUT
        SPageFilePhysics* physics  = (SPageFilePhysics *)m_physics.mapFileBuffer;
        SPageFileGraphic* graphics = (SPageFileGraphic *)m_graphics.mapFileBuffer;
        SPageFileStatic*  statics  = (SPageFileStatic  *)m_static.mapFileBuffer;
        
        KeyStates[0] = GetAsyncKeyState(0x31);
        KeyStates[1] = GetAsyncKeyState(0x32);
        KeyStates[2] = GetAsyncKeyState(0x33);
        
        u8 ShittyBuffer[256];
        SerialPortRecieveData(&Arduino, ShittyBuffer, sizeof(ShittyBuffer));
        
        /// RENDER
        v4f32 gray = {0.3f, 0.3f, 0.3f, 1.0f};
        FillBitmap(&g_DrawBuffer, gray);
        
        Print(ShittyBuffer, sizeof(ShittyBuffer), Glyphs, &g_DrawBuffer);
        Print((u8 *)"acc G", sizeof("acc G"), Glyphs, &g_DrawBuffer);
        
        
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
    
    Dismiss(m_graphics);
    Dismiss(m_physics);
    Dismiss(m_static);
    
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


