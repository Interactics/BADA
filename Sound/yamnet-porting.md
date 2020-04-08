# sound recognization ros node

[warning: not tested for recent changes, you may need extra step]


## install prerequisites for yamnet

1. install youtube-dl

`sudo -H pip install --upgrade youtube-dl`

2. install ffmpeg

`sudo apt install ffmpeg`

3. install python dependencies

before installing, make sure that you are using latest pip with command below:
`sudo -H pip3 install --upgrade pip`

`python3 -m pip install numpy resampy tensorflow==1.15 tf_slim six soundfile`

`python3 -m pip install matplotlib`

(warning: tensorflow version used on colab is 1.15 but it is not found on pip 9.0 version)

4. download yamnet weights

`curl -O https://storage.googleapis.com/audioset/yamnet.h5`

5. get wave input

```py
start_time="5"
end_time="15"
video_id='ArE_BqO-YPk'
out_file='out.wav'
tfrecord_file='embedding1.tfrecord'
```

`youtube-dl -x --postprocessor-args "-ss 00:00:{start_time}.00 -t 00:00:{end_time}.00 -ar 16000 out.wav" "https://youtu.be/{video_id}" --audio-format "wav"`


## clone yamnet models repo

copy tensorflow models 

`git clone https://github.com/tensorflow/models.git`

copy audioset yamnet model contents to current folder

`cp -r models/research/audioset/yamnet/* .`



# trying on headset device

## install pyaudio prerequisite

`sudo apt-get install portaudio19-dev`

`python3 -m pip install pyaudio`


run `audio_test2.py` with python to test the demo.


TODO: 

1. [ ] use callback to update frames
2. [x] make frame update frequency adjustable
3. [x] measure frame update time
4. [x] make ros node