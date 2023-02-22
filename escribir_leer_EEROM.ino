#include <EEPROM.h>
String nombre;
String cedula;


//---------------------GUARDAR CONFIGURACION-------------------------
void guardar_conf() {
   
  grabar(0,nombre);
 // grabar(16,cedula);

}

void grabar(int addr, String a) {
  int tamano = a.length(); 
  char inchar[14]; 
  a.toCharArray(inchar, tamano);
  
  for (int i = 0; i < 15; i++) {
       EEPROM.write(i, inchar[i]);
  }
  EEPROM.commit();
}

void borrar() {
 
  for (int i = 0; i <= 1024; i++) {
       EEPROM.write(i, 255);
  }
  EEPROM.commit();
}

String leer(int addr) {
   byte lectura;
   String strlectura;
   for (int i = addr; i < 15; i++) {
      lectura = EEPROM.read(i);
      if (lectura != 255) {
        strlectura += (char)lectura;
      }
   }
   return strlectura;
}

void setup() {
   EEPROM.begin(4096);//ESP8266
   if(leer(0) == "")
   {
    nombre = "Nuevo";
    cedula = "12345";
    guardar_conf();
   }
   Serial.begin(115200);
  while (!Serial) {
    
  }
Serial.println("");
}

void loop() {
String palabra = Serial.readStringUntil('#');
    if( palabra == "memoria") {
      Serial.println(leer(0));
     //Serial.println(leer(50));
      }
     if( palabra == "guardar") {
      nombre = Serial.readStringUntil('#');
     //cedula = Serial.readStringUntil('#');
        guardar_conf();
    }
     if( palabra == "borrar") {
     borrar();
    }
    
}
