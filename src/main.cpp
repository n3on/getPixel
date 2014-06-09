#include <windows.h>
#include <stdio.h>

//process the window messages
LRESULT CALLBACK windowProc(HWND, UINT, WPARAM, LPARAM);
//extract RGB values from COLORREF
void toRGB(COLORREF color, unsigned char *rgb);

//handle to the screen
HDC screen;
//timer id
unsigned int timer = 100;
//interval in ms to get the color
unsigned int interval = 100;
//color of the pixel under mouse pointer
//colorref has form: 0x00bbggrr
COLORREF pixelColor = 0;
COLORREF pixelColorLast = 0;
//color rectangle for visualization
RECT colorRect;

//main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
    //program and class name
    const char *progName = "getPixel";

    //create window class
    WNDCLASS windowClass;

    //message function
    windowClass.lpfnWndProc = windowProc;
    //enable redrawing
    windowClass.style       = CS_HREDRAW | CS_VREDRAW;
    //program instance
    windowClass.hInstance   = hInstance;
    //standard cursor
    windowClass.hCursor     = LoadCursor(NULL, IDC_ARROW);
    //standard icon
    windowClass.hIcon       = LoadIcon(NULL, IDI_APPLICATION);
    //gray background
    windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));
    //extra buffer
    windowClass.cbClsExtra  = 0;
    windowClass.cbWndExtra  = 0;
    //name to represent our window class
    windowClass.lpszClassName = progName;
    //we don't need menu
    windowClass.lpszMenuName  = NULL;

    //register our class to windows
    if(!RegisterClass(&windowClass))
            return 0;

    //create window
    HWND window;
    window = CreateWindowEx(WS_EX_TOPMOST, //allways on top of others
                            progName, //name of window class
                            progName, //text in the titlebar
                            //no resizable window without minimize option
                            WS_OVERLAPPEDWINDOW^WS_MAXIMIZEBOX^WS_THICKFRAME,
                            //default window position
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            //window width and height
                            300, 185,
                            //no parent window
                            NULL,
                            //no menu
                            NULL,
                            hInstance,
                            //no extra parameters
                            NULL);

    //quit if we can't create a window
    if(!window)
        return 0;

    //show
    ShowWindow(window, nCmdShow);
    UpdateWindow(window);

    //process messages
    MSG message;
    while(GetMessage(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return message.wParam;
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        //window creation
        case WM_CREATE:
        {
            //get the whole display context
            screen = CreateDC("DISPLAY",NULL,NULL,NULL);
            if(!screen)
                return 0;

            //set timer to get the pixel color
            if(!SetTimer(hwnd, timer, interval, NULL))
                return 0;

            //rectangle for color visualisation
            colorRect.left   = 180;
            colorRect.top    = 15;
            colorRect.right  = 280;
            colorRect.bottom = 126;

        }break;
        //timer
        case WM_TIMER:
        {
            //get the current mouse position
            POINT mousePos;
            GetCursorPos(&mousePos);

            //get the color under the mouse position
            pixelColor = GetPixel(screen, mousePos.x, mousePos.y);

            //if the color doesn't change, then don't redraw
            if(pixelColor!=pixelColorLast)
                InvalidateRect(hwnd, NULL, true);

            //save color for comparisson
            pixelColorLast = pixelColor;
        }break;
        //repaint
        case WM_PAINT:
        {
            //get the window device context for drawing
            PAINTSTRUCT ps;
            HDC windowDC;
            windowDC = BeginPaint(hwnd, &ps);

            //background color for text
            SetBkColor(windowDC, 0x848284);

            //draw rectangular frame around our rectangle
            Rectangle(windowDC, colorRect.left-1,  colorRect.top-1,
                        colorRect.right+1, colorRect.bottom+1);

            //draw the rectangle in pixel color for more visuualization
            HBRUSH color = CreateSolidBrush(pixelColor);
            FillRect(windowDC, &colorRect, color);

            char text[50] = " ";
            //saves rgb values
            unsigned char rgbColors[3];
            //extract RGB values from COLORREF
            toRGB(pixelColor, rgbColors);

            //Output of information
            sprintf(text, "Red   :  0x%02X  %d", rgbColors[0], rgbColors[0]);
            TextOut(windowDC, 10, 15, text, strlen(text));

            sprintf(text, "Green:  0x%02X %d", rgbColors[1], rgbColors[1]);
            TextOut(windowDC, 10, 45, text, strlen(text));

            sprintf(text, "Blue  :  0x%02X  %d", rgbColors[2], rgbColors[2]);
            TextOut(windowDC, 10, 75, text, strlen(text));

            sprintf(text, "RGB Color:  0x%02X%02X%02X", rgbColors[0], rgbColors[1], rgbColors[2]);
            TextOut(windowDC, 10, 115, text, strlen(text));

            EndPaint(hwnd, &ps);
        }break;
        case WM_DESTROY:
        {
            //clean up
            DeleteDC(screen);
            KillTimer(hwnd, timer);
            PostQuitMessage(0);
            return 0;
        }break;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

//rgb[0] = red
//rgb[1] = green
//rgb[2] = blue
void toRGB(COLORREF color, unsigned char *rgb)
{
    rgb[0] = (color & 0x000000FF);
    rgb[1] = (color & 0x0000FF00)>>8;
    rgb[2] = (color & 0x00FF0000)>>16;
}
