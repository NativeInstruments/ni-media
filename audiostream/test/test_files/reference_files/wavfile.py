#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ---------------------------------------------------------------------------------------------------------
# (c) Copyright André Bergner, 2012-2015
# This file is free software and may be  altered, distributed and used in commercial products without fees.
# However, this copyright notice may not be removed!
# ---------------------------------------------------------------------------------------------------------

import numpy as np


def loadwav(filename, N_frames2read=0):

    import wave

    if filename.__class__ != ''.__class__:
        raise TypeError(
            "wrong argument type! expecting a string containing the file name")

    wf = wave.open(filename)

    N_channels = wf.getnchannels()
    N_frames = wf.getnframes()

    if N_frames2read > 0:
        N_frames = min(N_frames, N_frames2read)

    bytestring = wf.readframes(N_frames)

    if wf.getsampwidth() == 1:                                 # 8bit samples
        data = np.fromstring(bytestring, np.int8)
        norm = 1. / 2**7

    elif wf.getsampwidth() == 2:                               # 16bit samples
        data = np.fromstring(bytestring, np.int16)
        norm = 1. / 2**15

    elif wf.getsampwidth() == 4:                               # 32bit samples
        data = np.fromstring(bytestring, np.int32)
        norm = 1.

    else:
        raise Exception("unsupported sample width: " + wf.getsampwidth())

    return np.reshape(data, [N_frames, N_channels]) * norm


def savewav(filename, data, samplerate, format):

    import wave

#   TODO:  something similar like below ....
#   if filename.__class__ != ''.__class__:
#      raise TypeError("wrong argument type! expecting a string containing the file name")

    data = np.array(data)       # convert to numpy array

    if len(data.shape) > 2:
        raise Exception(
            "data array has more than 2 dimensions. Don't know what to do!")

    if len(data.shape) == 1:
        N_frames = len(data)
        N_channels = 1

    else:
        N_frames = data.shape[1]    # here a check is needed if we maybe...
        N_channels = data.shape[0]    # ...have a row vector        TODO

    wf = wave.open(filename, 'wb')

    wf.setnframes(N_frames)
    wf.setnchannels(N_channels)
    wf.setframerate(samplerate)
    wf.setcomptype('NONE', '')
    wf.setsampwidth(format.itemsize)

    amplitude = 2**(format.itemsize * 8 - 1) - 1

    if format.itemsize == 1:  # 8 bit WAV needs to be written as unsigned data
        wf.writeframes(np.array(amplitude * data + amplitude + 1, format).T.tostring())
    else:
        wf.writeframes(np.array(amplitude * data, format).T.tostring())

    wf.close()


def wav2txt(wav_filename, overwrite=False):
    """
    Converts wave files into text files.

    The file extension of the resulting text file will be '.txt'

    Parameters
    ----------
    wav_filename : file name of the wave file
    overwrite    : default is 'False'. If set to 'True' already existing
                   text files will be overwritten.
    """

    if wav_filename.__class__ != ''.__class__:
        raise TypeError(
            "Wrong argument type! expecting a string containing the file name")

    import os

    txt_filename = os.path.splitext(wav_filename)[0] + '.txt'
    if os.path.exists(txt_filename) and not overwrite:
        print("There already exists a file with name '", txt_filename, "'!")
        print("Use option 'overwrite=True' to enfore overwriting!")
        return

    np.savetxt(txt_filename, loadwav(wav_filename), fmt="%1.6f")
    print("saved to file '", txt_filename, "'")


def txt2wav(txt_filename, overwrite=False):
    """
    Converts text files into wave files.

    The file extension of the resulting text file will be '.wav'

    Parameters
    ----------
    wav_filename : file name of the text file
    overwrite    : default is 'False'. If set to 'True' already existing
                   wave files will be overwritten.
    """

    if txt_filename.__class__ != ''.__class__:
        raise TypeError(
            "Wrong argument type! expecting a string containing the file name")

    import os

    wav_filename = os.path.splitext(txt_filename)[0] + '.wav'
    if os.path.exists(wav_filename) and not overwrite:
        print("There already exists a file with name '", wav_filename, "'!")
        print("Use option 'overwrite=True' to enfore overwriting!")
        return

    savewav(wav_filename, np.loadtxt(txt_filename))
    print("saved to file '", wav_filename, "'")


"""
class WaveFile ( np.ndarray ):      # should be esstentially an numpy array


   # ------ CONSTRUCTOR -----------------

   def  __init__ ( self , arg ):

      if arg.__class__ == ''.__class__:      # --- case string -> file name

         filename = arg

         try:
            self.wavfile = wave.open( filename )

         except IOError:
            print "ERROR: could not open file", filename
            return

         self.signal = signal
         self.sr = 44100
         #...

      elif arg.__class__ == [].__class__  \
        or arg.__class__ == numpy.ndarray:   # --- case numpy.array

         pass

      else:
         print "ERROR: unknown type", arg.__class__, "of passed variable!"

"""


def usage():
    print("""
______________________________________________________________
Usage:  wavfile.py  <filename>

<filename> can be either a txt-file or a wav-file.
The file will be converted in the other format, respectively.

Options:
   -t     force overwrite of files.

Examples:
   wavfile  sound.txt
""")


def main():

    import os
    import sys
    import getopt as go

    try:
        opts, args = go.getopt(sys.argv[1:], "hq:w:", ["help"])

    except go.GetoptError as err:
        # print help information and exit:
        print(str(err))  # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    if len(args) == 0:
        print("\n    ERROR: No file specified!")
        usage()
        sys.exit(1)

    filename = args[0]
    if not os.path.exists(filename):
        print("\n    ERROR '", filename, "' does not exist.\n")
        return

    overwrite = '-t' in args and True or False

    file_ext = os.path.splitext(filename)[1]

    if file_ext == '.txt':
        print("converting '", filename, "' into a wav file...")
        txt2wav(filename, overwrite)

    elif file_ext == '.wav':
        print("converting '", filename, "' into a txt file...")
        wav2txt(filename, overwrite)

    else:
        print("\n    ERROR: unrecognised file extension:'", file_ext, "'\n")
        return


if __name__ == "__main__":
    main()
