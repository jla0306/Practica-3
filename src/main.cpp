#include <WiFi.h>
#include <WebServer.h>

// ================== CONFIGURACIÓN ==================
const char* ssid = "Nautilus";
const char* password = "20000Leguas";

WebServer server(80);

// ================== FUNCIÓ handle_root (ABANS de setup) ==================
void handle_root() {
  String HTML = "<!DOCTYPE html>"
                "<html>"
                "<head><title>ESP32 Web Server</title></head>"
                "<body style='font-family:Arial;text-align:center;background:#0f0f0f;color:#00ff9d;padding:50px;'>"
                "<h1>🌐 Mi Primera Pagina con ESP32 - Station Mode 😎</h1>"
                "<p><strong>Conectado correctamente!</strong></p>"
                "<p>IP del servidor: " + WiFi.localIP().toString() + "</p>"
                "<hr>"
                "<p>Práctica 3 - WiFi + Bluetooth</p>"
                "</body>"
                "</html>";
  
  server.send(200, "text/html", HTML);
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Iniciando ESP32 Web Server ===");

  WiFi.begin(ssid, password);
  Serial.print("Conectando a ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_root);
  server.begin();
  
  Serial.println("Servidor HTTP iniciado en puerto 80");
  Serial.println("Abre en el navegador: http://" + WiFi.localIP().toString());
}

// ================== LOOP ==================
void loop() {
  server.handleClient();
}