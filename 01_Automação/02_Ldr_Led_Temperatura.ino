// Import required libraries


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <ESP8266mDNS.h>                    //Biblioteca que permite chamar o seu modulo ESP8266 na sua rede pelo nome ao inves do IP.

#include <WiFiClient.h>

#include <ESP8266WiFiMulti.h>

#include <Adafruit_Sensor.h>
#include "DHT.h"





#define ldr 0
#define led1Pin 14                              // GPIO14 do ESP8266 e D5 do NodeMCU


//________________________________________________________________________________________________________________
 
//GPIO do NodeMCU que o pino de comunicação do sensor está ligado.

#define DHTPIN 5                                // Pino digital conectado ao sensor DHT

#define DHTTYPE DHT11   // DHT 11


/*
 * Variáveis e objetos globais
 */

//objeto para comunicação com sensor DHT11 

DHT dht(DHTPIN, DHTTYPE);


// temperatura e umidade atuais, atualizadas no loop ()
float t = 0.0;
float h = 0.0;


//________________________________________________________________________________________________________________

String statusLed;
int luminosidade;


//________________________________________________________________________________________________________________
const char* ssid = "net";            // Nome da Rede
const char* password = "9999999999";          //Password da rede
const char* host = "projeto";


//Definikção de Ip fixo para o NodeMCU

  IPAddress ip(100,0,0,105;
  IPAddress gateway(10,0,0,1);                //GATEWAY de conexão
  IPAddress subnet(255,255,255,0);



//--------------------------------------------------------------------

// Criar objeto WiFiServer na porta 80

  WiFiServer server(80);                    //Servidor Web na porta 80

  //ESP8266WebServer server(80);


//--------------------------------------------------------------------

// Geralmente, você deve usar "sem assinatura por muito tempo" para variáveis ​​que mantêm o tempo
// O valor rapidamente se tornará muito grande para um int armazenar

unsigned long previousMillis = 0;    // will store last time DHT was updated


// Updates DHT readings every 10 seconds

const long interval = 10000;  

//--------------------------------------------------------------------

  ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

  

//////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////

void run_html(WiFiClient client){

    String html_content = \
     "<!DOCTYPE html><html>" \ 
      "<meta name='viewport' content='width=device-width, initial-scale=1'>" \
        "<head><style media='screen' type='text/css'>" \  
            "html{display:inline-block;margin:10px auto;text-align:center;}" \
            "h2 { font-size: 3.0rem; }" \
            "p { font-size: 3.0rem; }" \
            ".units { font-size: 1.2rem; }" \
            ".dht-labels{font-size: 1.5rem;vertical-align:middle;padding-bottom: 15px;}" \  
        "</style></head>" \   
        "<body>" \ 
            "<h1 style='font-size:40px'>Acionamento LED</h1>" \  
            "<p><a href='/LED_ON'>" \ 
                "<button style='width:100px;font-size:20px'>ON "+statusLed+"</button>" \  
            "</a></p>" \ 
            "<p><a href='/LED_OFF'>" \ 
                "<button style='width:100px;font-size:20px'>OFF "+statusLed+"</button>" \  
            "</a></p>" \
            "<p style='font-size:40px'>LED "+luminosidade+" " \ 
            "</p>" \


            "<h2>ESP8266 DHT Server</h2>" \
              "<p><i class='fas fa-thermometer-half' style='color:#059e8a;'></i>" \
                "<span class='dht-labels'>Temperatura</span>" \
                "<span id='temperature'>"+t+"</span>" \
                "<sup class='units'>&deg;C</sup>" \
              "</p>" \
              "<p><i class='fas fa-tint' style='color:#059e8a;'></i>" \
                "<span class='dht-labels'>Humidade</span>" \
                "<span id='temperature'>"+h+"</span>" \
                "<sup class='units'>%</sup>" \
              "</p>" \

            
        "</body>" \ 

            
            "<script>" \
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
              "xhttp.send();" \
            "}, 10000 ) ;" \
            "</script>" \


"</html>)rawliteral";

    client.println(html_content); 

}

//////////////////////////////////////////////////////////////////////////////////////////////////

void InicializaPinos(){


  
  pinMode(ldr, INPUT);
  pinMode(led1Pin, OUTPUT);                     // Define o D5 como saída
  
  digitalWrite(led1Pin, LOW);                   // O LED começa desligado
  
  delay(100);
  
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void InicializaWifi() {
  

// Captura do endereço MAC
  
  Serial.println();
  delay(1000);
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

//////////////////////////////////////////////////////////////////////////////////////////////////


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

//////////////////////////////////////////////////////////////////////////////////////////////////

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
//////////////////////////////////////////////////////////////////////////////////////////////////



//--------------------------------------------------------------------

//--------------------------------------------------------------------

void setup() {
  
  Serial.begin(115200);
  delay(500);

  InicializaPinos();
  InicializaWifi();
  InicializaMDNS();
  InicializaServidor();



  
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {


   unsigned long currentMillis = millis();
   
  if (currentMillis - previousMillis >= interval) {

    
    
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    

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
  }

  //----------------------------------------------------------------------

 
// Verificação se o cliente está conectado

  WiFiClient client = server.available();

    if (!client) {                                        // Verifica se o cliente está conectado ao servidor, executa este ciclo até estar conectado

      return;

    }


// Espera até o cliente enviar dados

  Serial.println("novo cliente");                         //Apresenta esta mensagem quando o cliente se liga ao servidor

    while(!client.available()){

      delay(1);

    }

 
// Ler a primeira linha do pedido

  String request = client.readStringUntil('\r');

  Serial.println(request);

  client.flush();

 

// Operação do pedido

  int value = LOW;


    if (request.indexOf("/LED") >= 0)  {
    
    digitalWrite(led1Pin, !digitalRead(led1Pin)); // Se o pedido no LedPin for LED=ON, acende o LED
    
    //value = HIGH;
    
    }
    
    
    int luminosidade = analogRead(ldr);
    
                    
                    // iniciamos a resposta http com o código OK(200), 
                    // o tipo de conteúdo a ser enviado e tipo de conexão.
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type:text/html");
                    client.println("Connection: close");
                    client.println();
                    
                    // INSIRA AQUI SUA APLICAÇÃO



                    //envia ao client conteúdo HTML
                    run_html(client);






        
       // cabecalho = "";               //ao encerrar conexão, limpa variável cabecalho
       // client.flush(); 
       // client.stop();
       // Serial.println("Client desconectado."); 
      //  Serial.println();

      

}


//////////////////////////////////////////////////////////////////////////////////////////////////





