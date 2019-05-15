#!/usr/bin/env python3

import os
import sys
import subprocess
import platform
import chunk
import struct

#############################################################################
#                                HELPER
#############################################################################

def generate(generator, outfile, sample_rate, num_channels, num_frames, format ):
    subprocess.check_call(
        ['./generator', '-g', generator, '-r', str(sample_rate), '-c', str(num_channels), '-l', str(num_frames), '-f', format, '-o', outfile]
   )

def sndfile_convert(infile, outfile, format):
    subprocess.check_call(
        ['sndfile-convert', infile, outfile]
   )

def convert_to_aiff(infile, outfile, format ):
    subprocess.check_call(
        ['afconvert', '-f', 'AIFF' if format.is_signed_integer() else 'AIFC', '-d', format.to_afconvertformat(), infile, outfile]
    )

def convert_to_mp3(infile, outfile, bitrate):
    subprocess.check_call(
        ['lame', '-h', '-b', str(bitrate), infile, outfile]
    )

def convert_to_mp4_aac(infile, outfile, bitrate):
    subprocess.check_call(
        ['afconvert', '-f', 'm4af', '-d', 'aac', '-b', str(bitrate * 1000), infile, outfile]
    )


def convert_to_mp4_alac(infile, outfile):
    subprocess.check_call(
        ['afconvert', '-f', 'm4af', '-d', 'alac', infile, outfile]
    )


def convert_to_ogg(infile, outfile):
    devnull = open(os.devnull, 'w')
    subprocess.check_call(
        ['ffmpeg', '-y', '-i', infile, '-qscale:a', '10', '-acodec', 'libvorbis', outfile],
        stdout=devnull,
        stderr=devnull
    )


def check_afconvert():
    if platform.system() != 'Darwin':
        print('afconvert not available on this system (must be on OSX)')
        print('skipping generation of AIFF, ALAC and AAC files')
        return False
    else:
        return True

def check_ffmpeg():
    try:
        devnull = open(os.devnull, 'w')
        subprocess.call('ffmpeg', stdout=devnull, stderr=devnull)
    except FileNotFoundError:
        raise Exception('ffmpeg is not installed! To install run: brew install ffmpeg')

def check_lame():
    try:
        devnull = open(os.devnull, 'w')
        subprocess.call('lame', stdout=devnull, stderr=devnull)
    except FileNotFoundError:
        raise Exception('lame is not installed! To install run: brew install lame')

def check_libsndfile():
    try:
        devnull = open(os.devnull, 'w')
        subprocess.call('sndfile-convert', stdout=devnull, stderr=devnull)
    except FileNotFoundError:
        raise Exception('libsndfile is not installed! To install run: brew install libsndfile')



class PcmFormat:
    def __init__(self, typeprefix, numbits, endian):
        self.typeprefix = typeprefix
        self.numbits = numbits
        self.endian = endian

    def to_string(self):
        return self.typeprefix + str(self.numbits) + self.endian

    def is_native_endian(self):
        return self.endian == 'ne'

    def is_little_endian(self):
        return self.endian == 'le'

    def is_big_endian(self):
        return self.endian == 'be'

    def is_floating_point(self):
        return self.typeprefix == 'f'

    def is_signed_integer(self):
        return self.typeprefix == 's'

    def is_unsigned_integer(self):
        return self.typeprefix == 'u'


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


    def to_little_endian_format(self):
        return PcmFormat(self.typeprefix, self.numbits, 'le')

    def to_big_endian_format(self):
        return PcmFormat(self.typeprefix, self.numbits, 'be')

    def to_native_endian_format(self):
        return PcmFormat(self.typeprefix, self.numbits, 'be')

    def to_signed_format(self):
        return PcmFormat('s' if self.typeprefix == 'u' else self.typeprefix, self.numbits, self.endian)



