
localectl set-keymap --no-convert es
echo gateway > /etc/hostname
userdel -r vagrant
cp driver gateway.py protocols.py /root
pip install scapy
cp gtw.service /etc/systemd/system
systemctl enable gtw
cp eth0.network /etc/systemd/network
