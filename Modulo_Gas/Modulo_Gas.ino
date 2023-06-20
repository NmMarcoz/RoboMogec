#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> //Biblioteca para o ESP funcionar como servidor
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"

#include "addons/RTDBHelper.h"


#define API_KEY "AIzaSyA8Za8tg6Y7GU22MrpDT8J9jKGFXHthpn8"
#define DATABASE_URL  "https://mogec-f96d1-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;


int sensor_gas = A0;
int digital_gas = D0;
int nivel_normal = 30;

const char* ssid = "GVIEIRA 2G";  // Rede WiFi
const char* password = "tariq234";  //Senha da Rede WiFi

ESP8266WebServer server(80); //server na porta 80


void setup() {

  Serial.begin(9600); //Inicializa a comunicação serial
  delay(50); // ?Intervalo para aguardar a estabilização do sistema
  pinMode(sensor_gas, INPUT);
  pinMode(digital_gas, INPUT);

 

  Serial.println("Conectando a Rede: "); //Imprime na serial a mensagem
  Serial.println(ssid); //Imprime na serial o nome da Rede Wi-Fi

  WiFi.begin(ssid, password); //Inicialização da comunicação Wi-Fi

  //Verificação da conexão
  while (WiFi.status() != WL_CONNECTED) { //Enquanto estiver aguardando status da conexão
    delay(1000);
    Serial.print("."); //Imprime pontos
  }
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); //Função para exibir o IP da ESP

  server.on("/", handle_OnConnect); //Servidor recebe uma solicitação HTTP - chama a função handle_OnConnect
  server.onNotFound(handle_NotFound); //Servidor recebe uma solicitação HTTP não especificada - chama a função handle_NotFound

  server.begin(); //Inicializa o servidor
  Serial.println("Servidor HTTP inicializado");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("signUp OK");
    signupOK = true;
  }else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);


}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 200 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    //___________________
     int valor_analogico = analogRead(sensor_gas);
     valor_analogico = map(valor_analogico, 0, 1023, 0, 100);

     if(Firebase.RTDB.setInt(&fbdo, "Sensor_de_Gas/Valor_de_Leitura", valor_analogico)){
      Serial.println(); Serial.print(valor_analogico);
      Serial.print(" -- Salvo com sucesso em " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
     }else{
      Serial.println("FAILED: " + fbdo.errorReason());
     }
    
  }
}

void handle_OnConnect() {
  int valor_sensor_gas = map(analogRead(sensor_gas), 0, 1023, 0, 100);
  server.send(200, "text/html", EnvioHTML(valor_sensor_gas)); //Envia as informações usando o código 200, especifica o conteúdo como "text/html" e chama a função EnvioHTML

}

void handle_NotFound() { //Função para lidar com o erro 404
  server.send(404, "text/plain", "Não encontrado"); //Envia o código 404, especifica o conteúdo como "text/pain" e envia a mensagem "Não encontrado"

}

String EnvioHTML(float Gas_stat) { //Exibindo a página da web em HTML
  String ptr = "<!DOCTYPE html> <html>\n"; //Indica o envio do código HTML
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n"; //Torna a página da Web responsiva em qualquer navegador Web
  ptr += "<meta http-equiv='refresh' content='2'>";//Atualizar browser a cada 2 segundos
  ptr += "<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr += "<title>Monitor de Temperatura e Umidade</title>\n"; //Define o título da página

  //Configurações de fonte do título e do corpo do texto da página web
  ptr += "<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #000000;}\n";
  ptr += "body{margin-top: 50px;}\n";
  ptr += "h1 {margin: 50px auto 30px;}\n";
  ptr += "h2 {margin: 40px auto 20px;}\n";
  ptr += "p {font-size: 24px;color: #000000;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Monitor de Temperatura e Umidade</h1>\n";
  ptr += "<h2>NODEMCU ESP8266 Web Server</h2>\n";

  //Exibe as informações de temperatura e umidade na página web
  ptr += "<p><b>Leitura do sensor de Gás: </b>";
  ptr += (float)Gas_stat;
  ptr += " %</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;

}
