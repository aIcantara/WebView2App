#ifndef WINDOW_H
#define WINDOW_H

#define WM_WEBVIEW_REQUEST WM_USER + 0x01

#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

#include <wrl.h>

#include <wil/com.h>

#include <cpprest/json.h>

#include <cmrc/cmrc.hpp>

using namespace Microsoft::WRL;
using namespace web;

struct stSize
{
    int width;
    int height;
};

class CWindow
{
public:
    CWindow(const char* name, stSize size);

private:
    HWND hWnd;

    wil::com_ptr<ICoreWebView2> webview;
    wil::com_ptr<ICoreWebView2Controller> webviewController;

    bool initialize();

    static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // WINDOW_H