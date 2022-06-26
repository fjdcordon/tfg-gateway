import socket
from datetime import datetime
from protocols import propProto
import os


def impr(packet, type):
    pkt_type = getattr(packet, "type")
    pkt_action = getattr(packet, "action")
    pkt_error = getattr(packet, "error")
    pkt_content = getattr(packet, "content").decode('utf-8')

    print(str(datetime.now()) +
          " -- [" + type + "] -- Type: " + str(pkt_type),
          ", Action: " + str(pkt_action),
          ", Error: " + str(pkt_error),
          ", Content: " + pkt_content)


sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.listen(1)

while True:
    connection, client_address = sock.accept()
    try:
        data = connection.recv(50)
        if data:
            packet = propProto(data)
            pkt_type = getattr(packet, "type")
            pkt_action = getattr(packet, "action")
            pkt_error = getattr(packet, "error")
            pkt_content = getattr(packet, "content").decode('utf-8')

            impr(packet, "REQ")

            if pkt_type == 0 and 5 >= pkt_action >= 0 == pkt_error:
                if pkt_action == 0:
                    err = 0
                    cnt = None

                    try:
                        energia = os.popen("/root/driver energy").read()
                        if not 0 <= float(energia) <= 500:
                            err = 1
                        else:
                            cnt = energia
                    except:
                        err = 1

                elif pkt_action == 1:
                    err = 0
                    cnt = None

                    try:
                        angulo = os.popen("/root/driver angle").read()
                        if not 0 <= float(angulo) <= 90:
                            err = 1
                        else:
                            cnt = angulo
                    except:
                        err = 1

                elif pkt_action == 2:
                    err = 0
                    cnt = None

                    try:
                        estado = os.popen("/root/driver status").read()
                        if estado=='on':
                            cnt = "Encendido"
                        elif estado=='off':
                            cnt = "Apagado"
                        else:
                            err =1
                    except:
                        err = 1

                elif pkt_action == 3:
                    err = 0
                    cnt = None

                    try:
                        salida = os.popen("/root/driver on").read()
                        if salida != 'ok':
                            err = 1
                    except:
                        err = 1

                elif pkt_action == 4:
                    err = 0
                    cnt = None

                    try:
                        salida = os.popen("/root/driver off").read()
                        if salida != 'ok':
                            err = 1
                    except:
                        err = 1
                else:
                    err = 0
                    cnt = None

                    try:
                        salida = os.popen("/root/driver move "+pkt_content).read()
                        if salida != 'ok':
                            err = 1
                    except:
                        err = 1

                resp_packet = propProto(type=1, action=pkt_action, error=err, content=str(cnt))
            else:
                resp_packet = propProto(type=1, action=pkt_action, error=1, content=None)

            impr(resp_packet, "RESP")
            print("")
            connection.sendall(bytes(resp_packet))

    finally:
        connection.close()
