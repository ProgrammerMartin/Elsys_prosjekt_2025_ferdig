#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "wifi_config.h"
#include <WiFi.h>
#include "gyAc.h"
#include "piezo.h"
#include <Arduino.h>
#include <arduinoFFT.h>
#include "esp32-hal-timer.h"
#include "fftStruct.h"

portMUX_TYPE stopNowMux = portMUX_INITIALIZER_UNLOCKED;

// Oppretter en handle for hver task for å kunne stoppe og starte de
TaskHandle_t mpu6050TaskHandle = NULL;
TaskHandle_t piezoSamplingTaskHandle = NULL;
TaskHandle_t fftTaskHandle = NULL;
TaskHandle_t stopNowTaskHandle = NULL;
TaskHandle_t mainWifiTaskHandle = NULL;
TaskHandle_t waitForStartTaskHandle = NULL;
TaskHandle_t resetEspTaskHandle = NULL;

hw_timer_t *timer;

// Objekter
Adafruit_MPU6050 mpu;  // Definerer et objekt av Adafruit_MPU6050-klassen

const char* host = "192.168.51.219";  // IP-adressen til serveren på PC-en

WiFiClient client1, client2, fftClient, piezoClient, restartClient;

// Porter
const uint16_t generalSendPort = 5000;
const uint16_t espRestartedPort = 6000;
const uint16_t fftSendPort = 7000;
const uint16_t sendPiezoPort = 8000;
const uint16_t mainPort = 6500;
const uint16_t receiveStartMessagePort = 9000;

// Pins
const uint8_t piezoPin = 35;
const uint8_t micPin = 32;

// Mic-konstanter
const uint16_t bufferSize = 256;
const float samplingPeriod = 256e-6;
const float samplingRate = 1.0 / samplingPeriod;
const uint8_t BUFFER_SIZE = 100;

// Andre konstanter
const unsigned long timeout = 30000;
const unsigned long wifiSendTimeout = 10000;

// Oppretter et piezo-objekt
PiezoSensor piezo1(piezoPin);

// FFT-arrays for midlertidig lagring av FFT-data
float vReal[bufferSize];
float vImag[bufferSize];

// Oppretter et Arduino-FFT-objekt
ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, bufferSize, samplingRate);

// Vektorer for å lagre dataene
std::vector<mpuDataType> mpuLagringsVektor;
std::vector<piezoDataType> piezoLagringsVektor;
std::vector<frekvensPlottStruct> frekvensLagringsVektor;

// Variabler
int i = 0;
bool micDataReadyForFFT = false;
const long interval = 3000;
unsigned long startMillis;
volatile bool stopNow = false;
volatile bool sendOverWifiNow = false;
volatile bool startSampling = false;
uint32_t lastDotMillis = 0; // Ny variabel for tidtakning
unsigned long startAttemptTime;
volatile bool sampleMpu = false;
volatile bool samplePiezo = false;
volatile bool sampleMic = false;
volatile bool runFftTask = false;
unsigned long timeoutWifiSending = 5000;
unsigned long startTimeMpuWifi;
unsigned long startTimeFftWifi;
unsigned long startTimePiezoWifi;
unsigned long startTimeTimeout;


