import numpy as np

from scipy.io import wavfile
from scipy.signal import hilbert

samplerate, data = wavfile.read('Wav26.wav')

wavFileInfo = open("wafeInfo.txt", "a")
wavFileInfo.write(str(samplerate)+'\n')
wavFileInfo.write(str(data.size)+'\n')
wavFileInfo.close()

print(samplerate)
print(data.size)
print (data)

np.savetxt("waveData.txt", data, fmt="%2.0f")

analytic_signal = hilbert(data)
amplitude_envelope = np.abs(analytic_signal)

np.savetxt("Envelope.txt", amplitude_envelope, fmt="%2.0f")
print(amplitude_envelope)




