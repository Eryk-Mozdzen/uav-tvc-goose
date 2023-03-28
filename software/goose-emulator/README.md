# Install dependencies
```bash
sudo apt install libserial-dev
```
# Create new virtual port
> **Warning**
> Always restart socets after killing emulator
```bash
socat -d -d pty,raw,echo=0 pty,raw,echo=1
```
Output:
```
2023/03/28 12:31:21 socat[35447] N PTY is /dev/pts/13
2023/03/28 12:31:21 socat[35447] N PTY is /dev/pts/14
2023/03/28 12:31:21 socat[35447] N starting data transfer loop with FDs [5,5] and [7,7]
```
For me `socat` opend `/dev/pts/13` and `dev/pts/14`.

# In new terminal build the emulator
```
mkdir build
cd build
make
```
# Run the emulator
> **Warning**
> Remember always run writing `goose-emulator` before reading!
```bash
./goose-emulator
```