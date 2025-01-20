echo "Il y a certains calculs qui sont fait à chaque boucle alors qu'il change jamais"
for i in {1..10};
do
	echo "Run $i"
	./BSM 1000 1000;
done
echo "End Computing"
