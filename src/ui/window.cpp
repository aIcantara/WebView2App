#include "window.h"
#include "handlers/resource.hpp"
#include "handlers/js.hpp"

#include <windows.h>
#include <shlobj.h>

CWindow::CWindow(const char* name, stSize size)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wndProc;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = name;
    wc.hIcon = LoadIcon(wc.hInstance, "APP_ICON");
    wc.hIconSm = LoadIcon(wc.hInstance, "APP_ICON");

    RegisterClassExA(&wc);

    int windowX = (GetSystemMetrics(SM_CXSCREEN) - size.width) / 2;
    int windowY = (GetSystemMetrics(SM_CYSCREEN) - size.height) / 2;

    hWnd = CreateWindowA(
        name,
        name,
        WS_POPUP,
        windowX,
        windowY,
        size.width,
        size.height,
        nullptr,
        nullptr,
        GetModuleHandleA(nullptr),
        nullptr
    );

    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    handlers::js::hWnd = hWnd;

    if (initialize())
    {
        MSG message;

        while (GetMessage(&message, nullptr, 0, 0))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
}

bool CWindow::initialize()
{
    static bool initialized = false;

    if (initialized)
        return true;

    char userdata[MAX_PATH];

    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, userdata);

    auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();

    options->put_AdditionalBrowserArguments(L"--enable-features=msWebView2EnableDraggableRegions --disable-web-security --disk-cache-size=1 --media-cache-size=1");

    CreateCoreWebView2EnvironmentWithOptions(nullptr, std::wstring(std::begin(userdata), std::end(userdata)).c_str(), options.Get(),
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>([&](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
            {
                if (FAILED(result) || env == nullptr)
                    return S_OK;

                initialized = true;

                env->CreateCoreWebView2Controller(hWnd,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>([&](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT
                        {
                            webviewController = controller;

                            webviewController->get_CoreWebView2(&webview);

                            wil::com_ptr<ICoreWebView2Settings> settings;

                            webview->get_Settings(&settings);

                            settings->put_AreDevToolsEnabled(false);
                            settings->put_AreDefaultContextMenusEnabled(false);

                            webview->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(handlers::js::onMessage).Get(), nullptr);

                            webview->AddWebResourceRequestedFilter(L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
                            webview->add_WebResourceRequested(Callback<ICoreWebView2WebResourceRequestedEventHandler>(handlers::resource::onResource).Get(), nullptr);

                            RECT bounds;

                            GetClientRect(hWnd, &bounds);

                            webviewController->put_Bounds(bounds);

                            // transparent background
                            // webviewController.query<ICoreWebView2Controller2>()->put_DefaultBackgroundColor({ 0 });

                            auto ef = cmrc::WebResources::get_filesystem();

                            auto index = ef.open("index.html");
                            std::string content(index.begin(), index.end());

                            std::wstring wideContent(content.begin(), content.end());

                            webview->NavigateToString(wideContent.c_str());

                            // link view
                            // webview->Navigate(L"http://localhost:8080");

                            return S_OK;
                        }
                    ).Get());

                return S_OK;
            }
        ).Get());

    return initialized;
}

LRESULT CALLBACK CWindow::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindow* window = reinterpret_cast<CWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if (!window && uMsg != WM_CREATE)
        return DefWindowProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
    case WM_SIZE:
    {
        if (window->webviewController)
        {
            RECT bounds;

            GetClientRect(hWnd, &bounds);

            window->webviewController->put_Bounds(bounds);
        }

        break;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }

    case WM_WEBVIEW_REQUEST:
    {
        auto requestPtr = reinterpret_cast<std::shared_ptr<std::string>*>(lParam);

        if (requestPtr)
        {
            std::string request = **requestPtr;

            requestPtr->reset();

            wil::com_ptr<ICoreWebView2> webview;

            window->webviewController->get_CoreWebView2(&webview);

            if (webview)
            {
                std::wstring wrequest(request.begin(), request.end());

                webview->ExecuteScript(wrequest.c_str(), nullptr);
            }
        }

        break;
    }

    default:
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    }

    return 0;
}