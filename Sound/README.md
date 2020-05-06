# sound classification node

## Installation

1. install yamnet
  refer to [yamnet installation](./README-yamnet.md).

2. install signal_node
  refer to [signal_node setup](./signal_node/README.md).

3. download bag file and network weights

```shell
wget https://file.epikem.com/yamnet.h5

wget https://file.epikem.com/bags.zip
```

## Usage

running `python audio_ros.py` will publish data as `/audio` ros topic.

running `rosrun signal_node signal_node.py` and above `audio_ros.py` together will publish `/signal` ros topic.

## TODO:

- [ ] add `requirements.txt`.
- [x] refactor code : 
    - [x] rename script names
    - [x] merge `audio_ros.py` and `signal_node.py`

