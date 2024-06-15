# Secure Device
Este proyecto es un rastreador inalámbrico que utiliza GPS y LoRaWAN para transmitir la ubicación del dispositivo. El dispositivo utiliza un MPU6050 para recoger datos de movimiento y un módulo GPS para obtener la ubicación actual.

## Componentes
### MPU6050
Este es un sensor de movimiento que incluye un giroscopio y un acelerómetro. Para más detalles sobre cómo se utiliza en este proyecto, puedes consultar la documentación de la biblioteca aquí.

### GP
Utilizamos un módulo GPS para obtener la ubicación actual del dispositivo. Los datos del GPS se envían a través de LoRaWAN como parte del mensaje de enlace ascendente.

### LoRaWAN
Utilizamos LoRaWAN para transmitir los datos del dispositivo a una red. Los códigos de estado de la transmisión se pueden encontrar aquí.

## Uso
Para utilizar este dispositivo, simplemente enciéndelo y permitirá que se active automáticamente a través de la personalización (ABP). Una vez activado, comenzará a transmitir datos de ubicación y movimiento a través de LoRaWAN.

## Contribuciones
Las contribuciones a este proyecto son bienvenidas. Por favor, abre un problema para discutir lo que te gustaría cambiar o enviar una solicitud de extracción.