main:compile
	./main -i "./src/input.txt" -o "./src/output.txt" -x 4212 -a 84589 -c 45989 -m 217728
compile:CLib
	g++ OtpLib.o main.cpp -o main -pthread
CLib:
	g++ OtpLib.cpp -c
