//control de luminaria navideña 2020
//nodemcu / ESP8166 / ESP-01
//REV 08/12/21 no reconoce el string de las paginas, al colocar la IP funciona
// 


#include <NTPClient.h>//importamos la librería del cliente NTP
#include <ESP8266WiFi.h> //librería necesaria para la conexión wifi
#include <WiFiUdp.h> // importamos librería UDP para comunicar con 
                     // NTP

 byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,20,200);
IPAddress subnet(255,255,255,0); 
IPAddress gateway(192,168,20,1);
WiFiServer servidor(80);

// Pin que se desea modificar mediante el boton

String readString = String(30);
String Estado_Pin = String(8);

int rele = 0;
int control = LOW;


// datos para conectarnos a nuestra red wifi
const char *ssid     = "VTR-0093521";
const char *password = "qsg6xvvwLyQf";

int hora = 0 ;
int minuto = 0;
int h_prende = 20;
int h_apaga = 23;
int m_prende = 00;
int m_apaga = 25;
int la_hora = 0;


//iniciamos el cliente udp para su uso con el server NTP
WiFiUDP ntpUDP;

// cuando creamos el cliente NTP podemos especificar el servidor al // que nos vamos a conectar en este caso
// 0.south-america.pool.ntp.org SudAmerica
// también podemos especificar el offset en segundos para que nos 
// muestre la hora según nuestra zona horaria en este caso
// restamos -10800 segundos CHILE (-3 Hrs verano) 
// y por ultimo especificamos el intervalo de actualización en
// mili segundos en este caso 6000
//1.cl.pool.ntp.org
//ntp.shoa.cl 
//200.54.149.19

NTPClient timeClient(ntpUDP,"200.54.149.19",-10800,6000);


void automatico(){
  if (hora >= h_prende && hora < h_apaga  || Estado_Pin == "APAGAR"){ //agregar && minuto >= m_prende && minuto < m_apaga en caso de control por minutos
//digitalWrite(LED_BUILTIN,HIGH);
Serial.print ( "\n PRENDE LUMINARIA \n" );
digitalWrite(rele,HIGH);

}
 else { 
  digitalWrite(rele,LOW);
  }
}
void NTP_config(){

//timeClient.update(); //sincronizamos con el server NTP

  while (timeClient.update() == false) {
    Serial.println("Esperando NTP");
    delay(1000);
     WiFiClient cliente = servidor.available();
  if (cliente)
  {
       while (cliente.connected())
    {

      if (cliente.available())
      {
        char c = cliente.read();
        if (readString.length() < 30)
        {
          readString.concat(c);        
        }
          cliente.println("HTTP/1.1 200 OK");
          cliente.println("Content-Type: text/html");
          cliente.println();
          cliente.println("<html>");
          cliente.println("<head>");
          cliente.println("<title>ERROR</title>");
          cliente.println("</head>");
          cliente.println("<body width=100% height=100%>");
          cliente.println("<center>");
          cliente.println("<h1>ESPERANDO NTP</h1>");
          cliente.print("</body>");
          cliente.print("</html>");
          //cliente.stop();
          break;
        }
      }
    }
  }
  
//Serial.println();
String hora_completa =  timeClient.getFormattedTime();
Serial.println(hora_completa);

hora = timeClient.getHours();
minuto = timeClient.getMinutes();
}
void html(){
 WiFiClient cliente = servidor.available();
  if (cliente)
  {
   Serial.println("Cliente en linea");
    boolean lineaenblanco = true;
    while (cliente.connected())
    {

      if (cliente.available())
      {
        char c = cliente.read();
        if (readString.length() < 30)
        {
          readString.concat(c);        
        }
        if (c == '\n' && lineaenblanco) //Si la peticion HTTP ha finalizado
        {
          int Dato = readString.indexOf("PIN");
          String LED2 = readString.substring(Dato , Dato + 5);
          if (readString.substring(Dato, Dato + 5) == "PIN=S")
          {
            if (Estado_Pin == "ENCENDER") {
              digitalWrite(rele, HIGH);
              Estado_Pin = "APAGAR";
            }
            else {
              digitalWrite(rele, LOW);
              Estado_Pin = "ENCENDER";
            }
          }
          cliente.println("HTTP/1.1 200 OK");
          cliente.println("Content-Type: text/html");
          cliente.println();
          cliente.println("<html>");
          cliente.println("<head>");
          cliente.println("<title>Luces de navidad</title>");
          cliente.println("</head>");
          cliente.println("<body width=100% height=100%>");
          cliente.println("<center>");
          cliente.println("<h1>Luces de Navidad</h1>");
          cliente.print("<br><br>");
          cliente.print("<input type=submit value=");
          cliente.print(Estado_Pin);
          cliente.print(" style=width:200px;height:75px onClick=location.href='./?PIN=S\'>");
          cliente.print("</center>");
          cliente.print("<p>");
          cliente.println("<center>");
          cliente.println("La hora inicial: ");
          cliente.print(hora);
          cliente.println(":");
          cliente.println(minuto);
          cliente.print("</p>");
          cliente.print("</body>");
          cliente.print("</html>");
          //cliente.stop();
          readString = "";
          break;
        }
      }
    }
  }
}

void setup(){
  
pinMode(rele,OUTPUT);
digitalWrite(rele,HIGH);
delay ( 100 );
   
Serial.begin(115200); // activamos la conexión serial
delay ( 500 );
Serial.print ( "Configurando \n" );

WiFi.begin(ssid, password); // nos conectamos al wifi
WiFi.mode(WIFI_STA);
WiFi.config(ip, gateway, subnet);

// Esperamos hasta que se establezca la conexión wifi
 while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "Esperando conexión \n" );
  }

  servidor.begin();
  
  Serial.print ( "Conectado \n" );
  digitalWrite(rele,LOW);
  Estado_Pin = "ENCENDER";
  Serial.print(WiFi.localIP());
  Serial.print ( "\n" );
  
  timeClient.begin(); 
  
 }

void loop() {
  
NTP_config();
automatico();
html();
delay(1000);
}
