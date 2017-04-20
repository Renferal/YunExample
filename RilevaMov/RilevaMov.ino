/*
Sketch gestione notifica movimenti da motion
*/
 
#include <FileIO.h>
#include <Process.h>

int led_rosso = 13; // LED di avviso di motion detection
int buttonPin = 7; // Pulsnte reset LED rosso  
char buttonState = 0;        
String stato = "";

void setup() {
  // Setup Bridge richiama bridge.py su Linino
  Bridge.begin();

  pinMode(led_rosso, OUTPUT); // Imposta pin 5 come OUTPUT

  // Setup File IO richiama files.py su Linino 
  FileSystem.begin();
}  

void loop() {
 
  // Istanza oggetto file e Open del file in lettura
  File nome = FileSystem.open("/mnt/sda1/pgm/allarme.txt", FILE_READ); 
  stato = "";    // Lettura di un carattere dal file 
  stato += (char)nome.read();
  nome.close();  // chiude il file allarme.txt 
       
  if (stato == "1") {  // Se stato 1 e stato rilevato un movimento
     digitalWrite(led_rosso, HIGH);  // accende il LED rosso di avviso 
     Process zero_alarm;  // instanzia oggetto Process 
     zero_alarm.begin("python");  // Comando che si richiede di eseguire a Linino
     zero_alarm.addParameter("/mnt/sda1/pgm/allarme_off.py"); // Parametri comando
     zero_alarm.run();  // Richiesta di esecuzione del comando
  }
     
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {         // Se viene premuto il pulsante
     digitalWrite(led_rosso, LOW);  // viene spento il LED rosso
  }
  delay(2000); // Ritardo di 2 secondi ad ogni ciclo
}




