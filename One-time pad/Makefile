encode:compile
	./main -i "./src/input.txt" -o "./src/output.txt" -x 4212 -a 84589 -c 45989 -m 217728

decode:compile
	./main -i "./src/output.txt" -o "./src/temp.txt" -x 4212 -a 84589 -c 45989 -m 217728

clear:
	> ./src/output.txt
	> ./src/temp.txt

compile:CLib
	g++ OtpLib.o main.cpp -o main -pthread
CLib:
	g++ OtpLib.cpp -c