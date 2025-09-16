#include "window.h"
#include "handlers/resource.hpp"
#include "handlers/js.hpp"

#include <windows.h>
#include <shlobj.h>

using namespace ui;

CWindow::CWindow(const char* name, stSize size, stOptions options) : options(options)
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

    if (options.borderRadius > 0)
        SetWindowRgn(hWnd, CreateRoundRectRgn(0, 0, size.width + 1, size.height + 1, options.borderRadius, options.borderRadius), TRUE);

    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    if (options.registerDefaultMessages)
    {
        ui::handlers::js::registerMessage("exit", [](ICoreWebView2*, const web::json::value&) -> HRESULT
            {
                std::exit(ERROR_SUCCESS);

                return S_OK;
            }
        );

        ui::handlers::js::registerMessage("minimize", [&](ICoreWebView2*, const web::json::value&) -> HRESULT
            {
                ShowWindow(hWnd, SW_MINIMIZE);

                return S_OK;
            }
        );
    }

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
    std::wstring args(this->options.browserArguments.begin(), this->options.browserArguments.end());
    std::wstring fullArgs = L"--enable-features=msWebView2EnableDraggableRegions --disable-web-security --disk-cache-size=1 --media-cache-size=1 " + args;

    options->put_AdditionalBrowserArguments(fullArgs.c_str());

    CreateCoreWebView2EnvironmentWithOptions(nullptr, std::wstring(std::begin(userdata), std::end(userdata)).c_str(), options.Get(),
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>([&](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
            {
                if (FAILED(result) || env == nullptr)
                    return S_OK;

                initialized = true;

                env->CreateCoreWebView2Controller(hWnd,
                    Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>([&](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT
                        {
                            webviewController = controller;

                            webviewController->get_CoreWebView2(&webview);

                            wil::com_ptr<ICoreWebView2Settings> settings;

                            webview->get_Settings(&settings);

                            settings->put_AreDevToolsEnabled(this->options.devTools);
                            settings->put_AreDefaultContextMenusEnabled(this->options.devTools);

                            webview->add_WebMessageReceived(Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(handlers::js::onMessage).Get(), nullptr);

                            webview->AddWebResourceRequestedFilter(L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
                            webview->add_WebResourceRequested(Microsoft::WRL::Callback<ICoreWebView2WebResourceRequestedEventHandler>(handlers::resource::onResource).Get(), nullptr);

                            RECT bounds;

                            GetClientRect(hWnd, &bounds);

                            webviewController->put_Bounds(bounds);

                            if (this->options.transparentBackground)
                                webviewController.query<ICoreWebView2Controller2>()->put_DefaultBackgroundColor({ 0 });

                            if (this->options.url.empty())
                            {
                                auto index = cmrc::WebResources::get_filesystem().open("index.html");
                                std::wstring content(index.begin(), index.end());

                                webview->NavigateToString(content.c_str());
                            }
                            else
                            {
                                std::wstring url(this->options.url.begin(), this->options.url.end());

                                webview->Navigate(url.c_str());
                            }

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
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}