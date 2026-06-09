#ifndef RED_H
#define RED_H

#include "Config.h"
#include "Alarms.h"
#include "Interfaz.h"
#include "Scores.h"

extern bool deleteAlarma(String nombre);
extern void stopAlarm(bool snooze);
extern void updateWeather();
extern uint32_t alarmNeoColor;

const char GAMES_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Arcade Control</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { background: #111; color: #0ff; font-family: 'Segoe UI', sans-serif; overflow-x: hidden; user-select: none; -webkit-user-select: none; height: 100vh; display: flex; flex-direction: column; }
        .header { text-align: center; padding: 10px; background: #222; font-size: 20px; font-weight: bold; border-bottom: 2px solid #0ff; display: flex; justify-content: space-between; align-items: center; flex-shrink: 0; }
        .header button { padding: 8px 15px; background: #f0f; color: #fff; border: none; font-weight: bold; border-radius: 5px; cursor: pointer; }
        .content { flex: 1; overflow-y: auto; overflow-x: hidden; padding-bottom: 20px; }
        .game-selector { display: flex; flex-wrap: wrap; justify-content: center; gap: 10px; padding: 10px; background: #1a1a1a; }
        .game-selector button { flex: 1 1 calc(50% - 10px); min-width: 130px; padding: 14px; background: #333; color: #fff; border: 1px solid #0ff; font-weight: bold; font-size: 14px; cursor: pointer; border-radius: 5px; }
        .game-selector button:active { background: #0ff; color: #000; }
        .score-buttons { display: flex; flex-wrap: wrap; justify-content: center; gap: 8px; padding: 10px; background: #0a0a0a; }
        .score-buttons button { flex: 0 1 calc(50% - 4px); padding: 10px; background: #1a3a1a; color: #0f0; border: 1px solid #0f0; font-weight: bold; font-size: 12px; cursor: pointer; border-radius: 5px; }
        .score-buttons button:active { background: #0f0; color: #000; }
        .controls { display: flex; justify-content: center; gap: 28px; padding: 18px; background: #1a1a1a; flex-wrap: wrap; }
        .dpad { display: flex; gap: 10px; }
        .btn { width: 78px; height: 78px; min-width: 64px; min-height: 64px; background: rgba(0, 255, 255, 0.2); border: 2px solid #0ff; border-radius: 15px; display: flex; justify-content: center; align-items: center; font-size: 30px; color: #fff; font-weight: bold; touch-action: manipulation; }
        .btn:active { background: rgba(0, 255, 255, 0.8); color: #000; transform: scale(0.95); }
        .action-btns { display: flex; gap: 10px; }
        .action-btn { width: 82px; height: 82px; min-width: 68px; min-height: 68px; background: rgba(255, 0, 255, 0.2); border: 2px solid #f0f; border-radius: 50%; display: flex; justify-content: center; align-items: center; font-size: 14px; color: #fff; font-weight: bold; touch-action: manipulation; text-align: center; }
        .action-btn:active { background: rgba(255, 0, 255, 0.8); color: #000; transform: scale(0.95); }
        #scoresData { color: #0f0; background: #0a0a0a; border: 1px solid #0f0; border-radius: 8px; min-height: 80px; padding: 10px; margin: 10px; white-space: pre-wrap; word-break: break-all; font-size: 12px; }
    </style>
</head>
<body>
    <div class="header">
        <button onclick="window.location.href='/'">ATRÁS</button>
        ARCADE
        <div style="width:50px"></div>
    </div>
    <div class="content">
        <div class="game-selector">
            <button onclick="startGame('minijuego')">▶ Flappy</button>
            <button onclick="startGame('asteroids')">▶ Asteroids</button>
            <button onclick="startGame('runner')">▶ Runner</button>
            </div>
        <div class="score-buttons">
            <button onclick="showScores('flappy')">🏆 Flappy</button>
            <button onclick="showScores('aster')">🏆 Asteroids</button>
            <button onclick="showScores('runner')">🏆 Runner</button>
        </div>
        <div class="controls">
            <div class="dpad">
                <div class="btn" onpointerdown="p(1,1)" onpointerup="p(1,0)" onpointercancel="p(1,0)">◀</div>
                <div class="btn" onpointerdown="p(2,1)" onpointerup="p(2,0)" onpointercancel="p(2,0)">▶</div>
            </div>
            <div class="action-btns">
                <div class="action-btn" onpointerdown="f(3)">B3<br>Act</div>
                <div class="action-btn" onpointerdown="f(1)" style="border-color:#0f0; background:rgba(0,255,0,0.2);">B1<br>Sal</div>
            </div>
        </div>
        <pre id="scoresData"></pre>
    </div>
    <script>
        function startGame(g) { fetch('/api/startgame?game=' + g); }
        function showScores(g) { fetch('/api/scores?game=' + g).then(r=>r.text()).then(t=>document.getElementById('scoresData').innerText = t); }
        function p(b, s) { fetch('/api/btn?id='+b+'&state='+s); if(s==1) fetch('/api/btnImm?id='+b); }
        function f(b) { fetch('/api/btnImm?id='+b); }
        document.addEventListener('contextmenu', e => e.preventDefault());
    </script>
</body>
</html>
)rawliteral";

const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 Control Center</title>
    <style>
        :root { --bg: #121212; --card: #1e1e1e; --primary: #00adb5; --accent: #ff4c4c; --text: #e0e0e0; }
        body { font-family: 'Segoe UI', sans-serif; background: var(--bg); color: var(--text); margin: 0; padding: 10px; }
        .container { max-width: 600px; margin: 0 auto; }
        .card { background: var(--card); border-radius: 12px; padding: 15px; margin-bottom: 15px; border-left: 5px solid var(--primary); text-align: left; }
        h1, h3 { color: var(--primary); margin-top: 0; text-align: center; }
        input, button, select { width: 100%; padding: 10px; margin: 5px 0; border-radius: 8px; border: 1px solid #444; background: #2d2d2d; color: white; box-sizing: border-box; }
        button { background: var(--primary); color: #121212; font-weight: bold; border: none; cursor: pointer; }
        button:active { transform: scale(0.98); }
        .btn-danger { background: var(--accent); color: white; }
        .btn-stop { background: #ff9800; font-size: 1.2em; height: 50px; }
        .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
        .alarm-item { display: flex; justify-content: space-between; align-items: center; background: #2a2a2a; padding: 10px; border-radius: 8px; margin-bottom: 5px; }
        .info { font-size: 0.8em; color: #aaa; margin-bottom: 10px; text-align: center; margin-top: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 Alarma V5.1</h1>
        
        <div class="card" style="border-left-color: #ff9800;">
            <h3>Control en Vivo</h3>
            <button class="btn-stop" style="background:#f0f; color:#fff; font-size:1.4em; border: 2px solid #fff;" onclick="window.location.href='/games'">🎮 CONTROL ARCADE 🎮</button>
            <p style="text-align:center;color:#00adb5;margin-top:8px;font-weight:bold;">Juegos disponibles: Flappy, Asteroids, Runner</p>
            <div class="grid" style="margin-top: 10px;">
                <button class="btn-stop" onclick="cmd('/api/stop')">DETENER</button>
                <button class="btn-stop" style="background:#2196F3;" onclick="cmd('/api/stop?snooze=1')">SNOOZE</button>
            </div>
            <input type="text" id="webMsg" placeholder="Enviar mensaje a pantalla...">
            <button onclick="sendMsg()">Enviar Mensaje</button>
            <hr style="border:0.5px solid #555;margin:10px 0;">
            <label style="font-size:.8em;color:#aaa;">Color NeoLED:</label>
            <div style="display:flex;gap:8px;">
              <input type="color" id="neoColor" value="#ff0000" style="width:48px;height:38px;cursor:pointer;">
              <button onclick="setNeoColor()" style="flex:1;">Aplicar</button>
            </div>
        </div>

        <div class="card" style="border-left-color: #9c27b0;">
            <h3>Personalización</h3>
            <div class="grid">
                <select id="faceType">
                    <option value="ANALOG">Analógico</option>
                    <option value="DIGITAL">Digital</option>
                </select>
                <select id="faceIdx">
                    <option value="1">Opción 1</option>
                    <option value="2">Opción 2</option>
                    <option value="3">Opción 3</option>
                </select>
            </div>
            <button onclick="setFace()">Cambiar Esfera</button>
        </div>

        <div class="card" style="border-left-color: #ff6b6b;">
            <h3>Red WiFi</h3>
            <p style="font-size:0.9em;color:#aaa;margin-bottom:10px;"><strong>Red actual:</strong> <span id="wifiName">Cargando...</span></p>
            <button class="btn-danger" onclick="resetWiFi()">🔄 Resetear Credenciales WiFi</button>
            <p style="font-size:0.75em;color:#ff9800;margin-top:8px;">⚠️ Abre el portal de configuración para conectar a una nueva red</p>
        </div>

        <div class="card" style="border-left-color: #4caf50;">
            <h3>Cronómetro y Timer</h3>
            <div class="grid">
                <button onclick="cmd('/api/crono?action=START')">INICIAR CRONO</button>
                <button onclick="cmd('/api/crono?action=PAUSE')">PAUSA</button>
                <button class="btn-danger" onclick="cmd('/api/crono?action=RESET')">RESET</button>
                <button onclick="cmdReload('/api/weather')">ACT. CLIMA</button>
            </div>
            <p id="weatherData" style="text-align:center; color:#00adb5; font-size:1.1em; font-weight:bold; margin-top:15px; margin-bottom:0;">Clima: --</p>
            <hr style="border:0.5px solid #444; margin:15px 0;">
            <input type="number" id="tMin" placeholder="Minutos del Timer">
            <button onclick="setTimer()">Iniciar Temporizador</button>
        </div>

        <div class="card">
            <h3>Gestión de Alarmas</h3>
            <input type="text" id="n" placeholder="Nombre (Trabajo)">
            <input type="time" id="h" value="07:00">
            <input type="text" id="d" placeholder="DIAS (LUN,MAR,VIE)">
            <div style="display:flex; gap:10px;">
                <select id="t" style="flex:1;">
                    <option value="0">Tono 0 (Estándar)</option>
                    <option value="1">StarWars</option>
                    <option value="2">Imperial</option>
                    <option value="3">Beep</option>
                    <option value="4">Sirena</option>
                </select>
                <button style="width:auto; padding:0 20px; background:#2196F3;" onclick="previewTone()">Probar</button>
            </div>
            <button onclick="addAlarm()">+ Añadir Alarma</button>
            <div id="list" style="margin-top:15px;"></div>
        </div>

        <div class="info" id="sysInfo">Cargando info...</div>
    </div>

    <script>
        function cmd(url){fetch(url).then(r=>r.text()).then(t=>{if(t&&t!=='OK')alert(t);});}
        function cmdReload(url){fetch(url).then(r=>r.text()).then(t=>{if(t&&t!=='OK')alert(t);loadAlarms();});}
        let alarmData=[];
        function loadAlarms(){
            fetch('/api/alarms').then(r=>r.json()).then(data=>{
                alarmData=data;
                const list=document.getElementById('list');
                list.innerHTML=data.length===0?'<p>No hay alarmas.</p>':'';
                data.forEach((a,i)=>{
                    list.innerHTML+=`<div class="alarm-item"><div><b>${a.hora}</b> - ${a.nombre}<br><small>${a.dias}</small></div><button class="btn-danger" style="width:40px;" onclick="del(${i})">X</button></div>`;
                });
            });
            fetch('/api/info').then(r=>r.text()).then(t=>{
                document.getElementById('sysInfo').innerText=t;
                // Extraer el nombre de la red WiFi del info (busca línea con "WiFi:")
                const wifiLine = t.split('\n').find(l => l.includes('WiFi:'));
                if(wifiLine) {
                    const wifiName = wifiLine.split('WiFi:')[1]?.trim() || 'Desconocida';
                    document.getElementById('wifiName').innerText = wifiName;
                } else {
                    document.getElementById('wifiName').innerText = 'No conectado';
                }
            });
            fetch('/api/weather_data').then(r=>r.text()).then(t=>document.getElementById('weatherData').innerText='Clima: ' + t);
        }

        function del(idx) {
            const a = alarmData[idx];
            if(!a) return;
            if(confirm('Borrar: ' + a.nombre + '?'))
                fetch('/api/alarms?nombre='+encodeURIComponent(a.nombre),{method:'DELETE'}).then(()=>loadAlarms());
        }

        function addAlarm() {
            const data = { nombre: document.getElementById('n').value||'Alarma', hora: document.getElementById('h').value, dias: document.getElementById('d').value||'DOM', tono: parseInt(document.getElementById('t').value) };
            if(!data.hora) return alert('Ingresa una hora (HH:MM)');
            fetch('/api/alarms',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)}).then(r=>r.text()).then(t=>{ if(t!=='OK') alert(t); loadAlarms(); });
        }

        function sendMsg() { cmd('/api/msg?text='+encodeURIComponent(document.getElementById('webMsg').value)); }
        function setTimer() { cmdReload('/api/timer?min='+document.getElementById('tMin').value); }
        function setFace()  { cmd('/api/face?type='+document.getElementById('faceType').value+'&idx='+document.getElementById('faceIdx').value); }
        function setNeoColor(){var h=document.getElementById('neoColor').value.substring(1);cmd('/api/neocolor?hex='+h);}
        function previewTone() { cmd('/api/playTone?id=' + document.getElementById('t').value); }
        
        function resetWiFi() {
            if(confirm('⚠️  ¿Limpiar WiFi? El portal se abrirá para conectar a una nueva red.')) {
                fetch('/api/wifireset').then(r=>r.text()).then(t=>alert(t));
            }
        }

        window.onload = loadAlarms;
        setInterval(loadAlarms, 10000);
    </script>
</body>
</html>
)rawliteral";

void handleRoot() { server.send(200, "text/html", DASHBOARD_HTML); }
void handleGames() { server.send(200, "text/html", GAMES_HTML); }

void apiStop() {
    stopAlarm(server.hasArg("snooze"));
    server.send(200, "text/plain", "OK");
}

void apiCrono() {
    String act = server.arg("action");
    if (act == "START") { if (!cronoRunning) { cronoStart = millis(); cronoRunning = true; } }
    else if (act == "PAUSE") { if(cronoRunning) { cronoElapsed += (millis() - cronoStart); cronoRunning = false; } }
    else if (act == "RESET") { cronoElapsed = 0; cronoRunning = false; }
    forceRedraw = true;
    server.send(200, "text/plain", "OK");
}

void apiTimer() {
    int m = server.arg("min").toInt();
    if (m > 0) {
        timerFin = millis() + (unsigned long)m * 60000UL;
        timerActivo = true; timerSonando = false;
        notify("Timer: " + String(m) + " min");
        server.send(200, "text/plain", "Timer Iniciado");
    } else server.send(400, "text/plain", "Minutos?");
}

void apiFace() {
    String type = server.arg("type");
    int idx = server.arg("idx").toInt();
    if (idx < 1 || idx > 3) { server.send(400, "text/plain", "Idx:1-3"); return; }
    if (type == "ANALOG")       { isAnalog = true;  analogFace  = idx; }
    else if (type == "DIGITAL") { isAnalog = false; digitalFace = idx; }
    else { server.send(400, "text/plain", "ANALOG|DIGITAL"); return; }
    forceRedraw = true; lastDisplay = 0;
    server.send(200, "text/plain", "OK");
}

void apiMsg() {
    String msg = server.arg("text");
    if (msg.length() > 120) msg = msg.substring(0, 120);
    Serial.printf("[WEB] Mensaje recibido: %s\n", msg.c_str());
    notify(msg, 8000);
    server.send(200, "text/plain", "OK");
}

void apiNeoColor() {
    String hex = server.arg("hex");
    if (hex.length() == 6) {
        uint8_t r = (uint8_t)strtol(hex.substring(0,2).c_str(),NULL,16);
        uint8_t g = (uint8_t)strtol(hex.substring(2,4).c_str(),NULL,16);
        uint8_t b = (uint8_t)strtol(hex.substring(4,6).c_str(),NULL,16);
        alarmNeoColor = ((uint32_t)r<<16)|((uint32_t)g<<8)|b;
        server.send(200, "text/plain", "OK");
    } else { server.send(400, "text/plain", "Hex?"); }
}

void apiWeather() {
    // Trigger immediate fetch desde web
    triggerWeatherFetch();
    server.send(200, "text/plain", "OK");
}

void apiWeatherData() {
    server.send(200, "text/plain", globalTemp + "C - " + globalWeather);
}

void apiScores() {
    String key = server.arg("game");
    if (key != "flappy" && key != "aster" && key != "runner") {
        server.send(400, "text/plain", "game=flappy|aster|runner");
        return;
    }
    int scores[HIGHSCORE_COUNT];
    loadHighScores(key.c_str(), scores);
    String title = key;
    if (key == "flappy") title = "Flappy";
    else if (key == "aster") title = "Asteroids";
    else if (key == "runner") title = "Runner";
    String res = "Top 5 " + title + "\n";
    for (int i = 0; i < HIGHSCORE_COUNT; i++) {
        res += String(i + 1) + ": " + scores[i] + "\n";
    }
    server.send(200, "text/plain", res);
}

String getWiFiNetworksList();  // Forward declaration

void apiInfo() {
    String networks = getWiFiNetworksList();
    String info = "WiFi: " + WiFi.SSID() + "\nIP: " + WiFi.localIP().toString() + " | RSSI: " + String(WiFi.RSSI()) + "dBm | Uptime: " + String(millis()/60000) + "m\nRedes guardadas: " + networks;
    server.send(200, "text/plain", info);
}

extern bool vBtn1, vBtn2, vBtn3;
void apiBtn() {
    int id = server.arg("id").toInt();
    bool state = (server.arg("state").toInt() == 1);
    if (id == 1) vBtn1 = state;
    if (id == 2) vBtn2 = state;
    if (id == 3) vBtn3 = state;
    server.send(200, "text/plain", "OK");
}

extern volatile bool btnImm1, btnImm2, btnImm3;
void apiBtnImm() {
    int id = server.arg("id").toInt();
    if (id == 1) btnImm1 = true;
    if (id == 2) btnImm2 = true;
    if (id == 3) btnImm3 = true;
    server.send(200, "text/plain", "OK");
}

extern void initMinijuego();
extern void initAsteroids();
extern void initRunner();

extern int wizardTone;
extern bool previewActivo;
void apiPlayTone() {
    wizardTone = server.arg("id").toInt();
    previewActivo = true;
    server.send(200, "text/plain", "OK");
}

void apiStartGame() {
    String g = server.arg("game");
    if (g == "minijuego") { menuEstado = MENU_MINIJUEGO; initMinijuego(); }
    else if (g == "asteroids") { menuEstado = MENU_ASTEROIDS; initAsteroids(); }
    else if (g == "runner") { menuEstado = MENU_RUNNER; initRunner(); }
    else {
        server.send(400, "text/plain", "game=minijuego|asteroids|runner");
        return;
    }
    forceRedraw = true;
    server.send(200, "text/plain", "OK");
}

void apiGetAlarms() {
    StaticJsonDocument<2048> doc;
    JsonArray array = doc.to<JsonArray>();
    for (int i = 0; i < numAlarmas; i++) {
        JsonObject a = array.createNestedObject();
        a["nombre"] = alarmas[i].nombre;
        a["hora"] = alarmas[i].hora;
        a["dias"] = alarmas[i].dias;
        a["tono"] = alarmas[i].tono;
    }
    String res; serializeJson(doc, res);
    server.send(200, "application/json", res);
}

void apiAddAlarm() {
    StaticJsonDocument<512> doc;
    deserializeJson(doc, server.arg("plain"));
    if (numAlarmas >= MAX_ALARMAS) { server.send(400, "text/plain", "Max 10"); return; }
    String nombre = doc["nombre"] | "Alarma";
    String hora   = doc["hora"]   | "";
    String dias   = doc["dias"]   | "DOM";
    int    tono   = doc["tono"]   | 0;
    if (hora.length() != 5 || hora[2] != ':') { server.send(400, "text/plain", "Hora HH:MM"); return; }
    for (int i = 0; i < numAlarmas; i++) {
        if (alarmas[i].nombre.equalsIgnoreCase(nombre)) { server.send(400, "text/plain", "Duplicado"); return; }
    }
    alarmas[numAlarmas].nombre       = nombre;
    alarmas[numAlarmas].hora         = hora;
    alarmas[numAlarmas].dias         = normalizarDias(dias);
    alarmas[numAlarmas].esFecha      = (alarmas[numAlarmas].dias.indexOf('-') >= 0);
    alarmas[numAlarmas].tono         = tono;
    alarmas[numAlarmas].activa       = true;
    alarmas[numAlarmas].lastFired    = "";
    alarmas[numAlarmas].deleteOnFire = false;
    numAlarmas++; saveAlarmas(); forceRedraw = true;
    server.send(200, "text/plain", "OK");
}

void apiDeleteAlarm() {
    if (deleteAlarma(server.arg("nombre"))) server.send(200, "text/plain", "OK");
    else server.send(404, "text/plain", "No encontrada");
}

void apiWiFiReset();  // Forward declaration

void setupWebServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/games", HTTP_GET, handleGames);
    server.on("/api/alarms", HTTP_GET, apiGetAlarms);
    server.on("/api/alarms", HTTP_POST, apiAddAlarm);
    server.on("/api/alarms", HTTP_DELETE, apiDeleteAlarm);
    server.on("/api/stop", apiStop);
    server.on("/api/crono", apiCrono);
    server.on("/api/timer", apiTimer);
    server.on("/api/face", apiFace);
    server.on("/api/msg", apiMsg);
    server.on("/api/weather", apiWeather);
    server.on("/api/weather_data", apiWeatherData);
    server.on("/api/scores", apiScores);
    server.on("/api/info", apiInfo);
    server.on("/api/neocolor", apiNeoColor);
    server.on("/api/btn", apiBtn);
    server.on("/api/btnImm", apiBtnImm);
    server.on("/api/startgame", apiStartGame);
    server.on("/api/playTone", apiPlayTone);
    server.on("/api/wifireset", apiWiFiReset);
    server.begin();
}

void publishList() {
  if (!mqttClient.connected()) return;
  if (numAlarmas == 0) { mqttClient.publish(topicStatus, "Lista vacia"); return; }
  String total = "--- Lista de Alarmas ---\n";
  for (int i = 0; i < numAlarmas; i++) {
    total += String(i+1) + ") " + alarmas[i].nombre + " | " + alarmas[i].dias + " | " + alarmas[i].hora + " [T:" + String(alarmas[i].tono) + "]\n";
  }
  mqttClient.publish(topicStatus, total.c_str());
}

void connectMQTT() {
  if (mqttClient.connected()) return;
  if (millis() - lastMqttRetry < 30000) return;
  lastMqttRetry = millis();
  String myId = "ESP32Alarma_" + WiFi.macAddress();
  myId.replace(":", "");
  if (mqttClient.connect(myId.c_str())) {
    mqttClient.subscribe(topicSet);     mqttClient.subscribe(topicStop);
    mqttClient.subscribe(topicTimer);   mqttClient.subscribe(topicList);
    mqttClient.subscribe(topicDelete);  mqttClient.subscribe(topicModify);
    mqttClient.subscribe(topicFace);    mqttClient.subscribe(topicCrono);
    mqttClient.subscribe(topicWeather); mqttClient.subscribe(topicInfo);
    mqttClient.subscribe(topicMessage);
  }
}

// ── GESTIÓN DE MÚLTIPLES REDES WiFi ────────────────
// Guardar una red WiFi en la lista de redes conocidas
void saveWiFiNetwork(const char* ssid, const char* password) {
  if (!ssid || ssid[0] == '\0') {
    Serial.println("[WiFi] ❌ SSID vacío, no se guarda");
    return;
  }
  
  preferences.begin("wifi_nets", false);
  
  int count = preferences.getInt("net_count", 0);
  bool found = false;
  int foundIndex = -1;
  
  // Buscar si esta red ya está guardada
  for (int i = 0; i < count; i++) {
    String existingSSID = preferences.getString(("net_ssid_" + String(i)).c_str(), "");
    if (existingSSID == String(ssid)) {
      found = true;
      foundIndex = i;
      break;
    }
  }
  
  if (found) {
    // Actualizar contraseña de red existente
    Serial.printf("[WiFi] 🔄 Actualizando red existente: %s\n", ssid);
    preferences.putString(("net_pass_" + String(foundIndex)).c_str(), password);
  } else if (count < 10) {
    // Agregar nueva red
    preferences.putString(("net_ssid_" + String(count)).c_str(), String(ssid));
    preferences.putString(("net_pass_" + String(count)).c_str(), String(password));
    preferences.putInt("net_count", count + 1);
    Serial.printf("[WiFi] ✅ Nueva red guardada: %s (Total: %d)\n", ssid, count + 1);
  } else {
    Serial.println("[WiFi] ❌ Máximo de redes alcanzado (10)");
  }
  
  preferences.end();
}

// Escanear redes disponibles e intentar conectarse a una guardada
bool connectToAvailableNetwork() {
  Serial.println("[WiFi] 🔍 Escaneando redes disponibles...");
  
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("[WiFi] ❌ No se encontraron redes WiFi");
    WiFi.scanDelete();
    return false;
  }
  
  Serial.printf("[WiFi] 📡 Se encontraron %d redes\n", n);
  
  preferences.begin("wifi_nets", true); // Lectura
  int savedNetCount = preferences.getInt("net_count", 0);
  
  Serial.printf("[WiFi] 💾 Redes guardadas: %d\n", savedNetCount);
  
  // Intentar conectarse a cada red guardada que esté disponible
  for (int i = 0; i < savedNetCount; i++) {
    String savedSSID = preferences.getString(("net_ssid_" + String(i)).c_str(), "");
    if (savedSSID.length() == 0) continue;
    
    Serial.printf("[WiFi] 🔎 Buscando red guardada: %s\n", savedSSID.c_str());
    
    // Buscar esta red en las disponibles
    for (int j = 0; j < n; j++) {
      if (WiFi.SSID(j) == savedSSID) {
        String password = preferences.getString(("net_pass_" + String(i)).c_str(), "");
        preferences.end();
        
        Serial.printf("[WiFi] ✅ Encontrada red guardada: %s (señal: %d dBm)\n", 
                     savedSSID.c_str(), WiFi.RSSI(j));
        
        // Intentar conectar
        WiFi.begin(savedSSID.c_str(), password.c_str());
        unsigned long startTime = millis();
        int attempts = 0;
        
        while (millis() - startTime < 10000 && WiFi.status() != WL_CONNECTED) {
          delay(500);
          attempts++;
          if (attempts % 2 == 0) {
            Serial.print(".");
          }
        }
        
        if (WiFi.status() == WL_CONNECTED) {
          Serial.printf("\n[WiFi] 🎉 Conectado a: %s | IP: %s\n", 
                       savedSSID.c_str(), WiFi.localIP().toString().c_str());
          WiFi.scanDelete();
          return true;
        } else {
          Serial.printf("\n[WiFi] ⚠️  Fallo conectar a %s, intentando siguiente...\n", savedSSID.c_str());
          // Reintentar con siguiente red
          preferences.begin("wifi_nets", true);
        }
      }
    }
  }
  
  preferences.end();
  WiFi.scanDelete();
  return false;
}

// Obtener lista de redes guardadas
String getWiFiNetworksList() {
  preferences.begin("wifi_nets", true);
  int count = preferences.getInt("net_count", 0);
  String list = "";
  
  for (int i = 0; i < count; i++) {
    String ssid = preferences.getString(("net_ssid_" + String(i)).c_str(), "");
    if (ssid.length() > 0) {
      if (i > 0) list += ", ";
      list += ssid;
    }
  }
  
  preferences.end();
  return (list.length() > 0) ? list : "Ninguna";
}

// ── API para resetear WiFi ────────────────────────
extern void resetWiFiCredentials();
void apiWiFiReset() {
  server.send(200, "text/plain", "Reseteando WiFi...");
  delay(500);
  resetWiFiCredentials();
}

#endif
