#ifndef WINDOW_H
#define WINDOW_H

#define WM_WEBVIEW_REQUEST WM_USER + 0x01

#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

#include <wrl.h>

#include <wil/com.h>

#include <cpprest/json.h>

#include <cmrc/cmrc.hpp>

namespace ui
{
    struct stSize
    {
        int width;
        int height;
    };

    struct stOptions
    {
        int borderRadius = 0;
        bool devTools = false;
        bool transparentBackground = false;
        bool registerDefaultMessages = true;
        std::string browserArguments = "";
        std::string url = "";        
    };

    class CWindow
    {
    public:
        CWindow(const char* name, stSize size, stOptions options = {});

    private:
        HWND hWnd;
        stOptions options;

        wil::com_ptr<ICoreWebView2> webview;
        wil::com_ptr<ICoreWebView2Controller> webviewController;

        bool initialize();

        static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    };
} // namespace ui

#endif // WINDOW_H