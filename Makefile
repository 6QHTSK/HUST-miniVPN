all:
	cd app && make all
	docker-compose build
	docker-compose up -d
clean:
	cd app && make clean
	rm -rf release
	docker-compose down -v --rmi all --remove-orphans