void mpuWifiSend(){
  if (client2.connected()) {
    Serial.println("client.connected() = true, skriver til klienten.");
    Serial.print("Antall mpu-struct-objekter som sendes (lengden på mpuLagringsVektor) er: ");
    Serial.println(mpuLagringsVektor.size());
    Serial.print("Struct size: ");
    Serial.println(sizeof(mpuDataType));
    Serial.println("Sender mpu-data..");
    startTimeMpuWifi = millis();
    for (const auto& data : mpuLagringsVektor) {
      client2.write(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
      if (millis() - startTimeMpuWifi > timeoutWifiSending) {
        Serial.println("Advarsel: Timeout under sending av mpu-data! Avbryter funksjonen.");
        return;
      }
    }
    Serial.println("Skriver END til porten, for å signalisere at MPU-sending er fullført");
    client2.println("END");
    Serial.println("Ferdig å skrive mpu-data til klienten.");
    Serial.println("Tømmer mpuLagringsVektor...");
    mpuLagringsVektor.clear();
    mpuLagringsVektor.shrink_to_fit();
  } 
  else {
    Serial.println(" ");
    Serial.println("FEIL: TCP-forbindelse for mpu mistet.");
    Serial.println(" ");
    // Kall på connect to client?
    mpuLagringsVektor.clear();
    mpuLagringsVektor.shrink_to_fit();
  }
}

void fftWifiSend(){
  if (fftClient.connected()) {
    Serial.println("fftClient.connected() = true, skriver til klienten.");
    Serial.print("Antall FFT-struct-objekter som sendes (lengden på frekvensLagringsVektor) er: ");
    Serial.println(frekvensLagringsVektor.size());
    Serial.print("Struct size: ");
    Serial.println(sizeof(frekvensPlottStruct));
    Serial.println("Sender FFT-data..");
    startTimeFftWifi = millis();
    for (const auto& data : frekvensLagringsVektor) {
      fftClient.write(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
      if (millis() - startTimeFftWifi > timeoutWifiSending) {
        Serial.println("Advarsel: Timeout under sending av mpu-data! Avbryter funksjonen.");
        return;
      }
    }
    Serial.println("Skriver END til porten, for å signalisere at FFT-sending er fullført");
    fftClient.println("END");
    Serial.println("Ferdig å skrive FFT-data til klienten.");


    Serial.println("Tømmer frekvensLagringsVektor...");
    frekvensLagringsVektor.clear();
    frekvensLagringsVektor.shrink_to_fit();
    Serial.print("Kapasitet etter shrink_to_fit(): ");
    Serial.println(frekvensLagringsVektor.capacity());
  } 
  else {
    Serial.println(" ");
    Serial.println("FEIL: TCP-forbindelse for FFT mistet.");
    Serial.println(" ");
    frekvensLagringsVektor.clear();
    Serial.println(frekvensLagringsVektor.capacity());
  }
}

void piezoWifiSend(){
  if (piezoClient.connected()) {
    Serial.println("piezoClient.connected() = true, skriver til klienten.");
    Serial.print("Antall Piezo-struct-objekter som sendes (lengden på piezoLagringsVektor) er: ");
    Serial.println(piezoLagringsVektor.size());
    Serial.print("Struct size: ");
    Serial.println(sizeof(piezoDataType));
    Serial.println("Sender piezo-data..");
    startTimePiezoWifi = millis();
    for (const auto& data : piezoLagringsVektor) {
      piezoClient.write(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
      if (millis() - startTimePiezoWifi > timeoutWifiSending) {
        Serial.println("Advarsel: Timeout under sending av mpu-data! Avbryter funksjonen.");
        return;
      }
    }
    Serial.println("Skriver END til porten, for å signalisere at piezo-sending er fullført");
    piezoClient.println("END");
    Serial.println("Ferdig å skrive piezo-data til klienten.");
    Serial.println("Tømmer piezoLagringsVektor...");
    piezoLagringsVektor.clear();
    piezoLagringsVektor.shrink_to_fit();
  } 
  else {
    Serial.println(" ");
    Serial.println("FEIL: TCP-forbindelse for piezo mistet.");
    Serial.println(" ");
    piezoLagringsVektor.clear();
    piezoLagringsVektor.shrink_to_fit();
  }
}

bool connectClient(WiFiClient& client, const uint16_t port){
  Serial.printf("Prøver å koble til med et WiFiClient-objekt på porten %u.", port);
  startAttemptTime = millis();
  while (!client.connect(host, port)) {
    Serial.print(".");
    if (millis() - startAttemptTime > timeout) {
      Serial.println("Kunne ikke koble til TCP-server, starter ESP-en på nytt...");
      delay(200);
      ESP.restart();
    }
    delay(200); // Vent litt mellom forsøkene
  }
  Serial.println(".");
  Serial.printf("Koblet til på porten %u\n", port);
  return true;
}

void IRAM_ATTR sampleOnTimer() {
  if (startSampling){
    digitalWrite(2, HIGH);
    if (millis() - startMillis >= interval){
      stopNow = true;
      timerStop(timer);
      digitalWrite(2, LOW);
    }
    else{
      if (sampleMic){
        int micValue = analogRead(micPin);
        float rawVoltage = (micValue / 4095.0) * 3.3;  
        float signalVoltage = rawVoltage - 1.572;  
        
        vReal[i] = signalVoltage;
        i++;
        if (i >= bufferSize) {
          i = 0;
          micDataReadyForFFT = true;
          timerStop(timer); // Deaktiver timeren midlertidig
        }
      }
    }
  }
}

void performFFT(){
    // Serial.println("performFFT() kjører");
    memset(vImag, 0, bufferSize * sizeof(vImag[0]));

    FFT.compute(FFT_FORWARD);
    FFT.complexToMagnitude();
}

void storeFftDataInVector(){
  frekvensPlottStruct dataInStruct{static_cast<uint16_t>(millis() - startMillis)};
  for (int j = 1; j <= (bufferSize / 2) + 2; j++) {
    // udpFft.print(j * (samplingRate / bufferSize));  // Frekvens
    dataInStruct.amplitudes[j-1] = abs(100 * vReal[j]);
  }
  if (frekvensLagringsVektor.size() < 50){
    frekvensLagringsVektor.push_back(dataInStruct);
    // Serial.println("FFT-PAKKE SKAL VÆRE LAGT I LAGRINGSVEKTOREN!");
  }
  else{
    Serial.println("FFT-LAGRINGSVEKTOR FULL");
  }
  // Serial.print("frekvensLagringsVektor.size() = ");
  // Serial.println(frekvensLagringsVektor.size());
}

void initializeMpuCommunication(){
  // Prøver å starte MPU6050-sensoren ved å initialisere kommunikasjonen over I2C
  while (!mpu.begin()) {
    Serial.println(" ");
    Serial.println("FEIL: MPU6050 init error! Retrying...");
    Serial.println(" ");
    delay(1000);  // Vent i 1 sekund før vi prøver igjen
  }
  Serial.println("MPU6050 OK!");  // Går videre hvis MPU6050-sensoren er initialisert riktig
}

void createTask(TaskFunction_t taskFunction, const char* taskName, uint16_t stackSize, UBaseType_t priority, TaskHandle_t* taskHandle) {
  if (xTaskCreate(taskFunction, taskName, stackSize, NULL, priority, taskHandle) == pdPASS) {
      Serial.print(taskName);
      Serial.println(" opprettet");
  } else {
      Serial.println(" ");
      Serial.println("FEIL: ");
      Serial.println(" ");
      Serial.print(taskName);
      Serial.println(" kunne ikke opprettes");
  }
}

void piezoSamplingTask(void *pvParmeters){
  vTaskSuspend(NULL);
  vTaskDelay(30 / portTICK_PERIOD_MS);  // Gir setup() tid til å fullføre, når tasken er resumert
  Serial.println("piezoSamplingTask begynner å kjøre nå");
  samplePiezo = true;
  while(true){
    while (samplePiezo){
      piezoDataType dataInStruct = piezo1.readPiezo();
      dataInStruct.timestamp = static_cast<uint16_t>(millis() - startMillis);
      if (piezoLagringsVektor.size() < 5000){
        piezoLagringsVektor.push_back(dataInStruct);
      }
      // Serial.print("piezoLagringsVektor.size() = ");
      // Serial.println(piezoLagringsVektor.size());
      vTaskDelay(1 / portTICK_PERIOD_MS);
    }
  }
}

void fftTask(void *pvParameters){
  vTaskSuspend(NULL);
  vTaskDelay(30 / portTICK_PERIOD_MS);  // Gir setup() tid til å fullføre, når tasken er resumert
  Serial.println("fftTask begynner å kjøre nå");

  while (true){
    if (micDataReadyForFFT && runFftTask){
      // Serial.println("micDataReadyForFFT!");
      performFFT();
      storeFftDataInVector();
      micDataReadyForFFT = false;
      timerStart(timer);  // Gjenoppta timeren for å samle mer data
      // Serial.println("timer startet igjen!");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void mpu6050Task(void *pvParameters){
  vTaskSuspend(NULL);
  vTaskDelay(30 / portTICK_PERIOD_MS);  // Gir setup() tid til å fullføre, når tasken er resumert
  Serial.println("mpu6050Task begynner å kjøre nå");
  sampleMpu = true;
  while(true){
    while (sampleMpu){
      // Serial.println("Prøver å hente data fra GyAc().");
      // const char* data = GyAc(); // Hent data fra GyAc()
      mpuDataType dataInStruct = GyAc();
      dataInStruct.timestamp = static_cast<uint16_t>(millis() - startMillis);
      if (mpuLagringsVektor.size() < 500){
        mpuLagringsVektor.push_back(dataInStruct);
      }
      /*
      Serial.print("mpuLagringsVektor.size() = ");
      Serial.println(mpuLagringsVektor.size());
      Serial.print("LEDIG HEAP-MINNE: ");
      Serial.print(esp_get_free_heap_size());
      Serial.println(" BYTES");
      */
      vTaskDelay(10 / portTICK_PERIOD_MS); // 20 ms delay
    }
  }
}

void resetEspTask(void *pvParameters){
  vTaskSuspend(NULL);
  vTaskDelay(30 / portTICK_PERIOD_MS);  // Gir setup() tid til å fullføre, når tasken er resumert
  Serial.println("resetEspTask begynner å kjøre nå");
  Serial.printf("Lytter etter kommando på å restarte, på TCP-port %d...\n", espRestartedPort);
  while (true){
    char packetBuffer[50] = {0};
    if (restartClient.connected() && restartClient.available()){
      Serial.println("Prøver å lese fra restartClient sin port");
      int bytesRead = restartClient.readBytesUntil('\n', packetBuffer, sizeof(packetBuffer) - 1);
      packetBuffer[bytesRead] = '\0'; // Null-terminer
      Serial.printf("Mottok: %s\n", packetBuffer);
      if (strcmp(packetBuffer, "RESTART") == 0){
        Serial.println("Fått beskjeden RESTART");
        Serial.println("Restarter ESP-en...");
        delay(500);
        ESP.restart();
      }
    }
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

void stopNowTask(void *pvParameters) {
  vTaskSuspend(NULL);
  vTaskDelay(30 / portTICK_PERIOD_MS);
  Serial.println("stopNowTask begynner å kjøre nå");
  stopNow = false;
  while (true){
    if (stopNow){
      portENTER_CRITICAL(&stopNowMux);
      stopNow = false;
      startSampling = false;
      sampleMpu = false;
      samplePiezo = false;
      sampleMic = false;
      runFftTask = false;
      portEXIT_CRITICAL(&stopNowMux);
      vTaskDelay(50 / portTICK_PERIOD_MS);
      Serial.println("Recording ferdig.");
      Serial.println("Suspenderer (nesten) alle tasks...");
      vTaskSuspend(piezoSamplingTaskHandle);
      Serial.println("Task1 suspended");
      vTaskSuspend(mpu6050TaskHandle);
      Serial.println("Task2 suspended");
      vTaskSuspend(fftTaskHandle);
      Serial.println("Alle (nesten) tasks suspended");
      Serial.println("Resumerer mainWifiTask");
      sendOverWifiNow = true;
      vTaskResume(mainWifiTaskHandle);
      Serial.print("Suspender stopNowTask ...");
      vTaskSuspend(NULL);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void mainWifiTask(void *pvParameters){
  vTaskSuspend(NULL);
  vTaskDelay(30 / portTICK_PERIOD_MS);
  Serial.println("mainWifiTask begynner å kjøre nå");
  while (true){
    if (sendOverWifiNow){
      Serial.print("Ledig heap-minne før wifi-sendingene: ");
      Serial.print(esp_get_free_heap_size());
      Serial.println(" bytes");
      Serial.println("mainWifiTask har fått beskjed om å begynne wifi-sending");
      vTaskDelay(200 / portTICK_PERIOD_MS);
      Serial.println("Kaller på mpuWifiSend() ..");
      mpuWifiSend();
      Serial.println("mpuWifiSend() er ferdig.");
      Serial.println("Kaller på fftWifiSend() ..");
      fftWifiSend();
      Serial.println("fftWifiSend() er ferdig.");
      Serial.println("Kaller på piezoWifiSend() ..");
      piezoWifiSend();
      Serial.println("piezoWifiSend() er ferdig");
      delay(100);
      Serial.println(" ");
      Serial.println("ALL UTSKRIFT FERDIG. Setter ESP-en i ventemodus, ved å resume waitForStartTask");
      Serial.println(" ");
      delay(500);
      sendOverWifiNow = false;
      Serial.println("Resumerer waitForStartTask...");
      delay(100);
      vTaskResume(waitForStartTaskHandle);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void waitForStartTask(void *pvParameters){
  vTaskDelay(150 / portTICK_PERIOD_MS);
  Serial.println("waitForStartTask begynner å kjøre.");
  Serial.printf("Lytter etter kommandoer på TCP-port %d...\n", receiveStartMessagePort);
  bool waitingMessagePrinted = false;
  while (true){
    char packetBuffer[50] = {0};
    if (!waitingMessagePrinted) {
      Serial.print("Ledig heap-minne før ny start: ");
      Serial.print(esp_get_free_heap_size());
      Serial.println(" bytes");
      Serial.print("Venter på beskjed om å starte...");
      startTimeTimeout = millis();
      waitingMessagePrinted = true; // Sett indikatoren til true etter første utskrift
      lastDotMillis = millis();
    }
    if (millis() - lastDotMillis >= 1000) { // 1000 ms = 1 sekund
      Serial.print(".");
      lastDotMillis = millis(); // Nullstill etter hvert punktum
      if (millis() - startTimeTimeout > (120000)){
        Serial.println("Fikk ikke beskjed etter to minutt om å starte. Restarter fisken...");
        ESP.restart();
      }
    }
    if (client1.connected() && client1.available()) {
      Serial.println("Prøver å lese..");
      int bytesRead = client1.readBytesUntil('\n', packetBuffer, sizeof(packetBuffer) - 1);
      packetBuffer[bytesRead] = '\0'; // Null-terminer
      Serial.printf("Mottok: %s\n", packetBuffer);

      if (strcmp(packetBuffer, "START") == 0){
        waitingMessagePrinted = false;
        Serial.println("Fått beskjeden START");

        Serial.println("Forsikrer at lagringsvektorene er tomme...");
        mpuLagringsVektor.shrink_to_fit();
        if (mpuLagringsVektor.size() > 0){
          mpuLagringsVektor.clear();
          mpuLagringsVektor.shrink_to_fit();
        }
        piezoLagringsVektor.shrink_to_fit();
        if (piezoLagringsVektor.size() > 0){
          piezoLagringsVektor.clear();
          piezoLagringsVektor.shrink_to_fit();
        }
        frekvensLagringsVektor.shrink_to_fit();
        if (frekvensLagringsVektor.size() > 0){
          frekvensLagringsVektor.clear();
          frekvensLagringsVektor.shrink_to_fit();
        }
        Serial.println("Lagringsvektorene skal være tomme.");
        Serial.println("Setter startMillis.");
        startMillis = millis();
        Serial.print("startMillis = ");
        Serial.println(startMillis);

        sampleMpu = true;
        samplePiezo = true;
        sampleMic = true;
        runFftTask = true;
        Serial.println("Resumerer tasks");
        vTaskResume(mpu6050TaskHandle);
        vTaskResume(piezoSamplingTaskHandle);
        vTaskResume(fftTaskHandle);
        vTaskResume(stopNowTaskHandle);
        vTaskResume(resetEspTaskHandle);
        Serial.println("Alle tasks skal være resumert");
        startSampling = true;
        Serial.println("Suspenderer waitForStartTask.");
        vTaskSuspend(NULL);
      }
      else {
        Serial.println("Ugyldig kommando, venter på START...");
      }
    }
    vTaskDelay(30 / portTICK_PERIOD_MS);
  }
}

void setup() {
  stopNow = false;  // Nullstill eksplisitt etter opplastning eller reset
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);

  Serial.begin(115200);  // Starter seriell kommunikasjon med baudrate på 115200

  analogReadResolution(12);  // Oppløsningen til ADC-en

  Wire.begin(25, 26);  // Initialiserer I2C-kommunikasjonen på ESP-en, 
                       // og angir de spesifikke pinnene som skal brukes for SDA (Serial Data) og SCL (Serial Clock).
                       // Nødvendig for å hente data fra MPU6050-sensoren

  initializeMpuCommunication();

  setupWifi();  // Kobler til WiFi

  connectClient(client1, receiveStartMessagePort);
  connectClient(client2, generalSendPort);
  connectClient(fftClient, fftSendPort);
  connectClient(piezoClient, sendPiezoPort);
  connectClient(restartClient, espRestartedPort);
  Serial.println("Ferdig å koble til portene");

  hw_timer_t *timer = timerBegin(4000); // Frekvens på 4 kHz
  if (timer == NULL) {
      Serial.println("Timer creation failed!");
      return;
  }
  timerAttachInterrupt(timer, sampleOnTimer);

  Serial.println("Oppretter tasks.");
  createTask(waitForStartTask, "waitForStartTask", 2048, (UBaseType_t)20, &waitForStartTaskHandle);
  createTask(resetEspTask, "resetEspTask", 2048, (UBaseType_t)20, &resetEspTaskHandle);
  createTask(mpu6050Task, "mpu6050Task", 4096, (UBaseType_t)6, &mpu6050TaskHandle);
  createTask(piezoSamplingTask, "piezoSamplingTask", 2048, (UBaseType_t)5, &piezoSamplingTaskHandle);
  createTask(fftTask, "fftTask", 8192, (UBaseType_t)10, &fftTaskHandle);
  createTask(stopNowTask, "stopNowTask", 2048, (UBaseType_t)19, &stopNowTaskHandle);
  createTask(mainWifiTask, "mainWifiTask", 8192, (UBaseType_t)19, &mainWifiTaskHandle);
  Serial.println("Alle tasks opprettet.");

  Serial.println("Starter hardware-timer i setup()");
  timerStart(timer);
  timerAlarm(timer, 1, true, 0);
  Serial.println("Startet timer");
  // delay(200);

  vTaskResume(resetEspTaskHandle);

  // sendEspRestartedMessage();

  Serial.print("Ledig heap-minne når setup() har kjørt: ");
  Serial.print(esp_get_free_heap_size());
  Serial.println(" bytes");
}

void loop() {

}
