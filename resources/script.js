import { engine } from "res://js/engine.js"

document.addEventListener("DOMContentLoaded", () => {
    document.querySelector(".header-controls-minimize").onclick = () => engine.app.minimize();
    document.querySelector(".header-controls-exit").onclick = () => engine.app.exit();
    
    document.querySelector(".hello").onclick = () => engine.test.hello("World").then(response => alert(response));
});