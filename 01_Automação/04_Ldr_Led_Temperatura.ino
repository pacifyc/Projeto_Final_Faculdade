
#include <ESP8266WiFi.h>
#include <WiFiClient.h>


#include <ESP8266WebServer.h>                     //ESP Web Server Library to host a web page

#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>

#include <DHT.h>

#include <Adafruit_Sensor.h>

/*________________________________________________________________________________________________________________*/

  #define ldr 0
  #define led1Pin 14                              // GPIO14 do ESP8266 e D5 do NodeMCU

/*________________________________________________________________________________________________________________*/

  #define DHTPIN 5                                // Pino digital conectado ao sensor DHT

// Remova o comentário do tipo de sensor em uso:

  #define DHTTYPE    DHT11     // DHT 11
  
  DHT dht(DHTPIN, DHTTYPE);

/*________________________________________________________________________________________________________________*/

// Auxiliar variables to store the current output state

  String outputD5State = "off";

  String statusLed;
  int luminosidade;

  int valorLed;

/*________________________________________________________________________________________________________________*/

// temperatura e umidade atuais, atualizadas no loop ()

  float t = 0.0;    // Variavél para Temperatura do sensor DH11
  float h = 0.0;    // Variavél para Humidade do sensor DH11
  
  int l = 0;        // Variavél para Luminosidade do sensor DH11

/*________________________________________________________________________________________________________________*/

const char* ssid = "net";            // Nome da Rede
const char* password = "9999999999";          //Password da rede
const char* host = "projeto";
/*________________________________________________________________________________________________________________*/


//Declare a global object variable from the ESP8266WebServer class.
//ESP8266WebServer server(80); //Server on port 80

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


String Pg_Html = " "

"<!DOCTYPE HTML><html>" \
"<head>" \
  "<meta name='viewport' content='width=device-width, initial-scale=1'>" \
  //<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  "<style>" \
    "html {" \
     "font-family: Arial"
     "display: inline-block;" \
     "margin: 0px auto;" \
     "text-align: center;" \
    "}" \
    "h2 { font-size: 2.0rem; }" \
    "p { font-size: 1.0rem; }" \
    ".units { font-size: 1.2rem; }"
    ".dht-labels{" \
    " font-size: 1.5rem;" \
    "vertical-align:middle;" \
    "padding-bottom: 15px;" \
    "}" \

    ".button { background-color: #195B6A;" \ 
    " border: none; color: white; padding: 16px 40px;" \
    " text-decoration: none; font-size: 30px;" \
    " margin: 2px; cursor: pointer" \
    "}"
    ".button2 {background-color: #77878A;}" \
            


    
  "</style>" \
"</head>" \
"<body>" \
  "<h2>ESP8266 DHT Server</h2>" \
  "<p>" \
    "<i class='fas fa-thermometer-half' style='color:#059e8a;'></i>" \
    "<span class='dht-labels'>Temperature</span>" \
    "<span id='temperature'>%TEMPERATURE%</span>" \
    "<sup class='units'>&deg;C</sup>" \
  "</p>" \
  "<p>" \
    "<i class='fas fa-tint' style='color:#00add6;'></i>" \
    "<span class='dht-labels'>Humidity</span>" \
    "<span id='humidity'>%-.-%</span>" \
    "<sup class='units'>%</sup>" \
  "</p>" \
  "<h3>Luminosidade LED</h3>" \
    "<p>" \
    "<i class='fas fa-tint' style='color:#00add6;'></i>" \
    "<span class='dht-labels'>Luminosidade</span>" \
    "<span id='luminosity'>%-.-%</span>" \
    "<sup class='units'>*</sup>" \
  "</p>" \

   "<h2>Acionamento de LED</h2>" \
  "<p>" \
    "<i class='fas fa-thermometer-half' style='color:#059e8a;'></i>" \


    "<span class='dht-labels'>Estatus do Led</span>" \
    



  "<div id='demo'>" \
  
    "<h1>The ESP8266 NodeMCU Update web page without refresh</h1>"
    
      "<button type='button' onclick='sendData(1)'>LED ON</button>" \
      "<button type='button' onclick='sendData(0)'>LED OFF</button>" \
      
  "</div>" \
 
  "<div>" \
  
      "ADC Value is : <span id='ADCValue'>0</span><br>" \

  "</div>" \

  "<form action='/LED' method='POST'><input type='submit' value='Toggle LED'>"
  "</form>"
        

    
  "</p>" \

  
"</body>"
 
"<script>" \


"function sendData(led) {"
  "var xhttp = new XMLHttpRequest();"
  "xhttp.onreadystatechange = function() {"
    "if (this.readyState == 4 && this.status == 200) {"
     "document.getElementById('LEDState').innerHTML = this.responseText;" \
    "}" \
  "};" \
  "xhttp.open('GET', 'LedOne?LEDstate='+led, true);"
  "xhttp.send();" \
"}" \

"setInterval(function ( ) {" \
  "var xhttp = new XMLHttpRequest();" \
  "xhttp.onreadystatechange = function() {" \
    "if (this.readyState == 4 && this.status == 200) {" \
      "document.getElementById('temperature').innerHTML = this.responseText;" \
    "}" \
  "};" \
  "xhttp.open('GET', '/temperature', true);" \
  "xhttp.send();" \
"}, 10000 ) ;" \

"setInterval(function ( ) {" \
  "var xhttp = new XMLHttpRequest();" \
  "xhttp.onreadystatechange = function() {" \
    "if (this.readyState == 4 && this.status == 200) {" \
      "document.getElementById('humidity').innerHTML = this.responseText;" \
    "}" \
  "};" \
  "xhttp.open('GET', '/humidity', true);" \
  "xhttp.send();"
