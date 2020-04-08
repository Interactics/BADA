import pyaudio
import numpy as np
from matplotlib import pyplot as plt
# Imports.
import soundfile as sf

import params
import yamnet as yamnet_model

import time

# Set up the YAMNet model.
params.PATCH_HOP_SECONDS = 0.48  # 10 Hz scores frame rate. //0.1
yamnet = yamnet_model.yamnet_frames_model(params)
yamnet.load_weights('yamnet.h5')
class_names = yamnet_model.class_names('yamnet_class_map.csv')


CHUNKSIZE = 8000 # fixed chunk size
sr=16000
seconds=1
predictionPeriod=0.5
predictionRate=0.25
predChunkSize=int(sr*predictionPeriod)
readChunkSize=int(sr*predictionRate)
# initialize portaudio
duration=10
p = pyaudio.PyAudio()
stream = p.open(format=pyaudio.paInt16, channels=1, rate=sr, input=True, frames_per_buffer=CHUNKSIZE)

frames=[]
last5secFrames=[]
old5secFrames=[]

# do this as long as you want fresh samples
# Store data in chunks for 3 seconds

# for 10 seconds
for i in range(0, int(sr / (CHUNKSIZE) * duration)):
    # read new data and update last 5 sec frames
    data = stream.read(CHUNKSIZE)
    old5secFrames=last5secFrames
    last5secFrames=[data]
    frames+=data

    #frames.append(data)
    print(len(old5secFrames+ last5secFrames))
    waveform = np.frombuffer(b''.join(old5secFrames+ last5secFrames), dtype=np.int16) / 32768.0
    print(np.mean(waveform))
    old=time.time()
    scores, spectrogram = yamnet.predict(np.reshape(waveform, [1, -1]), steps=1)
    print('prediction time: ', time.time()-old)
    # Plot and label the model output scores for the top-scoring classes.
    mean_scores = np.mean(scores, axis=0)
    top_N = 10
    top_class_indices = np.argsort(mean_scores)[::-1][:top_N]

    print('prediction: ',class_names[top_class_indices])



print(len(frames))

#numpydata=numpydata.T
# # plot data
# plt.plot(numpydata)
# plt.show()

# close stream
stream.stop_stream()
stream.close()
p.terminate()
