#include <WiFi.h>
#include <PubSubClient.h>

#define PIN_LM35       36 // ESP32 pin GPIO36 (ADC0) connected to LM35
#define lampu_pin       5 // 
#define kipas_pin       18 // 

const char* ssid = "Note8";
const char* password = "mantapjiwa";
const char* mqtt_server = "192.168.221.87";  // Ganti dengan alamat IP Raspberry Pi

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, 1883);

  client.setCallback(callback);
  // client.publish("lampu", "LOW");
  client.subscribe("lampu");
  client.subscribe("kipas");  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  int serialvalue = analogRead(36);
  float temperature = (serialvalue / 4095.0) * 500.0; // Koreksi perhitungan

  char message[50];  // Buat array untuk menyimpan pesan
  snprintf(message, 50, "%.2f", temperature);  // Ubah nilai float menjadi string dengan dua angka desimal

  Serial.print("Temperature: ");
  Serial.println(message);
  client.publish("suhu", message);  // Kirim pesan dengan string hasil konversi
  // Serial.print("Subscribed to lampu: ");
  // Serial.println(client.subscribe("lampu"));
  // client.subscribe("lampu");


  // if (Serial.available() > 0) {
  //   char input = Serial.read();
  //   if (input == '0' || input == '1') {
  //     if (input == '0') {
  //       client.publish("lampu", "LOW");  // Kirim "LOW" jika input adalah '0'
  //     } else {
  //       client.publish("lampu", "HIGH");  // Kirim "HIGH" jika input adalah '1'
  //     }
  //   }
  // }

  client.loop();
  delay(1000);
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // if(topic=="lampu"){
  //   Serial.print("Changing lamp to ");
  //   if(messageTemp == "HIGH"){
  //     digitalWrite(lampu_pin, HIGH);
  //     Serial.print("On");
  //   }
  //   else if(messageTemp == "LOW"){
  //     digitalWrite(lampu_pin, LOW);
  //     Serial.print("Off");
  //   }
  // }
  // Serial.println();
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("lampu");
      client.subscribe("kipas");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
