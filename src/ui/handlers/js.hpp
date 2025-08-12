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

        auto action = message.at(U("action")).as_string();

        std::string actionStr = utility::conversions::to_utf8string(action);

        if (actionStr == "exit")
        {
            std::exit(ERROR_SUCCESS);
        }
        else if (actionStr == "minimize")
        {
            if (hWnd)
                ShowWindow(hWnd, SW_MINIMIZE);
        }
        else if (actionStr == "hello")
        {
            if (message.has_field(U("text")))
            {
                auto textVal = message.at(U("text")).as_string();
                std::string text = utility::conversions::to_utf8string(textVal);

                std::string response = "Hello, " + text;
                std::wstring wresponse(response.begin(), response.end());

                webview->PostWebMessageAsString(wresponse.c_str());
            }
        }

        return S_OK;
    }
} // handlers::js

#endif // JS_HPP