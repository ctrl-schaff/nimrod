## Nimrod
Exploring process extension through forking

## Build
```
git clone git@github.com:ctrl-schaff/nimrod.git
python3 -m venv ./nimrod
pip3 install -r ./nimrod/requirements.txt
```

## Run
```
make
./nimrod
===
python3 ./genesis.py iteration
```

## Design
```
             +------+
             |AGENT |
             +------+
             |
             |
             |
             |
             +-----------------+
             |                 |
             |                 |
             |                 |
             v                 v
+-----+      +------+ SIGTERM  +-------+  SIGINT
|/PROC|<-----+HUNTER+--------->|VULTURE|<------------------------+
+-----+      +------+          +-------+                         |
                               |       |                         |
                               |       |SIGINT                   |
                               |       |                         |
                               |       +-----------------+       |
                               |       |                 |       |
                               |       |                 |       |
                               |       |                 |       |
                               v       v                 v       |
                               +----+  +-------+         +-------+       +-----+
                               |EXIT|  |VULTURE|         |MONITOR|<------+/PROC|
                               +----+  +-------+         +-------+       +-----+
```
