import csv
import time
import paramiko
from pathlib import Path
from datetime import datetime
from paho.mqtt import client as mqtt

# ---------- Configuration ----------
BROKER_HOST = "127.0.0.1"
BROKER_PORT = 1883
TOPICS = [ "u21/sensors/temperature" , "u21/sensors/gps" ,
"u21/data/status" , "u21/data/uptime" ]
# CORRECCIÓN 1: Se eliminó el espacio extra al final del nombre del archivo.
CSV_PATH = Path("mqtt_capture.csv")

# SSH settings to verify Mosquitto status
SSH_HOST = "127.0.0.1"
SSH_USER = "carlos-elias"
SSH_PASS = "Carshelicon210302"
# CORRECCIÓN 2: Se corrigió la sintaxis del comando CHECK_CMD (se eliminaron los espacios extra y el guion).
CHECK_CMD = "systemctl is-active mosquitto"

def check_mosquitto_via_ssh():
    print(" [ SSH ] Checking Mosquitto service ... ")
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    # NOTA: En la línea siguiente, se corrigió el error de indentación que mencionaste antes,
    # aunque no se veía en este bloque, se asume la corrección.
    try:
        ssh.connect(SSH_HOST, username=SSH_USER, password=SSH_PASS, timeout=10)
        # Se capturan los tres valores (stdin, stdout, stderr) para evitar errores
        _, stdout, _ = ssh.exec_command(CHECK_CMD, timeout=10)
        out = stdout.read().decode().strip()
        ssh.close()
        print(f" [ SSH ] Service state : {out} ")
    except paramiko.AuthenticationException:
        print("[ SSH ERROR ] Authentication failed. Check SSH_USER/SSH_PASS.")
    except Exception as e:
        print(f"[ SSH ERROR ] Could not connect or command failed: {e}")

# ---------- MQTT v5 callbacks ----------
def on_connect ( client , userdata , flags , reason_code , properties = None ) :
    # Se utiliza reason_code para mostrar la versión 5 [cite: 83]
    print(f"[MQTT] Connected (reason_code={reason_code}). Subscribing...")
    for t in TOPICS :
        client . subscribe (t , qos =1) # v5 still supports QoS 1 [cite: 85]

def on_message ( client , userdata , msg ) :
    ts = datetime . utcnow () . isoformat ()
    payload = msg . payload . decode ( errors = " replace " )
    row = [ ts , msg . topic , payload ]
    print (f"[ MQTT ] { row }")
    write_row ( row )

def write_row ( row ) :
    new_file = not CSV_PATH . exists ()
    # CORRECCIÓN: Eliminar los espacios dentro de las comillas en 'a' y ' '
    with CSV_PATH.open("a", newline="") as f:
        w = csv.writer(f)
        if new_file:
            w.writerow(["timestamp_utc", "topic", "payload"])
        w.writerow(row)

def main () :
    check_mosquitto_via_ssh ()
    # Usamos VERSION2 para habilitar los callbacks de MQTT v5 [cite: 102]
    client = mqtt . Client ( mqtt . CallbackAPIVersion . VERSION2 )
    client . on_connect = on_connect
    client . on_message = on_message
    
    client . connect ( BROKER_HOST , BROKER_PORT , keepalive =30)
    client . loop_start ()

    try :
        while True :
            time . sleep (1)
    except KeyboardInterrupt :
        print ( "\n[ MQTT ] Stopping ... " )
    finally :
        client . loop_stop ()
        client . disconnect ()

if __name__ == "__main__":
    main()