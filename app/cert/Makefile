ca.key:
	openssl genrsa -out ca.key 2048
server.key:
	openssl genrsa -out server.key 2048
client.key:
	openssl genrsa -out client.key 2048
server.csr: server.key
	openssl req -new -key server.key -out server.csr -subj "/C=CN/ST=Hubei/L=Wuhan/O=HUST/OU=CSE/CN=<name>/emailAddress=<email>"
client.csr: client.key
	openssl req -new -key client.key -out client.csr -subj "/C=CN/ST=Hubei/L=Wuhan/O=HUST/OU=CSE/CN=<name>/emailAddress=<email>"
ca.crt: ca.key
	openssl req -x509 -new -nodes -sha512 -key ca.key -out ca.crt -subj "/C=CN/ST=Hubei/L=Wuhan/O=HUST/OU=CSE/CN=<name>/emailAddress=<email>"
	echo "1665" > ca.srl
server.crt: ca.crt server.csr
	openssl x509 -req -CA ca.crt -CAkey ca.key -in server.csr -out server.crt
client.crt: ca.crt client.csr
	openssl x509 -req -CA ca.crt -CAkey ca.key -in client.csr -out client.crt
expired_client.crt: ca.crt client.csr
	openssl x509 -req -days 0 -CA ca.crt -CAkey ca.key -in client.csr -out expired_client.crt
cert: client.crt server.crt expired_client.crt
clean:
	rm -f *.crt *.key *.csr ca.srl