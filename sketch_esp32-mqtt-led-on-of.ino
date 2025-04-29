// ESP32 MQTT LED Control
// Este código controla um LED interno do ESP32 via MQTT.
// O LED pode ser ligado ou desligado enviando mensagens "on" ou "off" para o tópico especificado.
// Certifique-se de ter as bibliotecas PubSubClient e WiFi instaladas no Arduino IDE.
// Para instalar as bibliotecas, vá em: Sketch -> Include Library -> Manage Libraries
// e busque por "PubSubClient" e "WiFi".
// Para usar este código, você precisará de um broker MQTT. Você pode usar o broker público em http://broker.emqx.io.
// Para testar, você pode usar o MQTT.fx ou qualquer outro cliente MQTT.
// Certifique-se de alterar as credenciais do Wi-Fi e o tópico MQTT conforme necessário.
// Author: Relton Lima
// Date: 2023-10-01
// License: GNU GPLv3
// This program is free software: you can redistribute it and/or modify
#include <WiFi.h>
#include <PubSubClient.h>

// Configurações de Wi-Fi
const char* SSID = "YOU_WIFI_SSID";
const char* PASSWORD = "YOU_WIFI_SSID_PASS";

// Configurações MQTT
const char* MQTT_BROKER = "broker.emqx.io"; // Broker público para teste
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "esp32/led"; // Tópico para enviar comandos
const char* MQTT_CLIENT_ID = "ESP32_LED_CONTROL"; // ID único
const String clientId = String(MQTT_CLIENT_ID) + "_" + String(random(0xffff), HEX);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Pino do LED interno (GPIO 2 para a maioria das placas ESP32)
const int LED_PIN = 2;

// Função para conectar ao Wi-Fi
void connectWiFi() {
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao Wi-Fi!");
  Serial.println(clientId);
}

// Função de callback para receber mensagens MQTT
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  // Converte o payload para String
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Controla o LED conforme o comando
  if (message == "on") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ligado");
  } else if (message == "off") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED desligado");
  }
}

// Função para reconectar ao broker MQTT
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      mqttClient.subscribe(MQTT_TOPIC); // Inscreve-se no tópico
    } else {
      Serial.print("Falha. Código de erro: ");
      Serial.print(mqttClient.state());
      Serial.println(" Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Inicia com LED desligado

  connectWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback); // Define a função de callback

  // Ip wifi
  Serial.println("Conectado ao Wi-Fi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); // Adicione esta linha
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop(); // Mantém a conexão MQTT ativa
}
