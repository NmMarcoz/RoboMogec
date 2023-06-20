//----------------------------------- mogec robot ------------------------------------------------------------
//----------------------------------- versao 1.2  ------------------------------------------------------------

// Motores DC declarados em : 3,5,6,9
// Servo Motor declarado no pino 10
// ULtrassonico utilizando os pinos 12 e 13
// O grau do motor servo utilizado não está exato, por isso valores como 90 graus serão tratados como 104 neste código.
// ALimentação do robô feita por uma bateria 9V. ATENÇÃO: ESTA BATERIA ALIMENTA TANTO A SHIELD QUANTO O ARDUINO, JAMAIS LIGAR O INTERRUPTOR QUANDO ESTIVER COM A PLACA LIGADA NO COMPUTADOR.
// MODELO DA PONTE H: L289n
// Velocidade do robo esta sendo trabalhada com 200 e 255
// O código conta com vários prints no console serial para debugar. Sinta-se a vontade para monitorar as decisões do robô por lá, E NAO LIGUE O INTERRUPTOR ENQUANTO ESTIVER NO PC
//aa



//-----------------------------------BIBLIOTECAS--------------------------------------------------------------
#include <Servo.h> // inclui biblioteca de manipulação de servos motores.
#include <AFMotor.h> // inclui biblioteca de manipulação de motores DCs.
#include <Ultrasonic.h> // Biblioteca do ultrassonico ADAFRUIT



 //Definindo os pinos
//-----------------------------------OBJETOS DOS SENSORES E MOTORES--------------------------------------------------------------
HC_SR04 distancia(12,13); //Porta 12: Trigger, Porta 13: Echo


AF_DCMotor motor1(1); // Define o motor1 ligado ao M1
AF_DCMotor motor2(2); // Define o motor2 ligado ao M2

Servo servo_ultra_sonico; // nomeando o servo motor
 
//------------------------------------VARIAVEIS--------------------------------------------------------------
int leituraDoSonar = distancia.distance(); // Declarando a leitura do sonar como a função distancia do objeto Ultrassonico
int TempoGirar = 250;//esse é o tempo para o robô girar em 90º com uma bateria de 9v.
int distanciaObstaculo = 20; //distância para o robô parar e recalcular o melhor caminho
int esquerda = 0;
int direita = 0;
int centro = 0;

//------------------------------------CLASSE DO MOTOR--------------------------------------------------------------
class DCMotor{
  int spd = 255, pin1, pin2;
  public:  
    void Pinout(int in1, int in2){ // Pinout é o método para a declaração dos pinos que vão controlar o objeto motor
      pin1 = in1;
      pin2 = in2;
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
      }   
    void Speed(int in1){ // Speed é o método que irá ser responsável por salvar a velocidade de atuação do motor
      spd = in1;
      }     
    void Forward(){ // Forward é o método para fazer o motor girar para frente

      digitalWrite(pin1, LOW);
      analogWrite(pin2, spd);
      }   
    void Backward(){ // Backward é o método para fazer o motor girar para trás
      analogWrite(pin1, spd);
      digitalWrite(pin2, LOW);
      }
    void Stop(){ // Stop é o metodo para fazer o motor ficar parado.
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
    }
   
};
DCMotor Motor1, Motor2;

void setup(){
 Serial.begin(9600); // inicializa a comunicação serial para mostrar dados
 //PINAGEM
 Motor1.Pinout(3,5);
 Motor2.Pinout(6,9);
 servo_ultra_sonico.attach(10); // Define o mini servo motor ligado no pino digital 10.
 servo_ultra_sonico.write(104);// O servo do sensor se inicia a 90 graus (meio)
 Motor1.Stop();
 Motor2.Stop();
 
 delay(500);
 Serial.print("Começando!");
 rotacao_Parado; //inica com os motores parados
}


 // Função principal do Arduino
void loop(){
  //teste_rotacao();
 Motor1.Speed(200);
 Motor2.Speed(200);
 pensar(); 
}





void reposicionaServoSonar(){
 servo_ultra_sonico.write(104);
 rotacao_Parado();
 delay(200);
}

void posicionaCarroMelhorCaminho(){
  char melhorDist = calculaMelhorDistancia();
   Serial.println("_______ANALISANDO A MAIOR DISTANCIA_______");
   Serial.print("melhor Distancia em cm: ");
   Serial.println(melhorDist);
   if (melhorDist == 'c'){
    rotacao_Frente();
   }else if (melhorDist == 'd'){
    rotacao_Direita();
    rotacao_Frente();
   }else if (melhorDist == 'e'){
    rotacao_Esquerda();
    rotacao_Frente();
   }else{
    rotacao_Re();
   }
   reposicionaServoSonar();
}

//------------------------------------FUNÇÕES DE CÁLCULO DE DISTANCIA--------------------------------------------------------------

