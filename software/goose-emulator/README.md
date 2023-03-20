# Create new virtual port
```bash
socat -d -d pty,raw,echo=0 pty,raw,echo=1
```

# Echo the virtual port e.g.
```bash
cat < /dev/pts/5
```

# Build
```
mkdir build 
cd build
make 
```