# MonCompilateur

Ce projet est un mini-compilateur développé en C++ pour traduire un programme en langage Pascal simplifié en assembleur **x86_64**, puis en exécutable **ELF**.

## Fonctionnement

- Le fichier `compilateur.cpp` génère un fichier assembleur `test.s` à partir du fichier source Pascal `test.p`.
- Ensuite, on utilise `gcc` pour générer l'exécutable.
- Le programme peut être exécuté ou débogué avec `ddd`.

## Utilisation

### Compilation du compilateur :
```bash
g++ compilateur.cpp -o compilateur
