
## Packages for Ubuntu 22.04

```bash
sudo apt-get install -y build-essential
sudo apt-get install -y libserial-dev

sudo apt-get install -y qtbase5-dev
sudo apt-get install -y qtdeclarative5-dev
sudo apt-get install -y qtwayland5
sudo apt-get install -y qml-module-qtquick-controls
sudo apt-get install -y qml-module-qtquick-controls2
```

## Permissions

```bash
sudo usermod -a -G dialout $USER
sudo usermod -a -G plugdev $USER
sudo reboot
```
