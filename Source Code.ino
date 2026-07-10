#define ECG_PIN 34 
#define BUZZER 25 
#include <HardwareSerial.h> 
HardwareSerial sim800(1); 
// Threshold values 
int thresholdHigh = 2800; 
int thresholdLow = 1500; 
int spikeCount = 0; 
int flatCount = 0; 
// Moving average filter 
const int sampleSize = 10; 
int buffer[sampleSize]; 
int index1 = 0; 
long sum = 0; 
// SMS Function 
void sendSMS(String message){ 
sim800.println("AT+CMGF=1"); 
delay(1000); 
68 
sim800.println("AT+CMGS=\"+91XXXXXXXXXX\""); // Replace with your 
phone number 
delay(1000); 
sim800.print(message); 
delay(1000); 
sim800.write(26); 
delay(5000); 
} 
void setup() { 
Serial.begin(115200); 
pinMode(BUZZER, OUTPUT); 
digitalWrite(BUZZER, HIGH); 
// Start SIM800L 
sim800.begin(9600, SERIAL_8N1, 16, 17); 
Serial.println("NeuroPatch System Started"); 
} 
void loop() { 
// Moving Average Filter 
sum -= buffer[index1]; 
buffer[index1] = analogRead(ECG_PIN); 
69 
sum += buffer[index1]; 
index1 = (index1 + 1) % sampleSize; 
int filtered = sum / sampleSize; 
String condition = "Normal"; 
// Spike detection 
if(filtered > thresholdHigh){ 
spikeCount++; 
} else { 
spikeCount = 0; 
} 
// Flat signal detection 
if(filtered < thresholdLow){ 
flatCount++; 
} else { 
flatCount = 0; 
} 
// Seizure detection 
if(spikeCount > 10){ 
condition = "Seizure-like"; 
digitalWrite(BUZZER, LOW); 
Serial.println("Seizure detected!"); 
70 
sendSMS("NeuroPatch ALERT: Seizure detected"); 
delay(10000); 
} 
// Coma detection 
else if(flatCount > 20){ 
condition = "Coma-like"; 
digitalWrite(BUZZER, LOW); 
Serial.println("Coma detected!"); 
sendSMS("NeuroPatch ALERT: Coma signal detected"); 
delay(10000); 
} 
else{ 
digitalWrite(BUZZER, HIGH); 
} 
// Serial Monitor Output 
Serial.print("Signal: "); 
Serial.print(filtered); 
Serial.print(" | Condition: "); 
Serial.println(condition); 
delay(50); 
}