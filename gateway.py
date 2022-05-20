import socket
from datetime import datetime
from protocols import propProto

def impr(packet, type):
    pkt_type = getattr(packet, "type")
    pkt_action = getattr(packet, "action")
    pkt_error = getattr(packet, "error")
    pkt_content = getattr(packet, "content").decode('utf-8')

    print(str(datetime.now()) +
        " -- ["+type+"] -- Type: " + str(pkt_type),
        ", Action: " + str(pkt_action),
        ", Error: " + str(pkt_error),
        ", Content: " + pkt_content)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('172.16.16.122', 5555)
print(str(datetime.now()) + ' -- Servicio corriendo en {}:{}'.format(*server_address)+"\n")
sock.bind(server_address)
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
                    cnt = 456.55
                elif pkt_action == 1:
                    err = 0
                    cnt = 27
                elif pkt_action == 2:
                    err = 0
                    cnt = "Encendido"
                elif pkt_action == 3:
                    err = 0
                    cnt = None
                elif pkt_action == 4:
                    err = 0
                    cnt = None
                else:
                    err = 0
                    cnt = None

                resp_packet = propProto(type=1, action=pkt_action, error=err, content=str(cnt))   
            else:
                resp_packet = propProto(type=1, action=pkt_action, error=1, content=None)

            impr(resp_packet, "RESP")
            print("")
            connection.sendall(bytes(resp_packet))

    finally:
        connection.close()
