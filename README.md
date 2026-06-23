# Informe de Pràctica 3: WiFi i Bluetooth

**Autors:** Julio Lázaro Alcobendas i Gerard Rodríguez González       
**Data:** 10 de Març de 2026  
**Repositori GitHub:** https://github.com/jla0306/Practica-3

---

# Pràctica A: Generació d'una Pàgina Web (Mode STA)

## 1. Objectius de la pràctica

L'objectiu principal d'aquesta pràctica és comprendre el funcionament de la connectivitat WiFi de l'ESP32-S3 i ser capaços de generar un servidor web des del microcontrolador. El dispositiu es connecta a una xarxa WiFi existent en mode Estació (STA) i serveix una pàgina HTML accessible des de qualsevol navegador de la mateixa xarxa local. A més, s'utilitza el port sèrie per monitorar el procés de connexió i obtenir la IP assignada.

## 2. Desenvolupament i Arquitectura

S'ha configurat l'entorn de PlatformIO amb el framework d'Arduino per a la placa ESP32-S3-DevKitC-1. No es requereix cap muntatge de hardware addicional, ja que el mòdul WiFi és integrat al xip. El programa utilitza les llibreries `WiFi.h` i `WebServer.h` pròpies del framework ESP32 per a Arduino.

El funcionament es basa en inicialitzar la connexió WiFi en mode STA amb les credencials de la xarxa (SSID i contrasenya), esperar fins que la connexió s'estableixi correctament, registrar el servidor HTTP al port 80 i, finalment, entrar en un bucle infinit on el servidor escolta i respon peticions entrants.

## 3. Codi Principal (main.cpp)

```cpp
#include <WiFi.h>
#include <WebServer.h>

// ================== CONFIGURACIÓ ==================
const char* ssid     = "Nautilus";
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
  Serial.println("\n=== Iniciant ESP32 Web Server ===");
  WiFi.begin(ssid, password);
  Serial.print("Connectant a ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connectat!");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());
  server.on("/", handle_root);
  server.begin();
  Serial.println("Servidor HTTP iniciat al port 80");
  Serial.println("Obre al navegador: http://" + WiFi.localIP().toString());
}

// ================== LOOP ==================
void loop() {
  server.handleClient();
}
```

## 4. Funcionament del codi

En primer lloc, definim les credencials de la xarxa WiFi i creem un objecte `WebServer` al port 80 (el port estàndard HTTP). La funció `handle_root()` es defineix abans del `setup()` i conté la pàgina HTML que es retornarà als clients.

Durant el **setup()**, s'inicia el port sèrie i es crida a `WiFi.begin()` per iniciar la connexió. El programa queda bloquejat en un bucle `while` fins que `WiFi.status()` retorna `WL_CONNECTED`. Un cop connectat, s'imprimeix la IP local assignada pel router, es registra el manejador de ruta per a `/` i s'inicia el servidor.

En el **loop()** infinit, la funció `server.handleClient()` comprova si hi ha peticions HTTP entrants i, si n'hi ha, crida al manejador corresponent. Quan un navegador accedeix a la IP del ESP32, rep la pàgina HTML amb informació del servidor.

## 5. Sortida pel Monitor Sèrie

```
=== Iniciant ESP32 Web Server ===
Connectant a Nautilus
.....
WiFi connectat!
IP del ESP32: 192.168.1.45
Servidor HTTP iniciat al port 80
Obre al navegador: http://192.168.1.45
```

## 6. Codi Especificacions (platformio.ini)

```ini
[env:esp32-s3-devkitc-1]
platform  = espressif32
board     = esp32-s3-devkitc-1
framework = arduino

; Configuració del monitor
monitor_speed = 115200
```

## 7. Diagrama de flux de main.cpp

```
Inici Programa
       ↓
Inicialitzar WiFi (ssid + password)
       ↓
Esperar WL_CONNECTED (bucle while)
       ↓
Imprimir IP local pel Port Sèrie
       ↓
Registrar ruta '/' → handle_root()
       ↓
Iniciar servidor HTTP al port 80
       ↓
Bucle Infinit / loop()
       ↓
server.handleClient() — atendre peticions HTTP
       ↻ (torna a llegir)
```

## 8. Diagrama de temps / seqüència de connexió

| Fase              | Acció                                | Durada aprox. |
|-------------------|--------------------------------------|---------------|
| Arrencada         | Serial.begin + WiFi.begin()          | < 100 ms      |
| Connexió WiFi     | Bucle while fins WL_CONNECTED        | 2 – 5 s       |
| Inici servidor    | server.on() + server.begin()         | < 10 ms       |
| Escolta (loop)    | server.handleClient() continu        | Indefinida    |
| Resposta petició  | handle_root() → envia HTML           | < 5 ms        |

## 9. Preguntes de la pràctica

**Quin és el temps lliure que té el processador?** A diferència de la Pràctica 1, aquí el processador no descansa en un `delay()` fix. La funció `server.handleClient()` retorna gairebé immediatament si no hi ha peticions, cosa que fa que el bucle s'executi centenars de vegades per segon. El processador té temps lliure entre execucions de `handleClient()`, però el bucle és continu i no hi ha un `delay` explícit. Això significa que el nucli FreeRTOS intern de l'ESP32-S3 pot assignar cicles a les tasques de WiFi en segon pla.

