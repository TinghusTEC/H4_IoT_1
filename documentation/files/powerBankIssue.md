An ESP32 repeatedly waking from deep sleep when connected to a power bank is often due to the power bank's low-current shutoff feature. Power banks are designed to turn off when they detect a minimal load, which can happen when the ESP32 is in deep sleep. To prevent this, you can either ensure the ESP32 is drawing enough current to keep the power bank active or modify the power bank's behavior. 
Here's a breakdown of common solutions:
1. Increase Minimum Current Draw:
Add a Load Resistor:
A simple solution is to add a resistor in parallel with the ESP32's power rail (the 5V or 3.3V output from the power bank) to create a constant load. The resistor's value should be chosen to draw enough current to keep the power bank on but also be high enough to minimize power consumption. 
LED or Other Active Component:
If your project uses LEDs or other active components, ensure they are on or set to a low-power, dim state to maintain a minimum current draw. 
2. Modify Power Bank Behavior:
"Always-On" or "Trickle" Mode:
Some power banks have a feature to stay on with a low-current load. This is usually activated by pressing and holding the power button for a few seconds.
Check the Power Bank's Manual:
Refer to the power bank's manual for information on its low-power or "always-on" modes and how to enable them. 
3. Alternative Power Source:
Connect Directly to Battery:
If possible, bypass the power bank's boost converter and connect the ESP32 directly to the battery inside the power bank (3.7V or 4.2V). This can avoid the low-current shutoff issue.
Use a Dedicated Battery Pack:
Consider using a dedicated, small battery pack with a charger and voltage regulator to power the ESP32. 
4. ESP32 Deep Sleep Considerations:
Deep Sleep Duration:
If the ESP32 is waking up too frequently, ensure the deep sleep duration is appropriately set. You can use the esp_deep_sleep_start() function with a timer to control how long the ESP32 stays in deep sleep. 
Wake-up Sources:
Check that the wake-up sources for deep sleep are configured correctly and are not causing the ESP32 to wake up prematurely. 
Software Errors:
Examine your code for any errors that might be causing the ESP32 to wake up unexpectedly. 
In summary, the ESP32 waking from sleep on a power bank is a common issue. By understanding the power bank's low-current shutoff and using the methods above, you can keep the ESP32 powered and in deep sleep for longer periods. 