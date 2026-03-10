#include <WiFi.h>
#include <WebServer.h>

const char* ssid_ap = "Nautilus";
const char* password_ap = "20000Leguas";  // mínimo 8 caracteres

WebServer server(80);

String HTML = R"rawliteral(
<!DOCTYPE html><html><body style="background:#000;color:#0f0;text-align:center;padding:50px;">
<h1>🚀 ESP32 en Modo AP</h1>
<p>Conéctate a la red <b>ESP32-Practica3-AP</b></p>
<p>IP del servidor: 192.168.4.1</p>
</body></html>
)rawliteral";

void handle_root() { server.send(200, "text/html", HTML); }

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid_ap, password_ap);
  Serial.print("AP creado! IP: ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", handle_root);
  server.begin();
  Serial.println("Servidor AP iniciado");
}

void loop() {
  server.handleClient();
}