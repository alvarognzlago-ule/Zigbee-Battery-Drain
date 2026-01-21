**Drenaje de Batería en Dispositivos Zigbee — Replicación del Experimento**

Descripción
-----------
Este repositorio contiene los materiales y las instrucciones para reproducir un experimento de agotamiento de batería (battery exhaustion) en nodos Zigbee. El experimento demuestra cómo un atacante puede forzar a un módulo de radio y al microcontrolador a permanecer activos continuamente, reduciendo drásticamente la autonomía de una placa alimentada por batería.

Estado: documento de réplica experimental y scripts de prueba.

Objetivo
--------
- Mostrar que es posible inducir un consumo elevado de energía en nodos Zigbee enviando tramas dirigidas con suficiente frecuencia.
- Proveer instrucciones reproducibles para validar los resultados y comparar perfiles de tráfico (control, estrés, saturación).

Materiales y hardware
---------------------
- Placas M5Stack.
- Módulos XBee Series 2 (o equivalente Zigbee) para nodos víctima y un coordinador.
- Cable USB para el coordinador (conexión al PC).
- Batería de prueba (mismos parámetros para todas las réplicas).

Archivos clave
-------------
- [script_msg.py](script_msg.py): script en Python usado para enviar tramas API al coordinador y generar perfiles de tráfico.
- m5zigbeebattery/m5zigbeebattery.ino: sketch Arduino/ESP para leer el voltaje de batería y registrar datos.

Configuración y metodología
---------------------------
1. Configurar los XBee de las víctimas con los parámetros adecuados (p. ej. AP=0 para modo transparent) y el coordinador en modo API (AP=1).
2. Conectar el coordinador al PC y verificar el puerto serie disponible (COMx en Windows).
3. Configurar el puerto COM y las direcciones MAC para ejecutar el script de ataque desde el PC para generar los perfiles de tráfico:

	- Control: 1 paquete cada 10 minutos.
	- Estrés: 1 paquete/segundo.
	- Saturación: 5 paquetes/segundo.

4. Subir el código .ino a nuestros M5Stack Core2.
5. Monitorear y registrar el voltaje de la batería en las placas víctimas cada 5 minutos hasta fallo.

Ejecutar el script (ejemplo en Windows PowerShell)
-----------------------------------------------
Reemplace `COM3` por el puerto correspondiente y ajuste la velocidad si es necesario.

```powershell
python script_msg.py --port COM3 --baud 115200 --target 0013A20040B2XXXX --rate 1
```

Notas sobre `script_msg.py`
---------------------------
- El script utiliza `pyserial` para abrir el puerto serie y enviar tramas API (Transmit Request 0x10) al coordinador.
- Asegúrese de que el puerto no esté en uso por otro software (por ejemplo XCTU).

Sketch de la placa (m5zigbeebattery.ino)
---------------------------------------
- El sketch lee el voltaje de batería mediante el ADC y puede registrar o transmitir valores por serie para su posterior análisis.

Resultados esperados y observaciones
-----------------------------------
- Los datos experimentales muestran una reducción significativa de la autonomía en escenarios de ataque frente al control.
- Un hallazgo importante es que, por encima de cierto umbral de tráfico, el consumo de la radio se mantiene en su nivel máximo posible. Por tanto, frecuencias de ataque moderadas (p. ej. 1 paquete/segundo) pueden acercarse al máximo efecto dañino sin necesidad de saturar más la red.

---
Fecha de creación: 2026-01-20