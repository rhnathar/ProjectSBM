# ESP32 DHT22 Sensor Display on ILI9341
This project demonstrates how to read temperature and humidity data from a DHT22 sensor using an ESP32 microcontroller and display the readings on ILI9341 TFT display. UI are generated using Squareline Studio

![demo.jpeg](https://github.com/rhnathar/ProjectSBM/blob/main/demo.jpeg)
![graph.jpeg](https://github.com/rhnathar/ProjectSBM/blob/main/graph.jpeg)

## Components
- ESP32 Development Board
- DHT22 Temperature and Humidity Sensor
- ILI9341 TFT Display

### Here is how I connected the components:

| Component     | ESP32 Pin |
|---------------|------------|
| **DHT22**     |            |
| VCC           | 3.3V       |
| GND           | GND        |
| Data          | GPIO23     |
| **ILI9341**   |            |
| VCC           | 3.3V       |
| GND           | GND        |
| CS            | GPIO15     |
| RESET         | GPIO12     |
| DC            | GPIO2      |
| SDI (MOSI)    | GPIO13     |
| SCK           | GPIO14     |
| LED           | GPIO21     |
| SDO (MISO)    | GPIO16     |
| T_CLK         | GPIO25     |
| T_IRQ         | GPIO36     |
| T_DO          | GPIO39     |
| T_DIN         | GPIO32     |
| T_CS          | GPIO16     |
