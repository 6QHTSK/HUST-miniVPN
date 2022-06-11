all:
	mkdir -p release
	cd release && cmake .. && make
	cd cert && make cert
	docker-compose build
clean:
	docker-compose down -v --rmi all --remove-orphans
	rm -rf release
	cd cert && make clean