#include <iostream>
#include <Windows.h>

bool IsPixelYellow(COLORREF pixel)
{
    int red = GetRValue(pixel);
    int green = GetGValue(pixel);
    int blue = GetBValue(pixel);

    int redmin = 200;
    int redmax = 260;     
                                        //настройка цвета
    int greenmin = 200;
    int greenmax = 260; 

    if (redmin <= red && redmax >= red && greenmin <= green && greenmax >= green && blue == 0)
    {
        return true;
    }

    return false;
}

std::pair<int, int> SearchTatget(int x1,int y1,int x2,int y2)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

    int targetX = 0, targetY = 0;
    
    for (int y = y1; y < y2; y++)
    {
        for (int x = x1; x < x2; x++)
        {
            COLORREF pixel = GetPixel(hdcMem, x, y);

            if (IsPixelYellow(pixel))
            {
                targetX = x;
                targetY = y;

                //std::cout << "Yellow pixel found at (" << x << ", " << y << ")\n";

                SelectObject(hdcMem, hBitmapOld);
                DeleteObject(hBitmap);
                DeleteDC(hdcMem);
                ReleaseDC(NULL, hdcScreen);

                return std::pair<int, int>(targetX, targetY);
            }
        }
    }

    SelectObject(hdcMem, hBitmapOld);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    return std::pair<int, int>(targetX, targetY);
}

void SetConsoleColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

int main()
{
    POINT pset1;
    POINT pset2;

    double DistanseSet;
    int nset;

    int CorrectionAdd = 0;
    int CorrectionPercent = 1;   //дефолтная корекция 

    const int amendmentX = 0;   
    const int amendmentY = 0;    //поправка для метки тут

    double DistanseMap;
    double DistanseMaptarget;

    POINT ptarget;
    POINT pyou;

    std::cout << "set default distanse : ";
    std::cin >> DistanseSet;
    std::cout << "set number of cells: ";
    std::cin >> nset;
    std::cout << std::endl;
    DistanseSet = DistanseSet * nset * sqrt(2);

    while (true)
    {
        int t = 0;
        std::cout << std::endl;
        std::cin >> t;

        if (t == 1)
        {
            GetCursorPos(&pset1);
            std::cout << "first position seted (" << pset1.x << "," << pset1.y << ")\n";
        }

        if (t == 2)
        {
            GetCursorPos(&pset2);
            std::cout << "second position seted (" << pset2.x << "," << pset2.y << ")\n";
            DistanseMap = sqrt((pset2.x - pset1.x) * (pset2.x - pset1.x) + (pset2.y - pset1.y) * (pset2.y - pset1.y));
        }

        if (t == 3)
        {
            GetCursorPos(&pyou);
            std::cout << "your position seted (" << pyou.x << "," << pyou.y <<")\n";

        }

        if (t == 4)
        {
            std::cout << "tracking mode ON\n";
            while (true)
            {
                if (GetAsyncKeyState('M') & 0x8000)
                {
                   Sleep(250);
                   std::cout << "-------------(start)" << std::endl;

                   auto Target = SearchTatget(pset1.x , pset1.y, pset2.x, pset2.y);
                   POINT ptarget;
                   ptarget.x = Target.first;
                   ptarget.y = Target.second;
                   if (ptarget.x == 0  && ptarget.y == 0)
                   {
                       SetConsoleColor(FOREGROUND_RED);
                       std::cout << "not found " << std::endl;
                       SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

                       std::cout << "-------------(end)" << std::endl;
                   }
                   else
                   {
                       ptarget.x = ptarget.x + amendmentX;
                       ptarget.y = ptarget.y + amendmentY;

                       std::cout << "target at (" << ptarget.x << ", " << ptarget.y << ")\n";

                       DistanseMaptarget = sqrt((ptarget.x - pyou.x) * (ptarget.x - pyou.x) + (ptarget.y - pyou.y) * (ptarget.y - pyou.y));
                       double s = DistanseMaptarget / DistanseMap * DistanseSet * CorrectionPercent + CorrectionAdd;

                       std::cout << "distanse: ";
                       SetConsoleColor(FOREGROUND_GREEN);
                       std::cout << s << std::endl;
                       SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

                       double Azim = acos((pyou.y - ptarget.y) / (1 * DistanseMaptarget)) / 3.141592 * 180;
                       if (ptarget.x <= pyou.x)
                       {
                           Azim = -Azim + 360;
                       }

                       std::cout << "azim: ";
                       SetConsoleColor(FOREGROUND_GREEN);
                       std::cout << Azim << std::endl;
                       SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

                       std::cout << "-------------(end)" << std::endl;
                       Sleep(250);
                   }
                }
                if (GetAsyncKeyState(VK_RCONTROL))
                {
                    std::cout << "tracking mode OFF\n";
                    break;
                }
            }
        }

        if (t == 5)
        {
            std::cout << "set default distanse : ";
            std::cin >> DistanseSet;
            std::cout << "set number of cells : ";
            std::cin >> nset;
            std::cout << std::endl;
            DistanseSet = DistanseSet * nset;
        }

        if (t == 6)
        {
            std::cout << "set Correction in percent : ";
            std::cin >> CorrectionPercent;
            std::cout << "set additional Correction : ";
            std::cin >> CorrectionAdd;
        }

    }
}