# MonCompilateur

Ce projet est un mini-compilateur développé en C++ pour traduire un programme écrit dans un langage Pascal simplifié en assembleur **x86_64**, puis en exécutable **ELF**.

## Fonctionnement

- Le fichier `compilateur.cpp` génère un fichier assembleur `test.s` à partir d’un fichier source Pascal (`test.p` ou `tests/test_tpX.p`).
- Ensuite, on utilise `gcc` pour produire un exécutable.
- Le programme peut être exécuté directement ou débogué avec `ddd`.

## Fonctionnalités par TP

### TP1 : Calculs arithmétiques
- Prise en charge des opérations : `+`, `-`, `*`, `/`
- Affectation simple : `a := 5 + 2 * 3;`
- Génération du code assembleur correspondant

### TP2 : Logique, comparaisons et variables
- Déclaration de variables globales : `[a, b, c]`
- Comparateurs relationnels : `==`, `!=`, `<`, `>`, `<=`, `>=`
- Opérateurs logiques : `&&`, `||`, `!`
- Valeurs booléennes : `0` (faux), `-1` (vrai)

### TP3 : Instructions de contrôle
- Instructions conditionnelles : `IF ... THEN ... [ELSE ...]`
- Boucles : `WHILE ... DO ...`, `FOR i := ... TO ... DO ...`
- Blocs : `BEGIN ... ; ... END`


### TP4 : Gestion des types
- Vérification automatique des types dans toutes les expressions
- Détection des erreurs de type : affectation incorrecte, mauvais type de condition
- Ajout d’un système de type (UNSIGNED_INT, BOOLEAN)

### TP5 : Instruction DISPLAY
- Ajout de l’instruction DISPLAY pour afficher une expression entière
- Génération du code assembleur d’affichage conforme à la convention d’appel de `printf`
- Gestion du format de sortie

---

###  TP6 : Déclaration de variables typées (en cours)
- Syntaxe de déclaration typée inspirée du Pascal
- Déclarations multiples avec séparation par `;` et `:`
- Gestion correcte des types dès la déclaration
- Vérification des doublons

---

## Utilisation


#### Compilation du compilateur :
-🔨 Compilation du compilateur:
```bash
make compilateur
```

-🚀 Génération d’un programme assembleur et exécutable

```bash
./compilateur < tests/test_tpX.p > test.s
gcc -ggdb -no-pie -fno-pie test.s -o test
./test

```
-Débogage avec DDD:

```bash
ddd ./test

```