# Install

**1. Requirements**
- **[vcpkg](https://github.com/microsoft/vcpkg)** (add in `PATH`)
- **[NuGet](https://www.nuget.org/downloads)** (add in `PATH`)
- **[Node.js](https://nodejs.org) + Vue CLI** (`npm install -g @vue/cli`) (if using vue.js)

**2. Installing cpprest sdk**
```
vcpkg install cpprestsdk:x86-windows-static
```

**3. Cloning a repository**
```
git clone https://github.com/aIcantara/WebView2App.git
cd WebView2App
```

**4. Building**
```
cmake -B build -A Win32
cmake --build build --config Release
```

# Vue.js
vue.config.js:
```js
const { defineConfig } = require("@vue/cli-service");

module.exports = defineConfig({
    transpileDependencies: true,
    publicPath: "res://"
})
```