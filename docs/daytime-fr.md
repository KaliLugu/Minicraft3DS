# Daytime

système qui permet de déterminer un moment de la journée

## architecture globale

c'est un système avec des ticks, chaque frame il y a 1tick ajouté jusqu'à 24000

quand ça atteint plus de 24000 ça soustrait 24000

## journée/nuit

la nuit est entre 18000 et 6000 tick

une nouvelle sauvegarde commence à 6000 tick au début de journée

le lit dans le jeu permet de ramener à 6000 tick
