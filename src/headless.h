#pragma once
#include "global.hpp"

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Adafruit_SSD1306.h>

AsyncWebServer server(80);
static uint8_t display_copy[(128 * 64) / 8];

void Headless_Thread(void *pvParameters)
{
    while (true)
    {
        memcpy(display_copy, display.getBuffer(), sizeof(display_copy));
        vTaskDelay(33 / portTICK_PERIOD_MS);
    }
}

void Headless_Setup(bool on)
{
    if (!on) return;

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("Pocket Puter", "deveclipse");
    MDNS.begin("pocketputer");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>Pocket Puter Display</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no, maximum-scale=1.0">
<style>
html, body {
  margin: 0;
  padding: 0;
  background: black;
  overflow: hidden;
  height: 100%;
  touch-action: none;
  -webkit-user-select: none;
  user-select: none;
}
body {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
}
canvas {
  background: black;
  image-rendering: pixelated;
  width: 100vw;
  height: calc(100vw * 0.5);
  max-height: 90vh;
  max-width: calc(90vh * 2);
  border: 2px solid #444;
  border-radius: 10px;
  touch-action: none;
}
.buttons {
  display: flex;
  justify-content: space-around;
  width: 100%;
  max-width: 400px;
  margin-top: 15px;
}
button {
  flex: 1;
  margin: 5px;
  padding: 15px;
  font-size: 20px;
  background: #222;
  color: white;
  border: 2px solid #555;
  border-radius: 10px;
  outline: none;
  touch-action: manipulation;
  -webkit-user-select: none;
  user-select: none;
}
button:active {
  background: #0f0;
  color: black;
}
#refreshBtn {
  position: fixed;
  top: 10px;
  right: 10px;
  width: 40px;
  height: 40px;
  background: #222;
  border: 1px solid #666;
  border-radius: 50%;
  color: white;
  font-size: 22px;
  display: flex;
  align-items: center;
  justify-content: center;
  line-height: 1;
  opacity: 0.6;
  transition: opacity 0.2s, background 0.2s;
}
#refreshBtn:hover {
  opacity: 1;
  background: #0ff;
  color: black;
}
</style>
</head>
<body>
<canvas id="disp" width="128" height="64"></canvas>
<div class="buttons">
  <button ontouchstart="press(0)" ontouchend="release(0)" onmousedown="press(0)" onmouseup="release(0)">Left</button>
  <button ontouchstart="press(1)" ontouchend="release(1)" onmousedown="press(1)" onmouseup="release(1)">Center</button>
  <button ontouchstart="press(2)" ontouchend="release(2)" onmousedown="press(2)" onmouseup="release(2)">Right</button>
</div>
<button id="refreshBtn" onclick="location.reload()">⟳</button>
<script>
document.addEventListener('gesturestart', e => e.preventDefault());
document.addEventListener('gesturechange', e => e.preventDefault());
document.addEventListener('gestureend', e => e.preventDefault());
document.addEventListener('dblclick', e => e.preventDefault());

const c = document.getElementById('disp');
const ctx = c.getContext('2d', {alpha:false});
const imgData = new ImageData(128,64);
let failCount = 0;

async function update() {
  try {
    const res = await fetch('/buffer');
    if (!res.ok) throw new Error("Bad response");
    const buf = new Uint8Array(await res.arrayBuffer());
    failCount = 0;

    for (let page = 0; page < 8; page++) {
      for (let x = 0; x < 128; x++) {
        const byte = buf[page * 128 + x];
        for (let bit = 0; bit < 8; bit++) {
          const y = page * 8 + bit;
          const v = (byte >> bit) & 1 ? 255 : 0;
          const i = (y * 128 + x) * 4;
          imgData.data[i] = v;
          imgData.data[i+1] = v;
          imgData.data[i+2] = v;
          imgData.data[i+3] = 255;
        }
      }
    }

    // Create bitmap and draw once — prevents flicker
    const bmp = await createImageBitmap(imgData);
    ctx.drawImage(bmp, 0, 0);
  } catch(e) {
    failCount++;
    if (failCount > 30) location.reload();
  }
}
setInterval(update, 33); // ~30fps

function press(id){ fetch(`/btn?id=${id}&state=1`); }
function release(id){ fetch(`/btn?id=${id}&state=0`); }
</script>
</body>
</html>
)rawliteral");
    });

    server.on("/buffer", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(
            200, "application/octet-stream", display_copy, sizeof(display_copy));
        request->send(response);
    });

    server.on("/btn", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!request->hasParam("id") || !request->hasParam("state")) {
            request->send(400, "text/plain", "Missing params");
            return;
        }
        int id = request->getParam("id")->value().toInt();
        int state = request->getParam("state")->value().toInt();
        bool pressed = (state == 1);

        if (id == 0) virtualButtons.left = pressed;
        else if (id == 1) virtualButtons.center = pressed;
        else if (id == 2) virtualButtons.right = pressed;

        request->send(200, "text/plain", "OK");
    });

    server.begin();

    xTaskCreatePinnedToCore(
        Headless_Thread,
        "Headless",
        4096,
        NULL,
        1,
        NULL,
        1
    );
}
