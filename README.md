# DMiX

## USB and BLE interface

```mermaid
classDiagram
bleJDY --|> appBluetooth
uart --|> appBluetooth
appProtocol --|> appBluetooth
appBluetooth --|> app

appProtocol --|> app
appProtocol --|> appUSB
appUSB --|> app
usb --|> appUSB



```

