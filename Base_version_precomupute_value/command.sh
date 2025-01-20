echo "Il y a certains calculs qui sont fait à chaque boucle alors qu'il change jamais"
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
