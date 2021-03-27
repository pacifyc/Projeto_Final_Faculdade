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
//float t = 0.0;
//float h = 0.0;

//variáveis que armazenam os valores máximo e mínimo de temperatura registrados.
float TemperaturaMax;
float TemperaturaMin;


  float TemperaturaLida;
  float UmidadeLida;
              

//prototypes
void AtualizaTemperaturaMaxEMinima(float TempLida);
void EnviaMedicoesParaSerial(float TempLida, float UmidLida, float TempMax, float TempMin);

//________________________________________________________________________________________________________________

String statusLed;
int luminosidade;


//________________________________________________________________________________________________________________
const char* ssid = "Selmy_neonet";            // Nome da Rede
const char* password = "58108c3ac162";          //Password da rede
const char* host = "projeto";


//Definikção de Ip fixo para o NodeMCU

  IPAddress ip(10,0,0,115);
  IPAddress gateway(10,0,0,1);                //GATEWAY de conexão
  IPAddress subnet(255,255,255,0);



//--------------------------------------------------------------------

// Criar objeto WiFiServer na porta 80

  WiFiServer server(80);                    //Servidor Web na porta 80


//--------------------------------------------------------------------

// Geralmente, você deve usar "sem assinatura por muito tempo" para variáveis ​​que mantêm o tempo
// O valor rapidamente se tornará muito grande para um int armazenar

unsigned long previousMillis = 0;    // will store last time DHT was updated


// Updates DHT readings every 10 seconds

const long interval = 10000;  

//--------------------------------------------------------------------

  ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

  

//////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Implementações
 */
 
//Função: verifica se os valores de temperatura máxima e mínima devem ser atualizados
//Parâmetros: temperatura lida
//Retorno: nenhum

void AtualizaTemperaturaMaxEMinima(float TempLida){
  
  if (TempLida > TemperaturaMax)
    TemperaturaMax = TempLida;

  if (TempLida < TemperaturaMin)
    TemperaturaMin = TempLida;  
}


//Função: envia, na forma de mensagens textuais, as medições para a serial
//Parâmetros: - Temperatura lida
//            - Umidade relativa do ar lida
//            - Máxima temperatura registrada
//            - Mínima temperatura registrada
//Retorno: nenhum

void EnviaMedicoesParaSerial(float TempLida, float UmidLida, float TempMax, float TempMin){
  
  char Mensagem[200];
  char i;

  //pula 80 linhas, de forma que no monitor serial seja exibida somente as mensagens atuais (impressao de refresh de tela)
  for(i=0; i<80; i++)
    Serial.println(" ");

  //constrói mensagens e as envia
  //- temperatura atual
  memset(Mensagem,0,sizeof(Mensagem));
  sprintf(Mensagem,"- Temperatura: %d C",(int)TempLida);
  Serial.println(Mensagem);
  
  //- umidade relativa do ar atual
  memset(Mensagem,0,sizeof(Mensagem));
  sprintf(Mensagem,"Umidade atual: %d \\%",(int)UmidLida);
  Serial.println(Mensagem);
  
  //- temperatura maxima
  memset(Mensagem,0,sizeof(Mensagem));
  sprintf(Mensagem,"- Temperatura maxima: %d C",(int)TempMax);
  Serial.println(Mensagem); 
  
  //- temperatura minima
  memset(Mensagem,0,sizeof(Mensagem));
  sprintf(Mensagem,"- Temperatura minima: %d C", (int)TempMin);
  Serial.println(Mensagem);
}


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
                "<button style='width:400px;font-size:80px'>ON "+statusLed+"</button>" \  
            "</a></p>" \ 
            "<p><a href='/LED_OFF'>" \ 
                "<button style='width:400px;font-size:80px'>OFF "+statusLed+"</button>" \  
            "</a></p>" \
            "<p style='font-size:80px'> LED "+luminosidade+" " \ 
            "</p>" \


            "<h2>ESP8266 DHT Server</h2>" \
              "<p><i class='fas fa-thermometer-half' style='color:#059e8a;'></i>" \
                "<span class='dht-labels'>Temperatura</span>" \
                "<span id='temperature'>"+TemperaturaLida+"</span>" \
                "<sup class='units'>&deg;C</sup>" \
              "</p>" \
              "<p><i class='fas fa-tint' style='color:#059e8a;'></i>" \
                "<span class='dht-labels'>Humidade</span>" \
                "<span id='temperature'>"+UmidadeLida+"</span>" \
                "<sup class='units'>   +%+ </sup>" \
              "</p>" \

            
        "</body>" \ 
    "</html>";

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

  delay(500);

  WiFi.begin(ssid, password);                   // Inicia a ligação a rede
  Serial.println("Comunicação iniciada...!");
  Serial.println("Conectando com a rede WiFi...!");

  int i = 0;  
  while (WiFi.status() != WL_CONNECTED) {
    
    delay(500);
  
    Serial.print(".");                //Enquanto a ligação não for efectuada com sucesso é apresentado no monitor série uma sucessão de “.”
    Serial.print(++i); 
    Serial.print(' ');
  
  }

  Serial.println("");
  Serial.println("WiFi conectado");         // Se a ligação é efectuada com sucesso apresenta esta mensagem no monitor série
  delay(100);
  
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
  
  Serial.print("http://");
  Serial.print(host);
  Serial.print(".local/");
  
  delay(100);
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
  delay(100);

  InicializaPinos();
  InicializaWifi();
  InicializaMDNS();
  InicializaServidor();


