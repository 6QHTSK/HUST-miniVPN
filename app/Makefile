all:
	docker build -t vpn-app .
	docker run --name vpn-app vpn-app
	docker cp vpn-app:/app/release ..
	docker stop vpn-app
	docker rm vpn-app
	docker rmi vpn-app