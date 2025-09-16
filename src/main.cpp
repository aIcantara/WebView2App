#include "ui/ui.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    ui::handlers::js::registerMessage("hello",
        [](ICoreWebView2* webview, const web::json::value& message) -> HRESULT
        {
            if (message.has_field(U("text")))
            {
                std::string text = utility::conversions::to_utf8string(message.at(U("text")).as_string());
                std::string response = "Hello, " + text;

                webview->PostWebMessageAsString(utility::conversions::to_string_t(response).c_str());
            }

            return S_OK;
        }
    );

    ui::stOptions options;
    options.devTools = true;
    options.borderRadius = 15;

    ui::CWindow window("WebView2App", { 700, 500 }, options);

    return 0;
}