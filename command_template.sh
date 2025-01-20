echo "Put the desc of what opt you do"
echo "gcc compiler"
g++ -O BSM.cxx -o BSM
for i in {1..10};
do
	echo "Run $i"
	./BSM 1000 1000;
done
echo "Arm compiler"
armclang++ -O BSM.cxx -o BSMarm
for i in {1..10};
do
	echo "Run $i"
	./BSMarm 1000 1000;
done