//inicializa temperaturas máxima e mínima com a leitura inicial do sensor

  TemperaturaMax = dht.readTemperature();
  TemperaturaMin = TemperaturaMax;
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

    //Verifica se existe algum client na rede 
    WiFiClient client = server.available(); 
      
    //Caso positivo, imprime "New Client" no monitor
    if (client){   
                                 
        Serial.println("Novo Cliente");  
        //Enquanto cliente conectado, verifica se existem bytes a serem lidos     
        //e concatena os bytes recebidos na String cabecalho
        
        String cabecalho;
        luminosidade = analogRead(ldr);
          
        while (client.connected()){
           
            if (client.available()){
              

              //float TemperaturaLida;
              //float UmidadeLida;
              
              //Faz a leitura de temperatura e umidade do sensor
              TemperaturaLida = dht.readTemperature();
              UmidadeLida = dht.readHumidity();
            
              //se houve falha na leitura do sensor, escreve mensagem de erro na serial
              if ( isnan(TemperaturaLida) || isnan(UmidadeLida) ) 
                Serial.println("Erro ao ler sensor DHT22!");
              else
              {
            
                AtualizaTemperaturaMaxEMinima(TemperaturaLida);
                EnviaMedicoesParaSerial(TemperaturaLida, 
                                        UmidadeLida, 
                                        TemperaturaMax, 
                                        TemperaturaMin);
              }
              

                cabecalho += (char)client.read();  
                //Se receber nova linha em branco, encerrou leitura dos dados
                
                if (cabecalho.endsWith("\n\r\n")){
                    Serial.println(cabecalho); //imprime cabeçalhos http recebidos
                    
                    // iniciamos a resposta http com o código OK(200), 
                    // o tipo de conteúdo a ser enviado e tipo de conexão.
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type:text/html");
                    client.println("Connection: close");
                    client.println();
                    
                    // INSIRA AQUI SUA APLICAÇÃO



                    //envia ao client conteúdo HTML
                    run_html(client);

                    if(cabecalho.indexOf("GET /LED_ON")>= 0){
                      digitalWrite(led1Pin, true);
                          statusLed = "Ligado";
                    }else if(cabecalho.indexOf("GET /LED_OFF")>= 0){
                      digitalWrite(led1Pin, false);
                          statusLed = "Desligado";
                    }
   
                                  
                    break;             //sai do while loop
                }
            }
        }
        
        cabecalho = "";               //ao encerrar conexão, limpa variável cabecalho
        client.flush(); 
        client.stop();
        Serial.println("Client desconectado."); 
        Serial.println();
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////








