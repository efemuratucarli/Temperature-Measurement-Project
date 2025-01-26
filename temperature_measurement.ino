#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0
#include "ESP8266TimerInterrupt.h"

// Analog pin
const int analogInPin = A0;

const char *ssid = "ESP8266 Access Point"; // The name of the Wi-Fi network that will be created
const char *password = "1122334455";

// Select a Timer Clock
#define USING_TIM_DIV1 false // for shortest and most accurate timer
#define USING_TIM_DIV16 false // for medium time and medium accurate timer
#define USING_TIM_DIV256 true // for longest timer but least accurate. Default
ESP8266Timer ITimer;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// HTML WebPage
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
html {
  font-family: Arial;
  display: inline-block;
  margin: 0px auto;
  text-align: center;
}
h2 { font-size: 3.0rem; }
p { font-size: 3.0rem; }
.units { font-size: 1.2rem; }
.sensor-labels {
  font-size: 1.5rem;
  vertical-align: middle;
  padding-bottom: 15px;
}
canvas {
  display: block;
  margin: 20px auto;
  border: 1px solid black;
}
</style>
</head>
<body>
<h2>Sıcaklık Zaman Grafiği</h2>
<p>
<span class="sensor-labels">Son Sıcaklık:</span> <span id="adc">--</span>°C
</p>
<canvas id="my-canvas" width="600" height="400"></canvas>
<script>
const canvas = document.querySelector('#my-canvas');
const ctx = canvas.getContext('2d');
const WIDTH = canvas.width;
const HEIGHT = canvas.height;
const PADDING = 50;
const graphWidth = WIDTH - 2 * PADDING;
const graphHeight = HEIGHT - 2 * PADDING;

let tempData = [];
const maxPoints = 10;

function drawAxes() {
    ctx.clearRect(0, 0, WIDTH, HEIGHT);

    ctx.strokeStyle = "black";
    ctx.lineWidth = 2;

    ctx.beginPath();
    ctx.moveTo(PADDING, HEIGHT - PADDING);
    ctx.lineTo(WIDTH - PADDING, HEIGHT - PADDING);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(PADDING, HEIGHT - PADDING);
    ctx.lineTo(PADDING, PADDING);
    ctx.stroke();
}

function drawGraph() {
    drawAxes();

    if (tempData.length === 0) return;

    ctx.strokeStyle = "blue";
    ctx.lineWidth = 2;
    ctx.fillStyle = "black";
    ctx.font = "10px Arial";

    ctx.beginPath();
    for (let i = 0; i < tempData.length; i++) {
        const x = PADDING + (i * graphWidth) / (maxPoints - 1);
        const y = HEIGHT - PADDING - (tempData[i] * graphHeight) / 100;
        if (i === 0) {
            ctx.moveTo(x, y);
        } else {
            ctx.lineTo(x, y);
        }
        ctx.fillText(`${tempData[i].toFixed(3)}°C`, x - 10, y + 10); // Sıcaklık değerini yaz
    }
    ctx.stroke();
}

function convertToTemperature(digitalValue) {
    const adcMax = 1023;      // ADC'nin maksimum değeri
    const refVoltage = 3.3;  // Referans voltajı (3.3V)

    // Dijital değeri voltaj değerine dönüştür
    const voltage = (digitalValue / adcMax) * refVoltage;

    const temperature = voltage / 0.05;

    return temperature;
}

setInterval(() => {
    const xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            const veri = this.responseText;
            const nums = veri.split(',').map(x => +x);

            console.log("nums dizisi: ", nums);

            const temperature = convertToTemperature(nums[0]);
            document.getElementById("adc").innerText = temperature;
            tempData.push(temperature);
            if (tempData.length > maxPoints) {
                tempData.shift();
            }

            drawGraph();
        }
    };
    xhttp.open("GET", "/adc", true);
    xhttp.send();
}, 500);
</script>
</body>
</html>
)rawliteral";

// //----- Örnekleme frakansını belirleme -----------------------------------------------
#define TIMER_INTERVAL_MS 5

// // Değişkenler
int sayac = 0;
String bak1 = "";
char Tampon[10000];
const char *bak;
int gonder = 0;

void IRAM_ATTR TimerHandler() {
  int analogValue = analogRead(analogInPin);

  String tempBak1 = bak1; // Geçici değişken
  tempBak1 += String(analogValue) + ","; // Yeni veri ekle
  bak1 = tempBak1;
  sayac++;
  if (sayac > 10) { // 10 veri biriktikten sonra işlem yap
    sayac = 0;
    tempBak1.toCharArray((char *)Tampon, tempBak1.length() + 1);
    gonder = 1; // Gönderim işareti
    Serial.println("Veri Gönderildi");
    bak1 = ""; // Tamponu sıfırla
  }
}

void setup() {
  // put your setup code here, to run once:
  bak1.toCharArray(Tampon, bak1.length()+1);
  bak = Tampon;
  // Serial port for debugging purposes
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // HTML sayfasını "/" endpoint'ine bağla
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // Analog veriyi "/adc" endpoint'ine bağla
  server.on("/adc", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", bak);
  });

  // Web sunucusunu başlat
  server.begin();
  delay(10000);

  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler)) {
  Serial.print(F("Starting ITimer OK, millis() = "));
  }
  else
  Serial.println(F("Can't set ITimer correctly. Select another freq. or interval"));
  }

void loop() {
  // put your main code here, to run repeatedly:
  if (gonder == 1) {
    gonder = 0; // İşareti sıfırla
    Serial.println(Tampon); // Tampondaki veriyi seri monitöre yazdır
  }

}