**Com s'identifica l'ESP32 a la xarxa?** El router DHCP de la xarxa local assigna una adreça IP dinàmica al ESP32 quan es connecta. Aquesta IP s'imprimeix pel monitor sèrie i és la que cal introduir al navegador per accedir a la pàgina web servida.

## 10. Conclusions

Aquesta pràctica ens ha permès descobrir les capacitats de connectivitat WiFi de l'ESP32-S3 i entendre com un microcontrolador pot actuar com a servidor web complet. Hem assolit amb èxit la connexió a una xarxa local en mode STA, l'obtenció d'una IP per DHCP i la generació d'una resposta HTML dinàmica que inclou la pròpia IP del dispositiu. El model de programació basat en registre de rutes (`server.on()`) és intuïtiu i escalable per afegir més endpoints en el futur.

---

# EXERCICI DE MILLORA DE NOTA: Mode Access Point (AP)

## 1. Objectiu

L'objectiu d'aquest exercici és repetir la pràctica A però canviant el mode de connexió WiFi. En lloc de connectar-se a una xarxa existent (mode STA), l'ESP32-S3 crea la seva pròpia xarxa WiFi (mode Access Point, AP). Qualsevol dispositiu que s'hi connecti podrà accedir al servidor web sense necessitar cap router extern.

## 2. Desenvolupament

En lloc de `WiFi.begin(ssid, password)`, s'utilitza `WiFi.softAP(ssid_ap, password_ap)`. Això configura el mòdul WiFi de l'ESP32 com un punt d'accés independent. Per defecte, el gateway i la IP del servidor és **192.168.4.1**. No cal esperar cap connexió externa: el servidor arrenca immediatament.

## 3. Codi main

```cpp
#include <WiFi.h>
#include <WebServer.h>

const char* ssid_ap     = "Nautilus";
const char* password_ap = "20000Leguas";  // mínim 8 caràcters

WebServer server(80);

String HTML = R"rawliteral(
<!DOCTYPE html><html>
<body style="background:#000;color:#0f0;text-align:center;padding:50px;">
<h1>🚀 ESP32 en Mode AP</h1>
<p>Connecta't a la xarxa <b>Nautilus</b></p>
<p>IP del servidor: 192.168.4.1</p>
</body></html>
)rawliteral";

void handle_root() { server.send(200, "text/html", HTML); }

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid_ap, password_ap);
  Serial.print("AP creat! IP: ");
  Serial.println(WiFi.softAPIP());
  server.on("/", handle_root);
  server.begin();
  Serial.println("Servidor AP iniciat");
}

void loop() {
  server.handleClient();
}
```

## 4. Funcionament del codi

El programa utilitza `WiFi.softAP()` per crear una nova xarxa WiFi amb el nom i la contrasenya indicats. Un cop creada, l'ESP32 actua simultàniament com a punt d'accés i com a servidor HTTP. La IP del servidor és sempre **192.168.4.1** (valor per defecte del mode AP d'Arduino per a ESP32).

La pàgina HTML en aquest cas s'emmagatzema com a cadena estàtica (literal `R"rawliteral"`) en lloc de generar-la dinàmicament, ja que la IP és sempre la mateixa i no depèn d'un servidor DHCP extern.

## 5. Diferències entre Mode STA i Mode AP

| Característica     | Mode STA                     | Mode AP                     |
|--------------------|------------------------------|-----------------------------|
| Connexió           | Es connecta a un router      | Crea la seva pròpia xarxa   |
| IP assignada per   | Router (DHCP dinàmic)        | Pròpia (192.168.4.1 fix)    |
| Accés a Internet   | Sí (a través del router)     | No (xarxa aïllada)          |
| Necessita router   | Sí                           | No                          |
| Funció clau        | WiFi.begin(ssid, pass)       | WiFi.softAP(ssid, pass)     |
| Cas d'ús           | Integrar-se a LAN existent   | Dispositiu autònom o IoT    |

## 6. Sortida pel Monitor Sèrie

```
AP creat! IP: 192.168.4.1
Servidor AP iniciat
```

## 7. Diagrama de flux de main.cpp (Mode AP)

```
Inici Programa
       ↓
Inicialitzar Port Sèrie a 115200 bauds
       ↓
WiFi.softAP(ssid_ap, password_ap) — crear xarxa
       ↓
Imprimir IP del AP (192.168.4.1) pel Port Sèrie
       ↓
Registrar ruta '/' → handle_root()
       ↓
Iniciar servidor HTTP al port 80
       ↓
Bucle Infinit / loop()
       ↓
server.handleClient() — atendre peticions HTTP
       ↻ (torna a llegir)
```

## 8. Conclusions

Aquest exercici de millora ens ha ensenyat una alternativa molt útil al mode STA: el mode AP. Amb una modificació mínima del codi (canviar `WiFi.begin()` per `WiFi.softAP()`), l'ESP32-S3 es transforma en un punt d'accés autònom. Això és especialment interessant per a aplicacions IoT on no hi ha xarxes WiFi disponibles, com a sistemes de configuració en camp o dispositius que necessiten ser configurats sense router.

La IP fixa **192.168.4.1** simplifica el codi (no cal llegir una IP dinàmica) i fa que l'accés al servidor sigui sempre predictible.