char calculaMelhorDistancia(){
  Serial.println("__________INICIANDO TAREFA DE DECISAO__________");
  esquerda = calcularDistanciaEsquerda();
  centro = calcularDistanciaCentro();
  direita = calcularDistanciaDireita();
  reposicionaServoSonar();
  int maiorDistancia = 0;
  char melhorDistancia = '0';
  
  if (centro > direita && centro > esquerda){
    melhorDistancia = 'c';
    maiorDistancia = centro;
  }else
  if (direita > centro && direita > esquerda){
    melhorDistancia = 'd';
    maiorDistancia = direita;
  }else
  if (esquerda > centro && esquerda > direita){
    melhorDistancia = 'e';
    maiorDistancia = esquerda;
  }
  if (maiorDistancia <= distanciaObstaculo) { //distância limite para parar o robô
    rotacao_Re();
    posicionaCarroMelhorCaminho();
  }
  reposicionaServoSonar();
  return melhorDistancia;
}

int calcularDistanciaDireita(){
   servo_ultra_sonico.write(0);
   delay(200);
   leituraDoSonar = distancia.distance();
   delay(20);
   leituraDoSonar = distancia.distance();
   delay(20);
   Serial.print("Distancia da Direita: ");
   Serial.println(leituraDoSonar);
   return leituraDoSonar;
 }

 // Função para calcular a distância da esquerda
int calcularDistanciaEsquerda(){
 servo_ultra_sonico.write(180);
 delay(200);
 leituraDoSonar = distancia.distance();
 delay(20);
 leituraDoSonar = distancia.distance(); 
 delay(20);
 Serial.print("Distancia Esquerda: ");
 Serial.println(leituraDoSonar);
 return leituraDoSonar;
}

int calcularDistanciaCentro(){
 servo_ultra_sonico.write(90);
 delay(200);
 leituraDoSonar = distancia.distance();
 delay(20);
 leituraDoSonar = distancia.distance();
 delay(20);
 Serial.print("Distancia do Centro: "); // Exibe no serial
 Serial.println(leituraDoSonar);
 return leituraDoSonar;
}

//------------------------------------FUNÇÕES DE MOVIMENTO--------------------------------------------------------------

void rotacao_Parado(){
 Serial.println(" Motor: Parar ");
 Motor1.Stop();
 Motor2.Stop();
}


 // Função para fazer o robô andar para frente
void rotacao_Frente()
 {
 rotacao_Parado();
 delay(500);
 Motor1.Speed(255);
 Motor2.Speed(255);
 Serial.println(" INDO PARA FRENTE");
 Serial.println(".");
 Serial.println(".");
 Serial.println(".");
 Serial.println(".");
 Motor1.Forward();
 Motor2.Forward();
 delay(500);
 }

 void rotacao_Frente_semDelay(){
 Motor1.Speed(255);
 Motor2.Speed(255);
 Serial.println(" INDO PARA FRENTE");
 Serial.println(".");
 Serial.println(".");
 Serial.println(".");
 Serial.println(".");
 Motor1.Forward();
 Motor2.Forward();
}

 // Função que faz o robô andar para trás e emite som quando ele dá ré
void rotacao_Re()
 {
 Serial.println(" VOLTANDO ");
 for (int i=0; i <= 3; i++){
 delay(100);
 //Motor1.Speed(200);
 //Motor2.Speed(200);
 Motor1.Backward();
 Motor2.Backward();
 delay(100);
 }
 rotacao_Parado();
 }



 // Função que faz o robô virar à direita
void rotacao_Direita()
 {
// Motor1.Speed(200);
 //Motor2.Speed(200);
 Motor1.Backward();
 Motor2.Backward();
 delay(100);
 rotacao_Parado();
 delay(500);
 Motor1.Speed(255);
 Motor2.Speed(255);
 Serial.println(".");
 Serial.println(".");
 Serial.println(".");
 Serial.println(".");
 Serial.println(" INDO PARA A DIREITA");
 Motor1.Forward();
 Motor2.Backward();
 delay(TempoGirar);
 }

 // Função que faz o robô virar à esquerda
void rotacao_Esquerda()
 {
 Motor1.Backward();
 Motor2.Backward();
 delay(100);
 rotacao_Parado();
 delay(500);
 Motor1.Speed(255);
 Motor2.Speed(255);
 Serial.println(".");
 Serial.println(".");
 Serial.println(".");
 Serial.println(".");
 Serial.println(" INDO PARA A ESQUERDA ");
 Motor2.Forward();
 Motor1.Backward();
 delay(TempoGirar);
 }

void teste_rotacao(){
  Motor1.Speed(255);
  Motor2.Speed(255);
  Motor1.Forward();
  Motor2.Backward();
  delay(TempoGirar);
  Motor1.Stop();
  Motor2.Stop();
  delay(1000);
}

//------------------------------------FUNÇÕES COGNITIVAS--------------------------------------------------------------

void pensar(){
 reposicionaServoSonar(); //Coloca o servo para olhar a frente
 Serial.print("distancia em cm: "); 
 Serial.println(leituraDoSonar); // Exibe no serial monitor
 if (leituraDoSonar > distanciaObstaculo) { // Se a distância for maior que 20 cm
  leituraDoSonar = distancia.distance();
  Serial.print(leituraDoSonar);
  while (leituraDoSonar > distanciaObstaculo){
      rotacao_Frente_semDelay();
      leituraDoSonar = distancia.distance();
      Serial.print("a");
  }

 }else{
  delay(200);
  posicionaCarroMelhorCaminho(); //calcula o melhor caminho
  pensar();
 }
}





//Fim
//Versão do Software
