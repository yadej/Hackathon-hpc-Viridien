echo "Default Version"
g++ -O BSM.cxx -o BSM
for i in {1..10};
do
	echo "Run $i"
	./BSM 1000 1000;
done
