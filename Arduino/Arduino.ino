//Programa : Medidor de energia elétrica com Arduino e SCT-013
//Baseado no programa exemplo da biblioteca EmonLib

//Carrega as bibliotecas
#include "EmonLib.h" 
#include "SPI.h"
#include "Ethernet.h"

EnergyMonitor emon1;

EthernetClient client;
//Tensao da rede eletrica
int rede = 110.0;

//Pino do sensor SCT
int pino_sct = A1;
char server [] = "microhmrtv.ddns.net"; // meu servidor
String apikey = "c9ec04763e8973fa4c3ee2d754f89c26"; //API key disponibilizada pelo emoncms
int node = 0;
byte mac[] = { 0xA4, 0x28, 0x72, 0xCA, 0x55, 0x2F }; //Endereço MAC da placa ethernet na rede

void setup() 
{
  Serial.begin(9600);
  pinMode(pino_sct,INPUT);
  if (Ethernet.begin(mac) == 0){
    Serial.println("Shield ethernet não inicializado.");
    while(1);// abortar (não temos rede)
  } else {
      Serial.println("Projeto Final de Microprocessadores - Medidor de energia elétrica com arduino.");
      Serial.println("Henrique Kenzo Kuroki");
      Serial.println("Matheus Modesto Oliveira");
      Serial.println("Rafael Rocha da Silva");
      Serial.println("Tasso Nayade da Rosa Santos");
      Serial.println("Victor Mitsuo Asato");
      Serial.println();
      Serial.println("Shield ethernet inicializado com sucesso!");
  }
  delay(1000);
  Serial.print("Endereço IP: ");
  Serial.println(Ethernet.localIP()); // mostra o endereço IP
 
 
  //Pino, calibracao - Cur Const= Ratio/BurdenR. 1800/62 = 29. 
  emon1.current(pino_sct, 29);

} 
 
void loop() 
{ 
   client.connect (server,8082);
   if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  client.stop();
  //Calcula a corrente  
  double Irms = emon1.calcIrms(1480);
  double Potencia = Irms*rede;
  //Mostra o valor da corrente
  Serial.print("Corrente : ");
  Serial.print(Irms); // Irms
  Serial.print(" A ");

  
  //Calcula e mostra o valor da potencia
  Serial.print(" Potencia : ");
  Serial.print(Potencia);
  Serial.println(" W ");

  sendData (Irms, Potencia);

  delay (3000);
}

void sendData (double Irms, double Potencia){
  client.connect (server,8082); //conecta ao servidor
  delay (500);
  if (client.connected()){
    //Envia os dados como parametros através do metodo GET
    client.print("GET /emoncms/input/post?node=medicoes&fulljson=");
    client.print("{\"Corrente\":");
    client.print(Irms);
    client.print(",\"Potencia\":");
    client.print(Potencia);client.print("}");
    client.print("&apikey=");client.print(apikey);

  client.println(" HTTP/1.1");
  client.println("Host: emoncms.org");
  client.println("User-Agent: Arduino-ethernet");
  client.println("Connection: close");
  client.println();

  Serial.println("Dados enviados!!");
  client.stop();
  Serial.println();
  }
    else{Serial.println("Não conectado!");client.stop();}
}
