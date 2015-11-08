from scikits.audiolab import Sndfile, Format
import numpy as np

class AudioFile:
	def __init__(self, file_name):
		self.sf = Sndfile(file_name)

		self.file_format = self.sf.format
		self.nchans = self.sf.channels
		self.sr = self.sf.samplerate
		self.length = self.sf.nframes
		self.audio = self.sf.read_frames(self.length)

