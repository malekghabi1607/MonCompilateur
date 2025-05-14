# =========================
# MonCompilateur Makefile
# =========================

# Cibles
all: test

# Nettoyage
clean:
		rm -f *.o *.s test compilateur tokeniser.cpp

# Analyseur lexical avec Flex++
tokeniser.cpp: tokeniser.l
		flex++ -d -o tokeniser.cpp tokeniser.l

# Compilation de l'analyseur
tokeniser.o: tokeniser.cpp
		g++ -Wall -Wextra -std=c++11 -c tokeniser.cpp

# Compilation du compilateur principal
compilateur: compilateur.cpp tokeniser.o
		g++ -Wall -Wextra -ggdb -std=c++11 -o compilateur compilateur.cpp tokeniser.o

# Génération et exécution du test
test: compilateur test.p
		./compilateur < test.p > test.s
		echo '\t.section .note.GNU-stack,"",@progbits' >> test.s
		gcc -ggdb -no-pie -fno-pie test.s -o test
		@echo "Compilation terminée avec succès. Lancez ./test"


# 🎯 Règle make test_tp4 : compile le compilateur + lance le test_tp4.p
test_tp4:
	./compilateur < tests/test_tp4.p > test.s
	gcc -ggdb -no-pie -fno-pie test.s -o test
	./test
