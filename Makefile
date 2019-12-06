update:
	@echo "clean workspace"
	rm -rf ./mosquitto
	rm -rf ./libs/mosquitto
	mkdir -p ./libs/mosquitto/src

	@echo "clone mosquitto library"
	git clone https://github.com/eclipse/mosquitto.git ./mosquitto
	cd mosquitto; git checkout v1.5.8; cd ..

	@echo "copy mosquitto files"
	cp ./mosquitto/config.h ./libs/mosquitto/src/config.h
	cp ./mosquitto/lib/*.h ./libs/mosquitto/src/
	cp ./mosquitto/lib/*.c ./libs/mosquitto/src/
	# cp ./mosquitto/src/deps/utlist.h ./libs/mosquitto/src/utlist.h

	@echo "remove temporary files"
	rm -rf ./mosquitto

fmt:
	clang-format -i src/*.h src/*.cpp -style="{BasedOnStyle: Google, ColumnLimit: 120}"
