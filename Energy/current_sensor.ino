void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  int avg_loop=600;
  uint32_t sensor_value=0;
  int adc=analogRead(A0);
  for(int i=0;i<avg_loop;i++){
    sensor_value=sensor_value+adc;
  }
  sensor_value=sensor_value/avg_loop;
  float voltage=sensor_value*(5.0/1.023);//voltage in mV
  float current = (voltage-2500.0)*5.0;//current in mA
  Serial.println(adc);
  Serial.println(voltage);
  Serial.println(sensor_value);
  Serial.println("Current [mA]:" + String(current));
  delay(1500);

  //}
  

}