def convert_to_file_with_gaps_between_chunks(src_file, gap_size, bigendian):
    name, ext = os.path.splitext(src_file)
    dst_file = name + '.gap' + ext
    int_format = '>L' if bigendian else '<L'
    with open(src_file, 'rb') as fdin:
        with open(dst_file, 'wb') as fdout:
            riff_or_form_chunk = chunk.Chunk(fdin, bigendian=bigendian)
            if riff_or_form_chunk.getname() == b'RIFF':
                format = riff_or_form_chunk.read(4)
                if format != b'WAVE':
                    raise Exception('not a WAVE file')
            elif riff_or_form_chunk.getname() == b'FORM':
                format = riff_or_form_chunk.read(4)
                if format not in (b'AIFF', b'AIFC'):
                    raise Exception('not an AIFF file')
            else:
                raise Exception('file does not start with RIFF or FORM id')

            fdout.write(riff_or_form_chunk.getname())
            fdout.write(b'xxxx') # will fill this in at the end
            fdout.write(format)

            while True:
                try:
                    subchunk = chunk.Chunk(riff_or_form_chunk, bigendian=bigendian)
                except EOFError:
                    break
                chunkname = subchunk.getname()
                gap_size = 0 if chunkname in (b'data', b'SSND') else gap_size
                fdout.write(chunkname)
                fdout.write(struct.pack(int_format, subchunk.getsize() + gap_size))
                fdout.write(subchunk.read())
                for i in range(gap_size + (gap_size % 2)):
                    fdout.write(b'0')
                subchunk.close()

            riff_or_form_chunk_size = fdout.tell() - 8
            fdout.seek(4)
            fdout.write(struct.pack(int_format, riff_or_form_chunk_size))



#############################################################################
#                          GENERATE TEST DATA
#############################################################################

if __name__ == '__main__':

    check_ffmpeg()
    check_afconvert()
    check_lame()
    check_libsndfile()


    generators = ['sin440', 'sin_mod440']
    channelconfigs = [1, 2]
    samplerates = [44100, 48000, 96000] # Hz 
    bitrates = [192]  # kbps  
    formats = [PcmFormat('u', 8, 'le'), PcmFormat('s', 16, 'le'), PcmFormat('s', 24, 'le'), PcmFormat('s', 32, 'le'), PcmFormat('f', 32, 'le'), PcmFormat('f', 64, 'le') ]
    gap_size = 1

    for generator in generators:
        for numchannels in channelconfigs:
            for samplerate in samplerates:
                for format in formats:

                    # file name is composed of: generator.numChannels.samplerate.numFrames.pcmFormat
                    name = generator + '.' + str(numchannels) + '.' + str(samplerate) + '.' + str(samplerate) + '.'
                    inputfilename = name + format.to_string() + '.wav'

                    # wav
                    generate(generator, inputfilename, samplerate, numchannels, samplerate, format.to_string()  )

                    # aiff
                    aiff_format = format.to_signed_format().to_big_endian_format()
                    aiff_filename = name + aiff_format.to_string() + '.aiff'
                    convert_to_aiff( inputfilename, aiff_filename, aiff_format )

                    if generator == generators[0] and format == formats[0]:
                        convert_to_file_with_gaps_between_chunks(inputfilename, gap_size, bigendian=False)
                        convert_to_file_with_gaps_between_chunks(aiff_filename, gap_size, bigendian=True)
                        gap_size += 3

                    if format.is_signed_integer():
                        # flac
                        if format.numbits >= 16 and format.numbits <= 24 :
                            sndfile_convert(inputfilename, name + format.to_string() + '.flac', format )

                        # alac
                        if format.numbits >= 16:
                            convert_to_mp4_alac(inputfilename, name + format.to_string() + '.alac.m4a')

                        # 16 bit WAV files are used for encoding of compressed formats
                        if format.numbits == 16:
                            if samplerate <= 48000:
                                for bitrate in bitrates:       
                                    convert_to_mp3(inputfilename, name + format.to_string() + '.cbr' + str(bitrate) + '.mp3', bitrate)
                                    convert_to_mp4_aac(inputfilename, name + format.to_string() + '.aac' + str(bitrate) + '.m4a', bitrate)

                            # ogg
                            convert_to_ogg(inputfilename, name + format.to_string() + '.ogg')


