function call(data) {
    window.chrome.webview.postMessage(data);
}

function callWithResponse(data) {
    return new Promise((resolve) => {
        function handler(event) {
            window.chrome.webview.removeEventListener("message", handler);

            resolve(event.data);
        }

        window.chrome.webview.addEventListener("message", handler);

        call(data);
    });
}

export const engine = {
    app: {
        exit() {
            call({ action: "exit" });
        },

        minimize() {
            call({ action: "minimize" });
        }
    },

    test: {
        hello(text) {
            return callWithResponse({ action: "hello", text: text });
        }
    }
};