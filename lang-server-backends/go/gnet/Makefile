gnet-ffead-cpp: clean
	go mod download 
	go build -gcflags="-l=4" -ldflags="-s -w" -o gnet-ffead-cpp .

clean:
	rm -f gnet-ffead-cpp