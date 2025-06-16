# Tchat TCP en C++

Tchat client-serveur multi-threadé développé en C++17, utilisant les sockets TCP pour la communication, CMake pour la compilation, et une interface graphique moderne avec Qt6.


![Démo de Job Tracker](https://raw.githubusercontent.com/AlexisRevol/tcp-tchat/main/.github/assets/demo_tchat.gif)

### Badges & Qualité


| Catégorie          | Badge                                                                                                                                                                                                                                                                                                                                       |
| ------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Build & Test**   | [![CI/CD Workflow](https://github.com/AlexisRevol/tcp-tchat/actions/workflows/cpp-ci.yml/badge.svg)](https://github.com/AlexisRevol/tcp-tchat/actions/workflows/cpp-ci.yml)                                                                                                                                          |
| **Analyse de Code**| [![CodeFactor](https://www.codefactor.io/repository/github/alexisrevol/tcp-tchat/badge)](https://www.codefactor.io/repository/github/alexisrevol/tcp-tchat)                                                                                                                                                                         |
| **Langage & Outils**   | ![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C.svg?logo=cplusplus) ![CMake](https://img.shields.io/badge/CMake-3.12+-064F8C.svg?logo=cmake) ![Qt6](https://img.shields.io/badge/Qt-6-41CD52.svg?logo=qt) ![Tests](https://img.shields.io/badge/Tests-Catch2-9B499E.svg)                                                              |
| **Plateformes**    | ![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)                                                                                                                                                                                                                                        |
| **Licence**        | [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)                                                                                                                                                                                                                                  |

---

## Table des matières

- [Fonctionnalités](#fonctionnalités)
- [Structure du Projet](#structure-du-projet)
- [Prérequis](#prérequis)
- [Instructions de Compilation](#instructions-de-compilation)
- [Utilisation](#utilisation)
- [Lancer les Tests](#lancer-les-tests)
- [Choix Techniques et Architecture](#choix-techniques-et-architecture)
- [Améliorations Possibles](#améliorations-possibles)
- [Licence](#licence)

## Fonctionnalités

-   **Architecture Client-Serveur** : Un serveur central robuste et plusieurs clients pouvant se connecter simultanément.
-   **Communication TCP** : Communication fiable et ordonnée via les sockets TCP.
-   **Serveur Multi-threadé** : Le serveur gère chaque client dans un thread dédié pour une performance et une réactivité maximales.
-   **Client Non-bloquant** : La logique réseau du client s'exécute dans un thread séparé pour garantir une interface utilisateur toujours fluide.
-   **Multi-plateforme** : Le code est écrit pour être compatible avec Windows (via Winsock2) et les systèmes POSIX (Linux, macOS).
-   **Deux types de clients** :
    1.  Un **client graphique** moderne et intuitif développé avec **Qt6**.
    2.  Un **client console** léger pour le débogage ou une utilisation en terminal.
-   **Fonctionnalités de tchat de base** :
    -   Notification de connexion et de déconnexion des utilisateurs.
    -   Diffusion des messages à tous les participants.
    -   Affichage en temps réel de la liste des utilisateurs connectés.
-   **Code Modulaire** : Logique partagée (protocole, parsing) isolée dans une bibliothèque commune pour éviter la duplication de code.
-   **Tests Unitaires** : Infrastructure de tests en place avec le framework **Catch2**.

## Prérequis

Pour compiler et exécuter ce projet, vous aurez besoin de :

-   **CMake** (version 3.12 ou supérieure)
-   Un **compilateur C++17** (GCC, Clang, MSVC, etc.)
-   **Qt6** (avec les modules `Core` et `Widgets`)

## Instructions de Compilation

Le projet utilise CMake, ce qui rend la compilation simple et multi-plateforme.

1.  **Clonez le dépôt :**
    ```bash
    git clone https://votre-lien-vers-le-depot.git
    cd TCP-Tchat
    ```

2.  **Configurez le projet avec CMake :**
    Il est recommandé de faire une compilation "out-of-source" en créant un répertoire `build`.
    ```bash
    cmake -B build
    ```
    *Cette commande crée un répertoire `build` et y génère les fichiers de projet (Makefiles, solution Visual Studio, etc.).*

3.  **Compilez le projet :**
    ```bash
    cmake --build build
    ```
    *Cette commande lance la compilation de toutes les cibles (serveur, clients, tests). Pour accélérer, vous pouvez utiliser `cmake --build build -j N` où N est le nombre de cœurs de votre processeur.*

Les exécutables seront générés dans le répertoire `build/` (ou un sous-dossier comme `build/Debug/` sur Windows).

## Utilisation

Après la compilation, vous obtiendrez trois exécutables principaux.

1.  **Lancer le serveur :**
    Le serveur doit être lancé en premier. Il se mettra en écoute sur le port 8080.
    ```bash
    # Depuis la racine du projet
    ./build/server/server
    ```
    Le terminal affichera "Serveur en ecoute..."

2.  **Lancer le client graphique Qt :**
    Vous pouvez lancer plusieurs instances de ce client.
    ```bash
    # Depuis la racine du projet
    ./build/client_qt/client_qt
    ```

3.  **Lancer le client console :**
    Utile pour des tests rapides. Vous pouvez également en lancer plusieurs instances dans des terminaux différents.
    ```bash
    # Depuis la racine du projet
    ./build/client/client
    ```

## Lancer les Tests

Le projet est configuré pour utiliser CTest, l'outil de test de CMake. Pour lancer les tests unitaires :

```bash
# Assurez-vous d'être dans le répertoire de compilation
cd build
ctest
```
