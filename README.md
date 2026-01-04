# Projet de systèmes informatiques
## Le projet
Ce projet s'inscrit dans le cadre du cours de systèmes informatiques.

Le rapport du projet est disponible en PDF [rapport.pdf](./rapport.pdf). 
Le code source (LaTeX) du rapport est le fichier [rapport.tex](./rapport.tex)

## Dépendances du projet
- WiringPi ([github](https://github.com/WiringPi/WiringPi))

## Instalation et compilation du programme
Avant de pouvoir compiler le programme avec la commande `make`, il faut installer l'unique dépendance de ce projet, WiringPi. Pour ce faire, il suffit de télécharger la version `3.16` de la manière suivante:
```bash
wget https://github.com/WiringPi/WiringPi/releases/download/3.16/wiringpi_3.16_arm64.deb
sudo apt install ./wiringpi_3.16_arm64.deb
```

## Vérification des fuites mémoires

(Utiliser -O0 sur gcc pour afficher les numéro de lignes)

```
valgrind --trace-children=yes --child-silent-after-fork=no --leak-check=full --show-leak-kinds=all ./bin/math_suite --input input.txt
```
