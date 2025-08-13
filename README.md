# Install

**1. Requirements**
- **[vcpkg](https://github.com/microsoft/vcpkg)** (add in `PATH`)
- **[NuGet](https://www.nuget.org/downloads)** (add in `PATH`)

**2. Installing cpprest sdk**
```cmd
vcpkg install cpprestsdk:x86-windows-static
```

**3. Cloning a repository**
```cmd
git clone https://github.com/aIcantara/WebView2App.git
cd WebView2App
```

**4. CMakeLists**

Change it to your own path
```cmake
set(VCPKG_ROOT "C:/vcpkg")
```

**5. Building**
```cmd
cmake -B build -A Win32
cmake --build build --config Release
```

# Vue.js
**1. Project**
```cmd
npm init vue@latest
```

**2. Config**

vite.config.js
```js
import { fileURLToPath, URL } from "node:url"

import { defineConfig } from "vite"

import vue from "@vitejs/plugin-vue"
import vueDevTools from "vite-plugin-vue-devtools"

export default defineConfig({
    base: "res://",
    plugins: [
        vue(),
        vueDevTools()
    ],
    resolve: {
        alias: {
            "@": fileURLToPath(new URL("./src", import.meta.url))
        }
    }
});
```
