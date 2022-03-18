update:
	@echo "clean workspace"
	rm -rf ./mosquitto
	rm -rf ./libs/mosquitto
	mkdir -p ./libs/mosquitto/src

	@echo "clone mosquitto library"
	git clone https://github.com/eclipse/mosquitto.git ./mosquitto
	cd mosquitto; git checkout v2.0.14; cd ..

	@echo "copy mosquitto files"
	cp ./mosquitto/config.h ./libs/mosquitto/src/config.h
	cp ./mosquitto/lib/*.h ./libs/mosquitto/src/
	cp ./mosquitto/lib/*.c ./libs/mosquitto/src/
	cp ./mosquitto/include/*.h ./libs/mosquitto/src/
	cp ./mosquitto/deps/*.h ./libs/mosquitto/src/

	@echo "remove temporary files"
	rm -rf ./mosquitto

fmt:
	clang-format -i src/*.h src/*.cpp -style="{BasedOnStyle: Google, ColumnLimit: 120}"
