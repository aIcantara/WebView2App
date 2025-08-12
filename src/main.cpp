#include "ui/window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    CWindow window("WebView2App", { 700, 500 });

    return 0;
}