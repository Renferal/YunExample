/*
  Gestione REST shield I/O Arduino Yun con Bridge 
  Lavora congiuntamente alla pagina web index.htm
  Ricordarsi di creare una cartella /www nella cartella 
  dello sketch ed inserirvi la pagina web. In queesto modo
  verra' caricata dall'IDE sulla micro SD Card in automatico. 
*/

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

// array per la gestione dello stato dei pin
int dac[] = {0, 0, 0, 0, 0, 0, 0};
int DigitalPin[] = {2, 4, 7, 8, 9, 10, 11, 12, 13};   
int DacPin[] = {3, 5, 6};  

// Instanzia oggetto server
YunServer server;

void setup() {

  // Pin di ingresso
  pinMode(3,INPUT);  
  pinMode(4,INPUT);  
  pinMode(7,INPUT);  
  // Pin uscite rele'
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
    
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  // Attiva il socket per l'ascolto dei messaggi REST
  // sulla porta 5555, solo da localhost
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  // Lo scketch si mette in attesa di messaggi entranti
  // attivando un client verso Yunserver
  YunClient client = server.accept();
  // Nuovo messaggio in arrivo?
  if (client) {
    // Richiamo funzione process() per gestire il messaggio
    process(client);
    // Alla fine chiude il client
    client.stop();
  }
  delay(50); // Un attimo di pausa
}

void process(YunClient client) {
  // Legge il contenuto del comando REST fino alla prima /
  // A seconda della richiesta richiama la funzione corretta
  String command = client.readStringUntil('/');
  // comando "digital"
  if (command == "digital") {
    digitalCommand(client);
  }
  // comando "analog"
  if (command == "analog") {
    analogCommand(client);
  }
  // comando "dac"
  if (command == "dac") {
    dacCommand(client);
  }
  // comando "stato"
  if (command == "stato") {
    statoCommand(client);
  }
}
// funzione  trattamento comando "digital"
void digitalCommand(YunClient client) {
  int pin, value;
  // Lettura numero pin
  pin = client.parseInt();
  // Se troviamo una seconda "/" significa che il comando
  // richiede un'azione come: "/digital/13/1"
  // Recuperiamo il valore da assegnare al pin
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  } 
  else {
    value = digitalRead(pin);
  }
  // Invio del messaggio di ritorno al browser
  client.print(F("digital,"));
  client.print(pin);
  client.print(F(","));
  client.println(value);
}
// funzione  trattamento comando "dac"
void dacCommand(YunClient client) {
  int pin, value;
  // Lettura numero pin
  pin = client.parseInt();
  // Se troviamo una seconda "/" significa che il comando
  // richiede un'azione come: "/dac/3/200"
  // Recuperiamo il valore da assegnare al pin
  // In caso di scrittura salviamo  il valore anche nell'array
  // In caso di lettura lo leggiamo dall'array
  if (client.read() == '/') {
    value = client.parseInt();
    dac[pin] = value;
    analogWrite(pin, value);
  } 
  else {
    value = dac[pin];
  }
  // Invio del messaggio di ritorno al browser
  client.print(F("dac,"));
  client.print(pin);
  client.print(F(","));
  client.println(value);
}
// funzione  trattamento comando "analog"
void analogCommand(YunClient client) {
  int pin, value;
  // Lettura numero pin
  pin = client.parseInt();
  // Se troviamo una seconda "/" significa che il comando
  // richiede un'azione come: "/analog/3/200"
  // Solo per sicurezza, in realta la scrittura analogica la 
  // esegue la funzione "dac"
  // Recuperiamo il valore da assegnare al pin
  if (client.read() == '/') {
    value = client.parseInt();
    analogWrite(pin, value);
    client.print(F("analog,"));
    client.print(pin);
    client.print(F(","));
    client.println(value);
  }
  else {
    // Legge il valore del pin di ingresso analogico
    value = analogRead(pin);
    // Invio del messaggio di ritono al browser
    client.print(F("analog,"));
    client.print(pin);
    client.print(F(","));
    client.println(value);
  }
}
// legge lo stato di tutti i pin e li 
// invia in un unico, lungo messaggio
void statoCommand(YunClient client) {
  int pin, value;
    client.print(F("stato"));
    // Lettura pin digitali, ingressi e rele'
    for (int thisPin = 0; thisPin < 9; thisPin++) {
        pin = DigitalPin[thisPin];
        value = digitalRead(pin);
        client.print(F("#"));
        client.print(pin);
        client.print(F("="));
        client.print(value);
        } 
    // Lettura pin dac dall'array        
    for (int thisPin = 0; thisPin < 3; thisPin++) {
        pin = DacPin[thisPin];
        value = dac[pin];
        client.print(F("#"));
        client.print(pin);
        client.print(F("="));
        client.print(value);
        } 
    // Lettura pin analogici        
    for (int thisPin = 0; thisPin < 6; thisPin++) {
       value = analogRead(thisPin);
       value = analogRead(thisPin);
       client.print(F("#A"));
       client.print(thisPin);
       client.print(F("="));
       client.print(value);
    }     
    // Chiusura messaggio con fine linea      
    client.println("");
}    



