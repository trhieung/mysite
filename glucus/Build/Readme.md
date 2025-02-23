# Docker build instruction
```
docker build -t python:3.11.0-tools .
docker run --rm -it -p 31337:31337 python:3.11.0-tools

```

# Nmap testing
## Installation
Go search f4 yourself :v

## Local running
1. windows as host, docker-container as target
``` target
ncat -lvnp 1234 -e /bin/bash
```

``` host
ncat 127.0.0.1 31337
```