from scapy.all import *

class propProto(Packet):
    name = "Protocolo propietario para la comunicación entre servidor web y gateway IoT"
    fields_desc = [
        BitField(name="type", default=0, size=1),
        BitField(name="action", default=0, size=6),
        BitField(name="error", default=0, size=1),
        StrField(name="content", default="")
    ]