# DNS Rebinder


## Foreword

- Simple Dns Server (A and PTR records) along with simple payloads to achieve a DNS rebinding attack.
- The DNS server has been developped on MacOS and might not compile on other plateform.

- https://tools.ietf.org/html/rfc1035

## Building and Running the DNS server

- Install SCons:https://scons.org/doc/0.98.4/HTML/scons-user/x166.html

- Run scons

- ./dns_server port hosts_file

## How it works

 - Run the http server on the same port than the target device: python3 main.py IP PORT

 - Run the node server on the ip and port you want to connect to to browse the target device once it's been hooked

 - Run the DNS server

 - Once the victim has been rebound, you can freely browse the target device as if you were inside the victim's LAN by brtowsing the node server.


