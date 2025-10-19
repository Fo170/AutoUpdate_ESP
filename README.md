# AutoUpdate_ESP v1.0.0
Mise à jour automatique (bibliothèque qui fournit des éléments interactifs pour vos mises à jour en direct sur ESP8266 et ESP32).

## 🎯 Description  
AutoUpdate_ESP est un projet visant à automatiser la mise à jour des firmwares ou programmes pour des modules ESP (tels que les ESP8266 ou ESP32). Grâce à ce système, les microcontrôleurs connectés peuvent vérifier périodiquement si une nouvelle version est disponible, télécharger et installer de nouveaux fichiers firmware si besoin.

## 🛠️ Fonctionnalités  
- Détection automatique de la version actuellement installée sur l’ESP.  
- Téléchargement de la nouvelle version depuis un serveur ou dépôt.   
- Mise à jour “OTA” (Over-The-Air) pour éviter l’intervention manuelle.    
- Compatibilité avec différents modèles d’ESP (ESP8266, ESP32…).  
- Option de déclenchement manuel ou programmé (via intervalle ou déclencheur réseau).

## 🔮 idées future
- faire une détection dans le .bin
- Ajout de la vérification de la signature du micrologiciel
- Mise en œuvre des mises à jour delta (téléchargement des modifications uniquement)
- Interface web pour la gestion des mises à jour
- Prise en charge de plusieurs canaux de micrologiciel (stable, bêta, développement)
- Restauration automatique en cas d'échec de démarrage
- Rappel de progression pour une interface utilisateur personnalisée

# AutoUpdate_ESP v1.0.0
Auto Update (library which provides interactive elements for your over-the-air updates on ESP8266 and ESP32).

## 🎯 Description
AutoUpdate_ESP is a project aimed at automating firmware or program updates for ESP modules (such as the ESP8266 or ESP32). This system allows connected microcontrollers to periodically check for new versions and download and install new firmware files if needed.

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
