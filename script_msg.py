import serial
import time
import struct

# --- CONFIGURACIÓN ---
PUERTO_COM = 'COMX'  # Cambia 'COMX' por el puerto correcto (ejemplo: 'COM3' en Windows o '/dev/ttyUSB0' en Linux)
BAUD_RATE = 9600

# --- DIRECCIONES MAC (Tus víctimas) --- En XCTU, la parte alta de la dirección es el parámetro "SH" y la baja "SL".
# VÍCTIMA 1: Router Viejo (ESTRÉS) -> Recibirá caña
MAC_ESTRES = b'\xXX\xXX\xXX\xXX\xXX\xXX\xXX\xXX'

# VÍCTIMA 2: Ex-Coordinador (CONTROL) -> Recibirá poco
MAC_CONTROL = b'\xXX\xXX\xXX\xXX\xXX\xXX\xXX\xXX'


def construir_trama_tx(mac_destino, mensaje):
    frame_type = b'\x10' 
    frame_id   = b'\x01' # ACK pedido
    dest_16bit = b'\xFF\xFE'
    options    = b'\x00'
    radius     = b'\x00'
    data       = mensaje.encode('utf-8')
    
    frame_content = frame_type + frame_id + mac_destino + dest_16bit + radius + options + data
    length = len(frame_content).to_bytes(2, byteorder='big')
    checksum = 0xFF - (sum(frame_content) & 0xFF)
    checksum = checksum.to_bytes(1, byteorder='big')
    
    return b'\x7E' + length + frame_content + checksum

def main():
    try:
        print(f"--- MODO INTENSIVO (0.2s) ACTIVADO ---")
        ser = serial.Serial(PUERTO_COM, BAUD_RATE, timeout=1)
        print(f"Conectado a {PUERTO_COM}.")
        print("------------------------------------------------")
        print("  [ESTRÉS] ...41AB29A7 -> ¡5 mensajes / seg! (0.2s)")
        print("  [CONTROL] ...41D1AEF1 -> 1 mensaje / 10 min")
        print("------------------------------------------------")
        
        ultimo_rapido = 0
        ultimo_lento = -600 
        contador = 0
        
        while True:
            ahora = time.time()
            
            # --- ENVÍO RÁPIDO (Cada 0.2 seg) ---
            # ### OJO: AQUÍ ESTÁ EL CAMBIO ###
            if ahora - ultimo_rapido >= 0.2: 
                msg = f"ATAQUE_INTENSIVO_{contador}"
                trama = construir_trama_tx(MAC_ESTRES, msg)
                ser.write(trama)
                
                # Imprimimos menos veces por pantalla para no saturar tu vista
                if contador % 5 == 0: 
                    print(f"[{time.strftime('%H:%M:%S')}] >>> TIRO RÁPIDO x5 ({contador})")
                
                ultimo_rapido = ahora
                contador += 1
                
            # --- ENVÍO LENTO (Cada 600 seg = 10 min) ---
            if ahora - ultimo_lento >= 600.0:
                print(f"\n[{time.strftime('%H:%M:%S')}] *** TIRO DE CONTROL ***")
                trama = construir_trama_tx(MAC_CONTROL, "CONTROL_10_MIN")
                ser.write(trama)
                print("------------------------------------------------\n")
                ultimo_lento = ahora
            
            # Bajamos el sleep un poco para asegurar precisión en 0.2s
            time.sleep(0.005) 

    except serial.SerialException:
        print(f"\n[ERROR] Cierra XCTU, por favor.")
    except Exception as e:
        print(f"\nERROR: {e}")

if __name__ == "__main__":
    main()