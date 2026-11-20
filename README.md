# LineageOS PSP

**Version de développement : 0.1.0**

LineageOS PSP est une évolution du projet homebrew **CyanogenPSP** de Joel16. Le cœur de l'application est récupéré depuis le dépôt original, puis adapté avec des couches propres à LineageOS PSP.

## Source de base et attribution

- Projet d'origine : https://github.com/omarmc9203/CyanogenPSP
- Auteur crédité dans la licence d'origine : Joel16
- Révision source utilisée pour la version 0.1.0 : `0a98c8b0c7503b54612e4d8d2de1f3010bc48fa4`
- La licence d'origine est conservée dans [LICENSE-CyanogenPSP](LICENSE-CyanogenPSP).

Le code original n'est pas présenté comme ayant été écrit par l'équipe LineageOS PSP. Les adaptations propres au projet sont maintenues séparément dans `overlays/lineage/`.

## Couches LineageOS PSP 0.1.0

- Identité du module et titre de lancement LineageOS PSP.
- Version produit indépendante : 0.1.0.
- Compilation automatisée avec PSPSDK dans GitHub Actions.
- Les écrans et préférences de premier démarrage de CyanogenPSP sont conservés : l'état `system/settings/boot.bin` est persistant, afin que l'accueil initial ne soit pas redemandé à chaque lancement.
- Le lancement reste manuel depuis le menu Jeux de la PSP ou depuis PPSSPP. Aucun remplacement du firmware ni démarrage automatique à l'allumage n'est effectué.

## Fonctionnalités héritées

La base CyanogenPSP fournit notamment son interface homebrew, écran de verrouillage, menus système, gestionnaire de fichiers, galerie, lecteur multimédia, lanceur de jeux, paramètres et navigateur NetFront du système PSP.

Certaines fonctions décrites comme expérimentales ou inachevées dans le projet d'origine restent expérimentales ou inachevées ici. En particulier, la messagerie adhoc n'est pas présentée comme pleinement fonctionnelle.

## Compilation

Le workflow :
1. récupère la révision amont épinglée ;
2. applique les adaptations de version et d'identité ;
3. compile les modules PRX requis par l'application ;
4. compile et publie le fichier autonome `EBOOT.PBP`.

Le fichier de sortie est destiné à être placé dans `PSP/GAME/LineageOS-PSP/EBOOT.PBP`.

**Important :** une compilation réussie ne remplace pas un test sur PPSSPP et sur une PSP physique. Le firmware et le menu XMB officiels restent intacts.
