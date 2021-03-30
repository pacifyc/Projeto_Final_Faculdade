

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Adafruit_Sensor.h>

#include <DHT.h>

#include <ESP8266mDNS.h> 
/*________________________________________________________________________________________________________________*/

  #define ldr 0
  #define led1Pin 14                              // GPIO14 do ESP8266 e D5 do NodeMCU

/*________________________________________________________________________________________________________________*/

  #define DHTPIN 5                                // Pino digital conectado ao sensor DHT

// Remova o comentário do tipo de sensor em uso:

  #define DHTTYPE    DHT11     // DHT 11
  
  DHT dht(DHTPIN, DHTTYPE);

/*________________________________________________________________________________________________________________*/

  String statusLed;
  int luminosidade;

/*________________________________________________________________________________________________________________*/

// temperatura e umidade atuais, atualizadas no loop ()

  float t = 0.0;
  float h = 0.0;

  int l = 0;

/*________________________________________________________________________________________________________________*/

const char* ssid = "net";            // Nome da Rede
const char* password = "999999999999";          //Password da rede
const char* host = "projeto";

/*________________________________________________________________________________________________________________*/

//Definição de Ip fixo para o NodeMCU

  IPAddress ip(10,0,0,115);
  IPAddress gateway(10,0,0,1);                //GATEWAY de conexão
  IPAddress subnet(255,255,255,0);

/*________________________________________________________________________________________________________________*/

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store

  unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds

  const long interval = 10000;  

/*________________________________________________________________________________________________________________*/


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  //<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
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
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP8266 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>

    <h3>Luminosidade LED</h3>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Luminosidade</span> 
    <span id="luminosidade">%LUMINOSIDADE%</span>
    <sup class="units">*</sup>
  </p>
</body>


<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("luminosidade").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/luminosidade", true);
  xhttp.send();
}, 10000 ) ;
</script>

</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  else if(var == "LUMINOSIDADE"){
    return String(l);
  }
  return String();
}

/*________________________________________________________________________________________________________________*/

//Servidor Web na porta 80

  AsyncWebServer server(80);     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

/*________________________________________________________________________________________________________________*/


/*//////////////////////////////////////////////////////////////////////////////////////////////////*/


/*________________________________________________________________________________________________________________*/

void InicializaPinos(){

  
  pinMode(ldr, INPUT);
  pinMode(led1Pin, OUTPUT);                     // Define o D5 como saída
  
  digitalWrite(led1Pin, LOW);                   // O LED começa desligado
  
  delay(100);
  
}

/*________________________________________________________________________________________________________________*/

void InicializaWifi() {
  

// Captura do endereço MAC
  
  Serial.println();
  //delay(500);
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

    delay(100);

  // WiFi.config(ip);
  WiFi.config(ip, gateway, subnet);          //Passa os parâmetros para a função que vai setar o Ip fixo no NodeMCU

    delay(100);
    
// Comunicação com a rede WiFi

  Serial.println();
  Serial.println();
  Serial.print("Conectando com ");              // Mensagem apresentada no monitor série
  Serial.println(ssid);                         // Apresenta o nome da rede no monitor série
  Serial.println();
  Serial.println();

  delay(500);

  WiFi.begin(ssid, password);                   // Inicia a ligação a rede
  Serial.println("Comunicação iniciada...!");
  Serial.println("Conectando com a rede WiFi...!");

  
  while (WiFi.status ()!= WL_CONNECTED) {
    delay (500);
    Serial.print (".");
    
  }
  
  Serial.println ("");
  Serial.println ("WiFi conectado");

  
}

/*________________________________________________________________________________________________________________*/

void InicializaMDNS() {
  
  if (!MDNS.begin(host)) {
    
    Serial.println("Erro ao iniciar o servico mDNS!");
  
    while (1) {
    
      delay(100);
    
    }

  }

  Serial.println("O servico mDNS foi iniciado com sucesso!");
   
  MDNS.addService("http", "tcp", 80);
  
  delay(500);
  Serial.print("http://");
  Serial.print(host);
  Serial.println(".local/");
  

}

/*________________________________________________________________________________________________________________*/

void InicializaServidor() {
  
// Servidor

  server.begin();                               // Comunicação com o servidor
  Serial.println("Servidor iniciado");          //é apresentado no monitor série que o servidor foi iniciado

  dht.begin();

  
// Impressão do endereço IP

  Serial.print("Use o seguinte URL para a comunicação: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());                 //Abrindo o Brower com este IP acessamos á pagina HTML
  Serial.println("/");
  
  delay(100);
  
}


/*________________________________________________________________________________________________________________*/


void setup(){

  Serial.begin(115200);
  
    delay(100);

  InicializaPinos();
  
  InicializaWifi();
  
  InicializaMDNS();
  
/*_______________________________*/


  l = analogRead(ldr);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });
  server.on("/luminosidade", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(l).c_str());
  });
  
/*_______________________________*/

  InicializaServidor();

  
}
 



void loop(){  
  unsigned long currentMillis = millis();


  
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
    }
    

    int newL = analogRead(ldr);
    // if humidity read failed, don't change h value 
    if (isnan(newL)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      l = newL;
      Serial.println(l);
    }
  }
}
