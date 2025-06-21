# Tchat TCP en C++ avec Qt6

<!-- CORRECTION : J'ai mis à jour le texte alternatif de l'image pour qu'il corresponde au projet. -->
![Démonstration du Tchat TCP](https://raw.githubusercontent.com/AlexisRevol/tcp-tchat/main/.github/assets/demo_tchat.gif)

> Tchat client-serveur multi-threadé développé en C++17, utilisant les sockets TCP pour la communication, CMake pour la compilation, et une interface graphique moderne avec Qt6.

<p align="center">
  <!-- SUGGESTION : Les badges sont plus percutants quand ils sont groupés et centrés. -->
  <!-- Qualité & CI/CD -->
  <a href="https://github.com/AlexisRevol/tcp-tchat/actions/workflows/cpp-ci.yml"><img src="https://github.com/AlexisRevol/tcp-tchat/actions/workflows/cpp-ci.yml/badge.svg" alt="CI/CD Status"></a>
  <a href="https://www.codefactor.io/repository/github/alexisrevol/tcp-tchat"><img src="https://www.codefactor.io/repository/github/alexisrevol/tcp-tchat/badge" alt="CodeFactor"></a>
  <!-- Technologies -->
  <br />
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C.svg?logo=cplusplus" alt="C++17">
  <img src="https://img.shields.io/badge/Qt-6-41CD52.svg?logo=qt" alt="Qt6">
  <img src="https://img.shields.io/badge/CMake-3.12+-064F8C.svg?logo=cmake" alt="CMake">
  <img src="https://img.shields.io/badge/Tests-Catch2-9B499E.svg" alt="Tests with Catch2">
  <!-- Plateformes & Licence -->
  <br />
  <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg" alt="Multi-platform">
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License: MIT"></a>
</p>

---

## Fonctionnalités

-   **Architecture Client-Serveur** : Un serveur central gère les connexions et la diffusion des messages à plusieurs clients simultanément.
-   **Serveur Multi-threadé** : Chaque client est géré dans un thread dédié pour des communications non-bloquantes et performantes.
-   **Deux Types de Clients** :
    -   `Client Graphique` : Interface moderne et intuitive développée avec **Qt6**.
    -   `Client Console` : Client léger pour une utilisation en terminal, idéal pour le débogage.
-   **Fonctionnalités Essentielles** :
    -   Notifications de connexion/déconnexion des utilisateurs.
    -   Diffusion des messages à tous les participants en temps réel.
    -   Affichage de la liste des utilisateurs connectés.

## 🛠️ Prérequis

Ce projet requiert les outils suivants :

-   **CMake** : version 3.12 ou supérieure.
-   **Compilateur C++17** : GCC, Clang, ou MSVC.
-   **Qt6** : `Core` et `Widgets` sont nécessaires.

## Démarrage

### 1. Cloner le Dépôt
```bash
git clone https://github.com/AlexisRevol/tcp-tchat.git
cd tcp-tchat
```

### 2. Compiler le Projet
Le projet utilise une configuration "out-of-source" pour garder le répertoire principal propre.
```bash
# Crée un répertoire de build et configure le projet
cmake -B build

# Compile tous les exécutables (serveur, client_qt, client_console, tests)
cmake --build build
```

Les binaires seront générés dans build/server/, build/client_qt/, etc. (ou build/Debug/... sur Windows avec Visual Studio).

## Utilisation

Après la compilation, on obtient 3 exécutables principaux.

1.  **Lancer le serveur :**
    Le serveur se met en écoute sur localhost (port 8080).
    ```bash
    ./build/server/server
    ```

2.  **Lancer un ou plusieurs Clients**
       - Client Graphique (Qt)
            ```bash
            ./build/client_qt/client_qt
            ```
       - Client Console
          ```bash
          ./build/client/client
          ```

## Lancer les Tests

Le projet inclut des tests unitaires avec le framework Catch2. Pour les exécuter :
```bash
cd build
ctest --output-on-failure
```
