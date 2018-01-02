#include "Battery.h"

Battery::Battery() {
}

bool Battery::power_connected( void ) {
	//return true; 														// debugging
	return (battery_voltage() > MIN_CHARGING_VOLTAGE);
}

float Battery::battery_voltage( void ) {
	int sensorValue = analogRead(A0);
	float v = ((float)sensorValue * (3.3 / 1024.00) * 2) + DIODE_DROP;
	I2CSerial.println("battery_voltage = " + String(v));
	I2CSerial.println("A0 sensor value = " + String(sensorValue));
	return v;
	//return ((((float)sensorValue/1023.0)*3.3) * 2.0);
	//return (((float)sensorValue / 1024.0) * 7.8) + DIODE_DROP;
	//return ((float)sensorValue / 130);
	//return DIODE_DROP + BATT_MARGIN + (((float)sensorValue/1024.0) * 3.3 * 2.0);	
}

bool Battery::recharge_level_reached( void ) {
	return (battery_voltage() < MIN_BATT_VOLTAGE);
}

