openssl genrsa -aes256 -out rootCA.key 4096
openssl req -new -key rootCA.key -x509 -out rootCA.crt -days 7650
openssl req -new -nodes -newkey rsa:4096 -keyout server.key -out server.req -batch -subj "/C=UN/ST=UN/L=UN/O=UN CA/OU=UN/CN=localhost" -reqexts SAN -config <(cat /etc/ssl/openssl.cnf <(printf "[SAN]\nsubjectAltName=DNS:localhost,IP:192.168.1.1"))
openssl x509 -req -in server.req -CA rootCA.crt -CAkey rootCA.key -CAcreateserial -out server.crt -days 3650 -sha256 -extfile <(printf "subjectAltName=DNS:localhost,IP:192.168.1.1")
cat server.key > serverchain.pem
cat server.crt >> serverchain.pem
cat rootCA.crt >> serverchain.pem
openssl dhparam -out dparams.pem 4096