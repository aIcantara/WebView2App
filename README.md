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
**1. Requirements**
- **[Node.js](https://nodejs.org) + Vue CLI** (`npm install -g @vue/cli`)

**2. Project**
```cmd
vue create ui
cd ui
npm run build
```

**3. Config**

vue.config.js
```js
const { defineConfig } = require("@vue/cli-service");

module.exports = defineConfig({
    transpileDependencies: true,
    publicPath: "res://"
})
```