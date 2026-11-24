# LineageOS PSP

**Version : 0.1.5 (profil PPSSPP)**

LineageOS PSP est une évolution du projet homebrew **CyanogenPSP** de Joel16. Le cœur de l'application est récupéré depuis le dépôt original, puis adapté avec des couches propres à LineageOS PSP.

## Sources et attribution

- Projet de base : https://github.com/omarmc9203/CyanogenPSP
- Auteur d'origine : Joel16
- Révision CyanogenPSP utilisée : `0a98c8b0c7503b54612e4d8d2de1f3010bc48fa4`
- Licence CyanogenPSP/PSPSDK : [LICENSE-CyanogenPSP](LICENSE-CyanogenPSP)
- Bibliothèque graphique OSLib : https://github.com/dogo/oslib
- Révision OSLib utilisée : `c9ab75c7a2b9a255ed126a21288f9f4c389fb266`
- Licence OSLib : GPL-2.0, reproduite dans [LICENSE-OSLib](LICENSE-OSLib).

Les crédits et licences des composants tiers sont conservés. Les adaptations propres à LineageOS PSP sont appliquées pendant la compilation, sans modifier le dépôt original de Joel16. Les références d'identité héritées `CyanogenPSP` sont remplacées dans les fichiers sources compilés par `LineageOS PSP ver.0.1.5`. Le nom du dossier de travail interne reste `CyanogenPSP` uniquement pour les opérations de compilation.

## Couches LineageOS PSP 0.1.5

- Identité embarquée dans le module PSP et le titre EBOOT : `LineageOS PSP ver.0.1.5`.
- Version produit indépendante : 0.1.5.
- Compatibilité avec le PSPSDK actuel : anciennes déclarations de dates, volume, sélecteur graphique, gestionnaire de fichiers et récupération adaptées.
- OSLib est recompilée depuis ses sources avec le toolchain PSPDEV actif, au lieu de réutiliser l'ancienne bibliothèque binaire.
- Les préférences de premier démarrage de CyanogenPSP sont conservées : l'état `system/settings/boot.bin` reste persistant, afin que l'accueil initial ne soit pas redemandé à chaque lancement.

Le lancement reste manuel depuis le menu Jeux de la PSP ou depuis PPSSPP. Aucun remplacement du firmware ni démarrage automatique à l'allumage n'est effectué.

## Navigateur intégré : Tilefinch

Tilefinch est le navigateur officiel de LineageOS PSP 0.1.5. Le code source complet est récupéré depuis le dépôt amont et compilé pour PSP dans le workflow GitHub Actions, à la révision épinglée `c1a4b7ac97e8057bd5ec52a6a5383587090c2484`.

- Moteur HTML/CSS natif, QuickJS, interface PSP moderne, onglets, favoris et historique.
- HTTPS avec vérification des certificats et des noms d'hôte.
- Le bouton Internet de LineageOS PSP lance directement `ms0:/PSP/GAME/TILEFINCH/EBOOT.PBP`.
- Le dossier complet `TILEFINCH/` est livré à côté du dossier LineageOS PSP ; les deux doivent être extraits dans `PSP/GAME/`.

### DNS over HTTPS natif

Le workflow applique un petit overlay à la couche transport PSP de Tilefinch. Le résolveur libcurl est configuré pour utiliser Cloudflare DoH via `https://cloudflare-dns.com/dns-query`, avec validation TLS du serveur DoH, un résolveur asynchrone c-ares compilé pour PSP et une entrée de bootstrap épinglée pour éviter de devoir résoudre le nom du serveur DoH en DNS classique. Si la configuration DoH ne peut pas être appliquée, la configuration TLS du transport échoue au lieu de revenir silencieusement au DNS classique.

Cette modification est appliquée au moment de la compilation ; le dépôt amont Tilefinch reste inchangé. Les licences et notices amont sont conservées dans `TILEFINCH/NOTICES/`.

## Profil de compilation PPSSPP 0.1.5

Cette version configure le tas principal avec `PSP_HEAP_SIZE_KB(53248)`, valeur explicitement documentée dans le code source CyanogenPSP pour les tests sous PPSSPP. Le profil est destiné à l'émulateur ; il ne modifie pas le modèle de PSP déclaré par PPSSPP.

Pour simuler une PSP-3000, sélectionner **PSP-2000/3000** dans les paramètres système de PPSSPP. Le choix du modèle est un réglage de l'émulateur, indépendant du fichier EBOOT.

## Démarrage 0.1.5

L'animation de démarrage et le sélecteur de langue initial sont temporairement contournés. Le programme entre directement dans le flux normal de premier démarrage, écran de verrouillage ou accueil. L'animation sera réintroduite après stabilisation du démarrage.

## Fonctionnalités héritées

La base CyanogenPSP fournit notamment son interface homebrew, écran de verrouillage, menus système, gestionnaire de fichiers, galerie, lecteur multimédia, lanceur de jeux et paramètres. Le navigateur NetFront hérité n'est plus l'entrée Internet par défaut : celle-ci lance Tilefinch.

Certaines fonctions décrites comme expérimentales ou inachevées dans le projet d'origine restent expérimentales ou inachevées ici. La messagerie adhoc n'est pas présentée comme pleinement fonctionnelle.

## Compilation

Le workflow 0.1.5 applique le profil mémoire PPSSPP de 53248 Ko, récupère Tilefinch à une révision déterminée, applique l'overlay DoH puis compile les deux applications.

Le workflow :
1. récupère les révisions amont épinglées ;
2. recompile OSLib pour le PSPSDK utilisé ;
3. applique les adaptations de compatibilité et d'identité LineageOS PSP ;
4. compile les modules PRX requis ;
5. compile Tilefinch avec le transport PSP natif et son résolveur DoH ;\n6. compile LineageOS PSP et publie les deux dossiers d'installation.

Le ZIP de distribution contient deux dossiers à sa racine : `LineageOS PSP ver.0.1.5/` et `TILEFINCH/`. Extraire les deux directement dans `PSP/GAME/`. Le dossier `TILEFINCH/` doit rester à cet emplacement pour que le bouton Internet de LineageOS PSP puisse lancer le navigateur.

**Important :** le dossier de distribution porte le nom versionné `LineageOS PSP ver.0.1.5`. Le dossier `CyanogenPSP` n'existe que dans l'environnement de compilation. Tilefinch conserve son propre dossier d'application `TILEFINCH`.

**Important :** une compilation réussie ne remplace pas un test sur PPSSPP et sur une PSP physique. Le firmware et le menu XMB officiels restent intacts.
