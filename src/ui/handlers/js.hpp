#ifndef JS_HPP
#define JS_HPP

namespace ui::handlers::js
{
    using registerMessageFunc = std::function<HRESULT(ICoreWebView2*, const web::json::value&)>;

    inline std::unordered_map<std::string, registerMessageFunc> handlers;

    inline void registerMessage(const std::string& action, registerMessageFunc func)
    {
        handlers[action] = func;
    }

    inline HRESULT onMessage(ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args)
    {
        wil::unique_cotaskmem_string raw;

        args->get_WebMessageAsJson(&raw);

        auto message = web::json::value::parse(raw.get());

        if (!message.has_field(U("action")))
            return S_OK;

        std::string action = utility::conversions::to_utf8string(message.at(U("action")).as_string());

        auto it = handlers.find(action);

        if (it != handlers.end())
            return it->second(webview, message);

        return S_OK;
    }
} // namespace ui::handlers::js

#endif // JS_HPP