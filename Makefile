all:
	cd app && make all
	docker-compose build
clean:
	rm -rf release
	docker-compose down -v --rmi all --remove-orphans
