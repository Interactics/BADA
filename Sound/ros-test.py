import pyaudio
import numpy as np
from matplotlib import pyplot as plt
# Imports.
import soundfile as sf

import params
import yamnet as yamnet_model

import time
import queue

import roslibpy
import json

client=roslibpy.Ros(host='localhost',port=9090)
client.run()

talker=roslibpy.Topic(client, '/audio', 'std_msgs/String')

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

# do this as long as you want fresh samples
# Store data in chunks for 3 seconds

# for 10 seconds
for i in range(0, int(1 / (predictionRate)) * duration):
    # read new data and update last 5 sec frames
    old=time.time()
    data = stream.read(readChunkSize)
    print('read time: ', time.time()-old)
    # old5secFrames=last5secFrames
    # last5secFrames=[data]
    frames+=[data]
    #if(len(frames)<predChunkSize): continue
    #print(b''.join(frames[-int(predictionPeriod/predictionRate):])[-5:])
    waveform = np.frombuffer(b''.join(frames[-int(predictionPeriod/predictionRate):]), dtype=np.int16) / 32768.0
    
    #print(len(frames[-int(predictionPeriod/predictionRate):]))
    #print(-int(predictionPeriod/predictionRate))
    #print(np.mean(waveform))
    #waveform= np.array(frames[-predChunkSize:], dtype=np.int16)/32768.0
    old=time.time()
    scores, spectrogram = yamnet.predict(np.reshape(waveform, [1, -1]), steps=1)
    print('prediction time: ', time.time()-old)
    old=time.time()
    # Plot and label the model output scores for the top-scoring classes.
    mean_scores = np.mean(scores, axis=0)
    top_N = 10
    top_class_indices = np.argsort(mean_scores)[::-1][:top_N]

    zipped=np.dstack((class_names[top_class_indices],mean_scores[top_class_indices])).tolist()
    #print(zipped)
    if(client.is_connected):
        talker.publish(roslibpy.Message({'data': json.dumps(zipped)}))
        print('sending msg')

    print('prediction: ',class_names[top_class_indices])
    print('etc time: ', time.time()-old)




print(len(frames))

#numpydata=numpydata.T
# # plot data
# plt.plot(numpydata)
# plt.show()

# close stream
stream.stop_stream()
stream.close()
p.terminate()

talker.unadvertise()
client.terminate()
