# ExploreDSP
Exploring Digital Signal Processing 

## Systems Requirements
GCC / Linux 

## File Structure
The modules folder contains:
1. fft.h/fft.c - Radix-2 FFT algorithm
2. filters.h/filters.c - Different comb filters and a couple reverberation algorithms
3. dataSim.h/dataSim.c - Simulates waveforms
4. wav.h/wav.c - Reads and writes from/to WAV format

The applications folder contains:
1. const-audio-waves - creating basic WAV audio files from simulated waveforms
2. fft-demo - demonstration of the FFT algorithm with intentions of being educational
3. reverberation - user friendly terminal app that can add a variety of effects 
to a user-inputed WAV file and, in the case of adding reverb, an impulse WAV file

## Summary
The two main things I took a look at are: 

1. Fourier Transforms (DFT, FFT, IFT)
2. Audio Filters: mainly reverberation (Shroeder and Slow/Fast Convolutions)

## FFT
I placed a lot of time, after writing the FFT (Radix-2) algorithm, into
understanding the fourier transform, discrete fourier transform, fast fourier 
transform algorithm, and the inverse fourier transform, with intentions to
develop a demo script and README that could serve as a reference for anyone 
else trying to understand. This is currently on the back-burner, since there
are a couple key characteristics of the FFT/IFFT I do not fully understand.

## Important Note 
This is in regard to both the modules/wav.h and modules/wav.c files.

I did not write the wav module for reading and writing .wav files, but
tweaked it to allow me to read and store wav data. The reason I didn't write 
my own is simply because I liked this one much better than what I had written. 
While the website I downloaded it from no longer exists, modules/wav.h has 
more information on who wrote it. 

## Left To Do
1. Clean audio\_processor.c by placing filter main functions into the filters
module and all WAV IO into the WAV module.
2. Move all files other than audio\_processor.c in the reverberation dir to
an archived sub-dir.
3. Rename audio\_processor.c
4. Optimization for FFT multiple calls of the same data size
