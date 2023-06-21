all:
	docker build -t vpn .
	docker run --name vpn vpn
	docker cp vpn:/app/release ..
clean:
	if docker ps -a | grep -q vpn; then docker stop vpn && docker rm vpn && docker rmi vpn; fi