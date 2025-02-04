#include <SPI.h>
#include <LoRa.h>

const int Output_Pin = 1; // Pin for the output (if needed for other purposes)

volatile int flow_frequency = 0; // Variable to hold pulse count

int l_hour = 0;  // Calculated litres/hour
unsigned char flowsensor = 2; // Sensor Input pin
unsigned long currentTime;
unsigned long cloopTime;

void flow() { // Interrupt function
   flow_frequency++; // Increment flow frequency
}

void setup() {
   Serial.begin(9600);  // Initialize serial communication
   while (!Serial);  // Wait for the serial port to be ready
   
   pinMode(flowsensor, INPUT);
   digitalWrite(flowsensor, HIGH); // Enable internal pull-up resistor for the flow sensor pin
   
   attachInterrupt(digitalPinToInterrupt(flowsensor), flow, RISING); // Setup Interrupt (pin 2)
   
   currentTime = millis();
   cloopTime = currentTime;

   Serial.println("LoRa Sender");

   if (!LoRa.begin(868E6)) { // Initialize LoRa at 868 MHz
      Serial.println("Starting LoRa failed!");
      while (1);  // Stay in an infinite loop if LoRa initialization fails
   }
}

void loop() {
   currentTime = millis();

   // Every second, calculate and print litres/hour
   if (currentTime >= (cloopTime + 1000)) {
      cloopTime = currentTime; // Update cloopTime
      // Calculate flow rate in litres per hour (L/hour)
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flow rate in L/hour
      flow_frequency = 0; // Reset the flow counter
      Serial.print(l_hour, DEC); // Print litres per hour
      Serial.println(" L/hour");
   }

   // Send the flow rate packet over LoRa
   LoRa.beginPacket();
   LoRa.print(l_hour);
   LoRa.endPacket();

   delay(5000); // Wait for 5 seconds before sending the next packet
}