"}, 10000 ) ;" \



"setInterval(function ( ) {" \
  "var xhttp = new XMLHttpRequest();" \
  "xhttp.onreadystatechange = function() {" \
    "if (this.readyState == 4 && this.status == 200) {" \
      "document.getElementById('luminosity').innerHTML = this.responseText;" \
    "}" \
  "};" \
  "xhttp.open('GET', '/luminosity', true);" \
  "xhttp.send();" \
"}, 10000 ) ;" \



"</script>" \
"</html>" ; \


/*________________________________________________________________________________________________________________*/

//Servidor Web na porta 80

     //Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
    
 ESP8266WebServer server(80);
  
/*________________________________________________________________________________________________________________*/

void handleRoot();              // protótipos de função para manipuladores HTTP

void handleTemperatura();

void handleHumidade();

void handleLuminosidade();

void handleLed();

//void handleNotFound();

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
  //delay(2000);
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
  Serial.println(".");
  Serial.println(".");

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
  
  delay(1000);
    Serial.println(".");
}

/*________________________________________________________________________________________________________________*/

                        //===============================================================
                        // Essa rotina é executada quando você abre seu IP no navegador
                        //===============================================================
                        
                        
void handleRoot() {

  String xstring = Pg_Html;                      //Read HTML contents
  
  server.send(200, "text/html", xstring);        //Send web page
 
}

/*__________________________________________________*/


void handleTemperatura() {

    float newT = dht.readTemperature();
                                                              // Temperatura de leitura como Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
                                                              //se a leitura da temperatura falhar, não altere o valor t
    
    if (isnan(newT)) {
      Serial.println("Falha ao ler o sensor DHT!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
  
  server.send(200, "text/plain", String(t).c_str());            //Enviar valor DH11 apenas para solicitação de cliente ajax

  
}
/*__________________________________________________*/


void handleHumidade() {

  Serial.println("Humidade Atual");

   // Umidade de leitura

    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
    }
    
  server.send(200, "text/plain", String(h).c_str());            //Enviar valor DH11 apenas para solicitação de cliente ajax
  
 }
 
/*__________________________________________________*/


void handleLuminosidade() {

  Serial.println("Luminosidade Atual");

    // Umidade de leitura

  int newL = analogRead(ldr);

  if (isnan(newL)) {
      Serial.println("Falha ao ler do sensor LDR!");
    }
    else {
      l = newL;
      Serial.println(l);
    }
    
 server.send(200, "text/plane", String(l).c_str());                      //Send ADC value only to client ajax request

}

/*__________________________________________________*/


/*
void handleLed() {
  
 String ledState = "OFF";
 String t_state = server.arg("LEDstate");             //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
 
 Serial.println(t_state);
 
 if(t_state == "1")
 {
  digitalWrite(led1Pin,LOW);                           //LED ON
  ledState = "ON";                                     //Feedback parameter
 }
 else
 {
  digitalWrite(led1Pin,HIGH);                          //LED OFF
  ledState = "OFF";                                    //Feedback parameter  
 }
 
 server.send(200, "text/plane", ledState);             //Send web page
}


*/

void handleLed() {                                    // If a POST request is made to URI /LED
  digitalWrite(led1Pin,!digitalRead(led1Pin));      // Change the state of the LED
  
  //server.sendHeader("Location","/");                // Add a header to respond with a new location for the browser to go to the home page again
  //server.send(303);                                 // Send it back to the browser with an HTTP status 303 (See Other) to redirect

  outputD5State = digitalRead(led1Pin);

  if (outputD5State == "LOW"){
    
    valorLed = 0;
    Serial.println(valorLed);
  
  }else{

    valorLed = 1;
    Serial.println(valorLed);
  }
  
 server.send(200, "text/plane" , outputD5State);             //Send web page
  
}

/*________________________________________________________________________________________________________________*/


                        //==============================================================
                        //                          SETUP
                        //==============================================================

  
void setup(){
  
  // Serial port for debugging purposes
  Serial.begin(115200);
  delay(100);


  InicializaPinos();
  
  InicializaWifi();
  
  InicializaMDNS();
  
  //InicializaServidor();

  server.on("/LedOne", HTTP_POST, handleLed);                             // Call the 'handleLED' function when a POST request is made to URI "/LED"
  
  server.on("/", HTTP_GET, handleRoot);                                 // Chame a função 'handleRoot' quando um cliente solicitar URI "/"
  

  server.on("/temperature", HTTP_GET, handleTemperatura);               // Chame a função 'handleTemperatura' quando uma solicitação GET for feita para o URI "/ temperature"

  server.on("/humidity", HTTP_GET, handleHumidade);

  
  server.on("/luminosity", HTTP_GET, handleLuminosidade);


  

  
/*
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

*/
  

  //server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  InicializaServidor();

  
}

/*________________________________________________________________________________________________________________*/

                        //==============================================================
                        //                          LOOP
                        //==============================================================


void loop(void){

  

  yield();                                      // Funcao extremamente importante no ESP8266, ela garante que o sistema atenda todas as funcoes que 
                                                // correm por traz do nosso programa, garantindo  estabilidade do sistema.
                                                
  server.handleClient();                        // FUNCAO DA BIBLIOTECA webServer que "ATENDE" a nossa requisicao quando acessamos o ip do webserver criado 
                                                // pelo nosso Nodemcu.






  
}
