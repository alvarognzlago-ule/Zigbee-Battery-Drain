import serial
import time
import struct

# --- CONFIGURATION ---
PUERTO_COM = 'COMX'  # Change 'COMX' to the correct port (e.g. 'COM3' on Windows or '/dev/ttyUSB0' on Linux)
BAUD_RATE = 9600

# --- MAC ADDRESSES (Victim nodes) --- In XCTU, the high part is 'SH' and the low part is 'SL'.
# VICTIM 1: Old Router (STRESS) -> will receive frequent packets
MAC_ESTRES = b'\xXX\xXX\xXX\xXX\xXX\xXX\xXX\xXX'

# VICTIM 2: Ex-Coordinator (CONTROL) -> will receive occasional packets
MAC_CONTROL = b'\xXX\xXX\xXX\xXX\xXX\xXX\xXX\xXX'


def construir_trama_tx(mac_destino, mensaje):
    frame_type = b'\x10'
    frame_id   = b'\x01'  # request ACK
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
        print(f"--- INTENSIVE MODE (0.2s) ENABLED ---")
        ser = serial.Serial(PUERTO_COM, BAUD_RATE, timeout=1)
        print(f"Connected to {PUERTO_COM}.")
        print("------------------------------------------------")
        print("  [STRESS] ...41AB29A7 -> 5 messages/sec (0.2s)")
        print("  [CONTROL] ...41D1AEF1 -> 1 message / 10 min")
        print("------------------------------------------------")

        ultimo_rapido = 0
        ultimo_lento = -600
        contador = 0

        while True:
            ahora = time.time()

            # --- FAST SEND (Every 0.2 s) ---
            # ### NOTE: CHANGE HERE ###
            if ahora - ultimo_rapido >= 0.2:
                msg = f"ATAQUE_INTENSIVO_{contador}"
                trama = construir_trama_tx(MAC_ESTRES, msg)
                ser.write(trama)

                # Print less frequently to avoid flooding the console
                if contador % 5 == 0:
                    print(f"[{time.strftime('%H:%M:%S')}] >>> FAST SHOT x5 ({contador})")

                ultimo_rapido = ahora
                contador += 1

            # --- SLOW SEND (Every 600 s = 10 min) ---
            if ahora - ultimo_lento >= 600.0:
                print(f"\n[{time.strftime('%H:%M:%S')}] *** CONTROL SHOT ***")
                trama = construir_trama_tx(MAC_CONTROL, "CONTROL_10_MIN")
                ser.write(trama)
                print("------------------------------------------------\n")
                ultimo_lento = ahora

            # Slight sleep to help achieve 0.2s resolution
            time.sleep(0.005)

    except serial.SerialException:
        print(f"\n[ERROR] Please close XCTU or any other serial terminal.")
    except Exception as e:
        print(f"\nERROR: {e}")


if __name__ == "__main__":
    main()