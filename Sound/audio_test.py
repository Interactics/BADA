#!/usr/bin/env python3
"""Plot the live microphone signal(s) with matplotlib.
Matplotlib and NumPy have to be installed.
"""
import queue
import sys

import numpy as np
import sounddevice as sd


def int_or_str(text):
    """Helper function for argument parsing."""
    try:
        return int(text)
    except ValueError:
        return text

samplerate=None
channels=[1]
device=None
window=200

q = queue.Queue()
mapping = [c - 1 for c in channels]  # Channel numbers start with 1

def audio_callback(indata, frames, time, status):
    """This is called (from a separate thread) for each audio block."""
    if status:
        print(status, file=sys.stderr)
    # Fancy indexing with mapping creates a (necessary!) copy:
    q.put(indata[::10, mapping])
    print('puttin')

try:

    if samplerate is None:
        device_info = sd.query_devices(device, 'input')
        samplerate = device_info['default_samplerate']

    length = int(window * samplerate / (1000 * 10))
    plotdata = np.zeros((length, len(channels)))

    stream = sd.InputStream(
        device=device, channels=max(channels),
        samplerate=samplerate, callback=audio_callback)

    i=0
    with stream:
        print(stream)
        pass

except Exception as e:
    print(e)