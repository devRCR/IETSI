import serial, time, json, requests, rel, websocket

# Configura el puerto serial
baseStation = serial.Serial('COM17', 115200)  # Cambia 'COM1' al nombre de tu puerto serial y 9600 a la velocidad correcta

url = 'https://appsqa.essalud.gob.pe/gestion-atencion-service/api/ticket/atender-cola'
headers = {'Content-type': 'application/json'}

try:
    while True:
        # Lee una línea desde el puerto serial
        data = baseStation.readline()
        
        # Decodifica los bytes a una cadena (ajusta el formato según lo que envía tu dispositivo)
        decoded_data = data.decode('utf-8').strip()
        
        # Imprime y realiza funciones de acuerto al mando recibido        
        if(decoded_data=='*31A'):
            print("Ventanilla 1")
            # acá se debe agregar las otras rutinas para que haga el post
            data = '{"ventanilla":"2","guiidOperador":"iU177A02EP-eb062dc8-0b44-4b21-8a57-b9a0df714af7-6L230u15LN-vn51AZ017K","codOficina":"11"}'
            response = requests.post(url, headers=headers, data=data)
            print(response.status_code)
            print(response.text)
            # líneas abajo agregar rutina para el websocket y STOMP Protocol
            
        elif (decoded_data=='*21A'):
            print("Ventanilla 2")
            # acá se debe agregar las otras rutinas para que haga el post
            data = '{"ventanilla":"2","guiidOperador":"iU177A02EP-eb062dc8-0b44-4b21-8a57-b9a0df714af7-6L230u15LN-vn51AZ017K","codOficina":"11"}'
            response = requests.post(url, headers=headers, data=data)
            print(response.status_code)
            print(response.text)
            # líneas abajo agregar rutina para el websocket y STOMP Protocol
        else:
            None

except KeyboardInterrupt:
    # Maneja una interrupción de teclado (Ctrl+C) para salir del bucle
    pass

finally:
    # Cierra el puerto serial al finalizar
    ser.close()
