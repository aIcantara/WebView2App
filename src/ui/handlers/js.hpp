#ifndef JS_HPP
#define JS_HPP

namespace handlers::js
{
    HWND hWnd;

    inline HRESULT onMessage(ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args)
    {
        wil::unique_cotaskmem_string raw;
        
        args->get_WebMessageAsJson(&raw);

        auto message = json::value::parse(raw.get());

        if (!message.has_field(U("action")))
            return S_OK;

        std::string action = utility::conversions::to_utf8string(message.at(U("action")).as_string());

        if (action == "exit")
        {
            std::exit(ERROR_SUCCESS);
        }
        else if (action == "minimize")
        {
            if (hWnd)
                ShowWindow(hWnd, SW_MINIMIZE);
        }
        else if (action == "hello")
        {
            if (message.has_field(U("text")))
            {
                std::string text = utility::conversions::to_utf8string(message.at(U("text")).as_string());

                std::string response = "Hello, " + text;

                webview->PostWebMessageAsString(utility::conversions::to_string_t(response).c_str());
            }
        }

        return S_OK;
    }
} // namespace handlers::js

#endif // JS_HPP