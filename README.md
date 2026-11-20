# LineageOS PSP

**Version de développement : 0.1.0**

LineageOS PSP est une évolution du projet homebrew **CyanogenPSP** de Joel16. Le cœur de l'application est récupéré depuis le dépôt original, puis adapté avec des couches propres à LineageOS PSP.

## Sources et attribution

- Projet de base : https://github.com/omarmc9203/CyanogenPSP
- Auteur d'origine : Joel16
- Révision CyanogenPSP utilisée : `0a98c8b0c7503b54612e4d8d2de1f3010bc48fa4`
- Licence CyanogenPSP/PSPSDK : [LICENSE-CyanogenPSP](LICENSE-CyanogenPSP)
- Bibliothèque graphique OSLib : https://github.com/dogo/oslib
- Révision OSLib utilisée : `c9ab75c7a2b9a255ed126a21288f9f4c389fb266`
- Licence OSLib : GPL-2.0, reproduite dans [LICENSE-OSLib](LICENSE-OSLib).

Les crédits et licences des composants tiers sont conservés. Les adaptations propres à LineageOS PSP sont appliquées pendant la compilation, sans modifier le dépôt original de Joel16.

## Couches LineageOS PSP 0.1.0

- Identité du module et titre de lancement LineageOS PSP.
- Version produit indépendante : 0.1.0.
- Compatibilité avec le PSPSDK actuel : anciennes déclarations de dates, volume, sélecteur graphique, gestionnaire de fichiers et récupération adaptées.
- OSLib est recompilée depuis ses sources avec le toolchain PSPDEV actif, au lieu de réutiliser l'ancienne bibliothèque binaire.
- Les préférences de premier démarrage de CyanogenPSP sont conservées : l'état `system/settings/boot.bin` reste persistant, afin que l'accueil initial ne soit pas redemandé à chaque lancement.

Le lancement reste manuel depuis le menu Jeux de la PSP ou depuis PPSSPP. Aucun remplacement du firmware ni démarrage automatique à l'allumage n'est effectué.

## Fonctionnalités héritées

La base CyanogenPSP fournit notamment son interface homebrew, écran de verrouillage, menus système, gestionnaire de fichiers, galerie, lecteur multimédia, lanceur de jeux, paramètres et navigateur NetFront du système PSP.

Certaines fonctions décrites comme expérimentales ou inachevées dans le projet d'origine restent expérimentales ou inachevées ici. La messagerie adhoc n'est pas présentée comme pleinement fonctionnelle.

## Compilation

Le workflow :
1. récupère les révisions amont épinglées ;
2. recompile OSLib pour le PSPSDK utilisé ;
3. applique les adaptations de compatibilité et d'identité LineageOS PSP ;
4. compile les modules PRX requis ;
5. compile et publie le fichier autonome `EBOOT.PBP`.

Le fichier de sortie est destiné à être placé dans `PSP/GAME/LineageOS-PSP/EBOOT.PBP`.

**Important :** une compilation réussie ne remplace pas un test sur PPSSPP et sur une PSP physique. Le firmware et le menu XMB officiels restent intacts.
