#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <shlwapi.h>
#include <shlobj_core.h>

#pragma comment(lib, "shlwapi.lib")

CMRC_DECLARE(WebResources);

namespace ui::handlers::resource
{
    inline std::string getMime(const std::string& path)
    {
        static const std::unordered_map<std::string, std::string> mimeMap =
        {
            // images
            { "gif", "image/gif" },
            { "svg", "image/svg+xml" },
            { "png", "image/png" },
            { "jpg", "image/jpeg" },
            { "jpeg", "image/jpeg" },
            { "webp", "image/webp" },

            // video
            { "mp4", "video/mp4" },
            { "avi", "video/x-msvideo" },
            { "mov", "video/quicktime" },
            { "mkv", "video/x-matroska" },
            { "webm", "video/webm" },
            { "wmv", "video/x-ms-wmv" },
            { "mpeg", "video/mpeg" },
            { "mpg", "video/mpeg" },

            // text
            { "txt", "text/plain" },
            { "xml", "application/xml" },
            { "json", "application/json" },
            { "css", "text/css" },
            { "js", "application/javascript" },
            { "html", "text/html" },

            // audio
            { "mp3", "audio/mpeg" },
            { "aac", "audio/aac" },
            { "ogg", "audio/ogg" },
            { "flac", "audio/flac" },
            { "wma", "audio/x-ms-wma" },
            { "alac", "audio/alac" },
            { "opus", "audio/opus" },
            { "wav", "audio/wav" },
            { "aiff", "audio/aiff" },
            { "m4a", "audio/mp4" },
            { "webm", "audio/webm" },

            // fonts
            { "ttf", "font/truetype" },
            { "otf", "application/font-otf" },
            { "eot", "application/vnd.ms-fontobject" },
            { "woff", "font/woff" },
            { "woff2", "font/woff2" },
            { "sfnt", "font/sfnt" },
            { "woff3", "font/woff3" }
        };

        auto pos = path.find_last_of('.');

        if (pos == std::string::npos)
            return "application/octet-stream";

        std::string extension = path.substr(pos + 1);

        for (auto& c : extension)
            c = static_cast<char>(tolower(c));

        auto it = mimeMap.find(extension);

        if (it != mimeMap.end())
            return it->second;

        return "application/octet-stream";
    }

    inline HRESULT onResource(ICoreWebView2* webview, ICoreWebView2WebResourceRequestedEventArgs* args)
    {
        wil::com_ptr<ICoreWebView2WebResourceRequest> request;

        args->get_Request(&request);

        wil::unique_cotaskmem_string uri;

        request->get_Uri(&uri);

        std::string source = utility::conversions::to_utf8string(uri.get());

        auto prefix = source.find("res://");

        if (prefix != std::string::npos)
        {
            auto ef = cmrc::WebResources::get_filesystem();

            source = source.substr(prefix + 6);

            if (ef.exists(source))
            {
                auto file = ef.open(source);
                std::string content(file.begin(), file.end());

                wil::com_ptr<IStream> stream(SHCreateMemStream(reinterpret_cast<const BYTE*>(content.data()), content.size()));

                wil::com_ptr<ICoreWebView2_2> webview2;

                webview->QueryInterface(IID_PPV_ARGS(&webview2));

                wil::com_ptr<ICoreWebView2Environment> environment;

                webview2->get_Environment(&environment);

                std::string mime = getMime(source);

                std::wstring wMime(mime.begin(), mime.end());

                std::wstring status = L"OK";

                wil::com_ptr<ICoreWebView2WebResourceResponse> response;

                environment->CreateWebResourceResponse(stream.get(), 200, status.c_str(), wMime.c_str(), &response);

                wil::com_ptr<ICoreWebView2HttpResponseHeaders> headers;

                response->get_Headers(&headers);

                std::wstring contentLength = std::to_wstring(content.size());

                headers->AppendHeader(L"Content-Type", wMime.c_str());
                headers->AppendHeader(L"Content-Length", contentLength.c_str());
                headers->AppendHeader(L"Access-Control-Allow-Origin", L"*");

                args->put_Response(response.get());
            }
        }

        return S_OK;
    }
} // namespace ui::handlers::resource

#endif // RESOURCE_HPP