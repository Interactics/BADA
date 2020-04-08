import pyaudio
import numpy as np
from matplotlib import pyplot as plt

import soundfile as sf

import params
import yamnet as yamnet_model

import time
import queue

# Set up the YAMNet model.
params.PATCH_HOP_SECONDS = 0.48  # 10 Hz scores frame rate. //0.1
yamnet = yamnet_model.yamnet_frames_model(params)
yamnet.load_weights('yamnet.h5')
class_names = yamnet_model.class_names('yamnet_class_map.csv')

CHUNKSIZE = 8000 # fixed chunk size
sr=16000
seconds=1
predictionPeriod=1
predictionRate=0.1
predChunkSize=int(sr*predictionPeriod)
readChunkSize=int(sr*predictionRate)

# initialize portaudio
duration=10
p = pyaudio.PyAudio()
stream = p.open(format=pyaudio.paInt16, channels=1, rate=sr, input=True, frames_per_buffer=CHUNKSIZE)

frames=[]
last5secFrames=[]
old5secFrames=[]

# for 10 seconds
for i in range(0, int(1 / (predictionRate)) * duration):
    # read new data and update last 5 sec frames
    old=time.time()
    data = stream.read(readChunkSize)
    print('read time: ', time.time()-old)

    frames+=[data]

    waveform = np.frombuffer(b''.join(frames[-int(predictionPeriod/predictionRate):]), dtype=np.int16) / 32768.0
    
    old=time.time()
    scores, spectrogram = yamnet.predict(np.reshape(waveform, [1, -1]), steps=1)
    print('prediction time: ', time.time()-old)
    old=time.time()
    # Plot and label the model output scores for the top-scoring classes.
    mean_scores = np.mean(scores, axis=0)
    top_N = 10
    top_class_indices = np.argsort(mean_scores)[::-1][:top_N]

    zipped=np.array(list(zip(class_names[top_class_indices], mean_scores[top_class_indices])))
    print(zipped)
    
    print('prediction: ',class_names[top_class_indices])
    print('etc time: ', time.time()-old)

print(len(frames))

# close stream
stream.stop_stream()
stream.close()
p.terminate()
