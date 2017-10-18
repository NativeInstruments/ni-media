#!/usr/bin/env python

import os
import sys
from wavfile import savewav
import numpy as np
import subprocess
import platform

#############################################################################
#                                HELPER
#############################################################################


def convert_to_aiff(infile, outfile, format):
    subprocess.call(
        ['afconvert', '-f', 'AIFF', '-d', format, infile, outfile]
    )

def convert_to_mp3(infile, outfile, bitrate):
    subprocess.call(
        ['lame', '-h', '-b', str(bitrate), infile, outfile]
    )

def convert_to_mp4_aac(infile, outfile, bitrate):
    subprocess.call(
        ['afconvert', '-f', 'm4af', '-d', 'aac', '-b', str(bitrate * 1000), infile, outfile]
    )


def convert_to_mp4_alac(infile, outfile):
    subprocess.call(
        ['afconvert', '-f', 'm4af', '-d', 'alac', infile, outfile]
    )


def convert_to_ogg(infile, outfile):
    devnull = open(os.devnull, 'w')
    subprocess.call(
        ['ffmpeg', '-y', '-i', infile, '-qscale:a', '10', '-acodec', 'libvorbis', outfile],
        stdout=devnull,
        stderr=devnull
    )


def can_use_afconvert():
    if platform.system() != 'Darwin':
        print('afconvert not available on this system (must be on OSX)')
        print('skipping generation of AIFF, ALAC and AAC files')
        return False
    else:
        return True

def can_use_ffmpeg():
    try:
        devnull = open(os.devnull, 'w')
        subprocess.call('ffmpeg', stdout=devnull, stderr=devnull)
        return True
    except FileNotFoundError:
        print('ffmpeg is not installed!')
        print('run: brew install ffmpeg --with-libvorbis')
        print('skipping generation of OGG files')
        return False

def can_use_lame():
    try:
        devnull = open(os.devnull, 'w')
        subprocess.call('lame', stdout=devnull, stderr=devnull)
        return True
    except FileNotFoundError:
        print('lame is not installed!')
        print('run: brew install lame')
        print('skipping generation of MP3 files')
        return False



class PcmFormat:
    def __init__(self, typeprefix, numbits, endian):
        self.typeprefix = typeprefix
        self.numbits = numbits
        self.endian = endian

    def to_string(self):
        return self.typeprefix + str(self.numbits) + self.endian

    def to_dtype(self):
        endianstring = '='
        if self.endian == 'le':
            endianstring = '<'
        elif self.endian == 'be':
            endianstring = '>'

        typeprefixstring = self.typeprefix
        if self.typeprefix == 's':
            typeprefixstring = 'i'

        return np.dtype(endianstring + typeprefixstring + str(int(self.numbits / 8)))

    def to_afconvertformat(self):
        typeprefixstring = ''
        if self.typeprefix == 'u':
            typeprefixstring = 'UI'
        elif self.typeprefix == 's':
            typeprefixstring = 'I'
        elif self.typeprefix == 'f':
            typeprefixstring = 'F'
        else:
            raise Exception("unsupported type prefix: " + self.typeprefix)

        return self.endian.upper() + typeprefixstring + str(self.numbits)


def wavformat_to_aiffformat(wavformat):
    aiffformat = PcmFormat(wavformat.typeprefix, wavformat.numbits, wavformat.endian)
    if wavformat.typeprefix == 'u':
        aiffformat.typeprefix = 's'
    aiffformat.endian = 'be'
    return aiffformat


class SinGenerator:
    def __init__(self, freqHz):
        self.freqHz = freqHz
        self.name = 'sin' + str(self.freqHz)

    def data(self, numchannels, samplerate):
        ts = np.arange(samplerate)
        freq = self.freqHz * 2 * np.pi / samplerate
        if numchannels == 1:
            return np.sin(freq * ts)
        elif numchannels == 2:
            return [np.sin(freq * ts), np.cos(freq * ts)]
        else:
            raise Exception("unsupported number of channels: " + numchannels)


class ModulatedSinGenerator:
    def __init__(self, freqHz):
        self.freqHz = freqHz
        self.name = 'sin_mod' + str(self.freqHz)

    def data(self, numchannels, samplerate):
        ts = np.arange(samplerate)
        freq = self.freqHz * 2 * np.pi / samplerate
        mod_freq = 23 * 2 * np.pi / samplerate
        mod_amp = 16
        mod_left = np.sin(freq * ts + mod_amp * np.sin(mod_freq * ts))
        mod_right = np.cos(freq * ts + mod_amp * np.sin(mod_freq * ts))
        if numchannels == 1:
            return mod_left
        elif numchannels == 2:
            return [mod_left, mod_right]
        else:
            raise Exception("unsupported number of channels: " + numchannels)


#############################################################################
#                          GENERATE TEST DATA
#############################################################################

if __name__ == '__main__':

    can_use_lame = can_use_lame()
    can_use_afconvert = can_use_afconvert()
    can_use_ffmpeg = can_use_ffmpeg()

    generators = [SinGenerator(440), ModulatedSinGenerator(440)]
    channelconfigs = [1, 2]
    samplerates = [44100, 48000, 96000] # Hz 
    bitrates = [192]  # kbps  
    formats = [PcmFormat('u', 8, 'le'), PcmFormat('s', 16, 'le'), PcmFormat('s', 32, 'le')]

    for generator in generators:
        for numchannels in channelconfigs:
            for samplerate in samplerates:
                for format in formats:

                    # file name is composed of: generator.numChannels.samplerate.numFrames.pcmFormat
                    name = generator.name + '.' + str(numchannels) + '.' + str(samplerate) + '.' + str(samplerate) + '.'
                    wavfilename = name + format.to_string() + '.wav'
                    savewav(wavfilename, generator.data(numchannels, samplerate), samplerate, format.to_dtype())

                    if can_use_afconvert:
                        aiffformat = wavformat_to_aiffformat(format)
                        convert_to_aiff(wavfilename, name + aiffformat.to_string() + '.aiff', aiffformat.to_afconvertformat())

                    # 16 bit WAV files are used for encoding of compressed formats
                    if format.numbits == 16:
                        if samplerate <= 48000:
                            for bitrate in bitrates:       
                                if can_use_lame:
                                    convert_to_mp3(wavfilename, name + 'cbr' + str(bitrate) + '.mp3', bitrate)
                                if can_use_afconvert:
                                    convert_to_mp4_aac(wavfilename, name + 'aac' + str(bitrate) + '.m4a', bitrate)

                        if can_use_afconvert:
                            convert_to_mp4_alac(wavfilename, name + 'alac.m4a')

                        if can_use_ffmpeg:
                            convert_to_ogg(wavfilename, name + 'ogg')
