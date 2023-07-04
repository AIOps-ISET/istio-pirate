build-pilot-image:
	cd ./docker-build && docker build -t local/pilot:latest .

copy-so:
	cp  /usr/local/lib/libyaml.so ./docker-build
	cp /usr/local/lib/libwebsockets.so.18 ./docker-build
	cp /usr/local/lib/libkubernetes.so ./docker-build
	cp /lib/x86_64-linux-gnu/libcrypto.so.1.1 ./docker-build
	cp /lib/x86_64-linux-gnu/libssl.so.1.1 ./docker-build

copy-app:
	cp ../cmake-build-debug-/pilot-app ./docker-build

create-pilot-pod:
	cd ./docker-build && kubectl create -f pilot-test.yaml
