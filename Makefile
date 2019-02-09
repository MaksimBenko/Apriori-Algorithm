.PHONY: all clean apriori debug

all: apriori debug

apriori: apriori.cpp
	g++ apriori.cpp -std=c++11 -o apriori -O3
debug: apriori.cpp
	g++ apriori.cpp -std=c++11 -o aprioriDeb -g


clean: apriori
	rm apriori
