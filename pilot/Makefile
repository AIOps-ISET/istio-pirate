build-pilot-dev-base:
	cd ./docker-build && docker build --target pilot-dev-base -t local/pilot-dev-base:latest .

build-pilot-runtime-base:
	cd ./docker-build && docker build --target pilot-runtime-base -t local/pilot-runtime-base:latest .

build-pilot: copy-sources
	cd ./docker-build && docker build --target build-pilot -t local/build-pilot:latest .

build-pilot-only-image: copy-sources
	cd ./docker-build && docker build --target pilot -t local/pilot:latest .

copy-sources:
	rm -r ./docker-build/istio-pirate
	mkdir -p ./docker-build/istio-pirate/pilot/src
	cp -r ./src ./docker-build/istio-pirate/pilot/
	cp CMakeLists.txt ./docker-build/istio-pirate/pilot/
	cp ../CMakeLists.txt ./docker-build/istio-pirate

create-pilot-pod:
	cd ./k8s-resources && kubectl create -f pilot-test.yaml
