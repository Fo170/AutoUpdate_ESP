# AutoUpdate_ESP v1.0.4
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Arduino Badge](https://img.shields.io/badge/framework-arduino-brightgreen?logo=arduino.svg)](https://www.arduino.cc/)
[![PlatformIO](https://img.shields.io/badge/platformio-ESP8266,ESP32-violet?logo=platformio)](https://platformio.org/)
[![Arduino Library Badge](https://www.ardu-badge.com/badge/AutoUpdate_ESP.svg)](https://github.com/Fo170?tab=repositories)

Mise à jour automatique (bibliothèque qui fournit des éléments interactifs pour vos mises à jour en direct sur ESP8266 et ESP32).

## 🎯 Description  
AutoUpdate_ESP est un projet visant à automatiser la mise à jour des firmwares ou programmes pour des modules ESP (tels que les ESP8266 ou ESP32). Grâce à ce système, les microcontrôleurs connectés peuvent vérifier périodiquement si une nouvelle version est disponible, télécharger et installer de nouveaux fichiers firmware si besoin.

## 🛠️ Fonctionnalités  
- Détection automatique de la version actuellement installée sur l’ESP.  
- Téléchargement de la nouvelle version depuis un serveur ou dépôt.   
- Mise à jour “OTA” (Over-The-Air) pour éviter l’intervention manuelle.    
- Compatibilité avec différents modèles d’ESP (ESP8266, ESP32…).  
- Option de déclenchement manuel ou programmé (via intervalle ou déclencheur réseau).

## 📦 Dépendances

`AutoUpdate()` utilise `NON_BLOCKING_DELAY(...)` pour ses attentes (plus aucun `delay()`). Cette macro provient de la bibliothèque header-only `NON_BLOCKING_DELAY` (`Delay.h`) : **le header `AutoUpdate_ESP.h` l'inclut lui-même**, il suffit donc de la déclarer dans `lib_deps` (elle ne peut pas l'être dans `dependencies` du `library.json`, car `NON_BLOCKING_DELAY` n'est pas au registre PlatformIO — d'où l'URL git) :

```ini
lib_deps = 
    https://github.com/Fo170/AutoUpdate_ESP.git@^1.0.4
    https://github.com/Fo170/NON_BLOCKING_DELAY.git@^1.0.0
```

Dans le sketch, seul `#include <AutoUpdate_ESP.h>` est nécessaire (une fois `FIRMWARE_VERSION`, `FIRMWARE_URL_BIN` et `FIRMWARE_URL_TXT` définis).

- **ESP32** : rien d'autre à déclarer — `WiFi`, `HTTPClient`, `Update` et `HTTPUpdate` font partie du cœur Arduino ESP32. `NON_BLOCKING_DELAY` utilise FreeRTOS `vTaskDelay()` : la tâche s'endort, les tâches de plus basse priorité (WiFi, idle…) continuent de tourner.
- **ESP8266** : depuis le core Arduino ESP8266 3.1+, le header `HTTPClient.h` a été renommé `ESP8266HTTPClient.h` et la bibliothèque `ESP8266HTTPClient` n'est plus trouvée automatiquement par PlatformIO. Il faut l'ajouter au `platformio.ini` :
  ```
  lib_deps = 
      ESP8266HTTPClient
  ```
  et inclure `<ESP8266HTTPClient.h>` (et non `<HTTPClient.h>`) dans le sketch. Sur ESP8266, `NON_BLOCKING_DELAY` fait un busy-wait `millis()` + `yield()` (robuste au dépassement de `millis()`) : le WiFi reste actif pendant l'attente.
- **LED optionnelle** : par défaut, aucune LED n'est utilisée pendant la mise à jour. Pour faire clignoter la LED sur `LED_BUILTIN` pendant le flash, définir `Use_LED_BUILTIN` (et `LED_BUILTIN` si le core/board ne le fournit pas) **avant** `#include <AutoUpdate_ESP.h>` :
  ```cpp
  #define Use_LED_BUILTIN
  #define LED_BUILTIN 2 // si le core ne le fournit pas
  ```

## 🔮 idées future
- faire une détection dans le .bin
- Ajout de la vérification de la signature du micrologiciel
- Mise en œuvre des mises à jour delta (téléchargement des modifications uniquement)
- Interface web pour la gestion des mises à jour
- Prise en charge de plusieurs canaux de micrologiciel (stable, bêta, développement)
- Restauration automatique en cas d'échec de démarrage
- Rappel de progression pour une interface utilisateur personnalisée

# AutoUpdate_ESP v1.0.4
Auto Update (library which provides interactive elements for your over-the-air updates on ESP8266 and ESP32).

## 🎯 Description
AutoUpdate_ESP is a project aimed at automating firmware or program updates for ESP modules (such as the ESP8266 or ESP32). This system allows connected microcontrollers to periodically check for new versions and download and install new firmware files if needed.

## 📦 Installation

### Via Arduino IDE
1. Téléchargez cette bibliothèque en format ZIP
2. Dans Arduino IDE : **Croquis → Inclure une bibliothèque → Ajouter la bibliothèque .ZIP**
3. Sélectionnez le fichier ZIP téléchargé

### Via PlatformIO (recommandé)

Ajoutez la dépendance à votre `platformio.ini` :

```ini
# Pour cartes standard (ESP32, ESP8266, etc.)
lib_deps = 
    https://github.com/Fo170/AutoUpdate_ESP.git@^1.0.4
    https://github.com/Fo170/NON_BLOCKING_DELAY.git@^1.0.0
```

## 📦 Dependencies

`AutoUpdate()` uses `NON_BLOCKING_DELAY(...)` for its wait periods (no more `delay()`). This macro comes from the header-only `NON_BLOCKING_DELAY` library (`Delay.h`): **`AutoUpdate_ESP.h` includes it by itself**, so you only need to declare the library in `lib_deps` (it cannot be declared in the `library.json` `dependencies`, because `NON_BLOCKING_DELAY` is not in the PlatformIO registry — hence the git URL):

```ini
lib_deps = 
    https://github.com/Fo170/AutoUpdate_ESP.git@^1.0.4
    https://github.com/Fo170/NON_BLOCKING_DELAY.git@^1.0.0
```

In your sketch, only `#include <AutoUpdate_ESP.h>` is needed (after defining `FIRMWARE_VERSION`, `FIRMWARE_URL_BIN` and `FIRMWARE_URL_TXT`).

- **ESP32**: nothing else to declare — `WiFi`, `HTTPClient`, `Update` and `HTTPUpdate` are part of the Arduino ESP32 core. `NON_BLOCKING_DELAY` uses FreeRTOS `vTaskDelay()`: the task sleeps while lower-priority tasks (WiFi, idle…) keep running.
- **ESP8266**: since Arduino ESP8266 core 3.1+, the `HTTPClient.h` header has been renamed `ESP8266HTTPClient.h` and the `ESP8266HTTPClient` library is no longer discovered automatically by PlatformIO. Add it to `platformio.ini`:
  ```
  lib_deps = 
      ESP8266HTTPClient
  ```
  and include `<ESP8266HTTPClient.h>` (not `<HTTPClient.h>`) in your sketch. On ESP8266, `NON_BLOCKING_DELAY` does an overflow-safe `millis()` + `yield()` busy-wait, so the WiFi stack stays alive during the wait.
- **Optional LED**: by default, no LED is used during the update. To blink the `LED_BUILTIN` pin during flashing, define `Use_LED_BUILTIN` (and `LED_BUILTIN` if the core/board doesn't provide it) **before** `#include <AutoUpdate_ESP.h>`:
  ```cpp
  #define Use_LED_BUILTIN
  #define LED_BUILTIN 2 // if the core doesn't provide it
  ```

## 🛠️ Features
- Automatic detection of the version currently installed on the ESP.
- Downloading of the new version from a server or repository.
- Over-the-Air (OTA) updates to avoid manual intervention.
- Compatibility with various ESP models (ESP8266, ESP32, etc.).
- Manual or scheduled triggering options (via interval or network trigger).

## 🔮 Future ideas
- Perform detection in the .bin file
- Added firmware signature verification
- Implemented delta updates (download changes only)
- Web interface for managing updates
- Support for multiple firmware channels (stable, beta, development)
- Automatic rollback in case of boot failure
- Progress reminder for a custom user interface
