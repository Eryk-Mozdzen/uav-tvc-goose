# Install dependencies
```bash
sudo apt install libserial-dev
```
# Create new virtual port
```bash
socat -d -d pty,raw,echo=0 pty,raw,echo=1
```
# Add permision
```bash
sudo chmod 777 /dev/pts/5
```
# In new terminal print the virtual port e.g.
```bash
cat < /dev/pts/5
```

# In new terminal build the emulator
```
mkdir build 
cd build
make 
```
# Run the emulator
```bash
./goose-emulator
```