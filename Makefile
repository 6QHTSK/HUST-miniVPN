all:
	mkdir -p release
	cd release && cmake .. && make
	cd cert && make cert
	docker compose build
clean:
	rm -rf release
	cd cert && make clean