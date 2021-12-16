/* 
	The goal is to successfully take audio in a .wav file I made and apply a 
	reverberation effect using the FFT I built. This is all I want for 
	Christmas - let the 12 days commense. 

	Currently, I have made the audio and saved it to a .wav file (see the 
	const-audio-waves folder if you are interested). I also have built the 
	FFT and the IFFT (see the fft-demo folder if you are interested). 

	*Update - I decided to use a beat that I made in Premiere of all things. 
	Sine waves are boring and this beat is not. Simply put, the effects that 
	I build will be cooler when applied to a more dynamic/interesting track.

	All that is left for me to do is figure out how to use the FFT for 
	convolution reverbs...
		
		Realistically, I am going to need to add a bunch of functionality to
		the filter module. Right now it looks like a delay line and comb 
		is what's in store for me. Shouldn't take too long, as long as I don't
		waste my time writing things that are ultimately going to be deleted.

	*Another Update - this is not entirely true. I realized there is distortion
	when I simply read data from the .wav file and write the exact same data
	back to a new .wav file. Currently trying to fix that...	

	I am close, but am still experiencing small amounts of distortion.
	
	If after reading all of this, you still want to build and risk damaging
	both your hearing and your headphones, be my guest:

		Build Command: gcc comb_filter_harmonic.c ../../modules/filters.c ../../modules/wav.c -lm	
		Run Command: ./a.out
*/

#include <stdlib.h>

#include "../../modules/wav.h"
#include "../../modules/fft.h"
#include "../../modules/filters.h"

void find_max_int(struct wav_info* w, int* max_int);
/**/
void normalize_comb(float* value, float b0, float bm);
/**/

void main() {

	char *input_file = "data/input_signals/DeChaka_Instrumental.wav";	
	struct wav_info input_info;
    FILE* input = fopen(input_file,"rb");

	read_wav_info(&input_info, input);

	printf("\n");
	printf("Input:\n");
    print_wav_info(&input_info);
	printf("\n");

	int16_t *input_idata;
	input_idata = (int16_t *)malloc(sizeof(int16_t) * (input_info.num_samples*input_info.num_channels));

	uint32_t max_input_int;	// used to convert wav data to float

	find_max_int(&input_info, &max_input_int);
	read_wav_data(&input_info, input_idata, input); 

    fclose(input);

	float *input_fdata, *output_fdata;
	input_fdata = (float *)malloc(sizeof(float)* (input_info.num_samples*input_info.num_channels));

	size_t	count = 0, count2 = 0;	// for printf debugging

	// Channel 1 is the first half, Channel 2 is the second half
	for(int i=0; i<input_info.num_samples*input_info.num_channels; i++)
	{
		input_fdata[i] = ((float)input_idata[i]/(float)max_input_int);

		/*
		//if (i>(input_info.num_samples*input_info.num_channels) - 10000)
		if ((i<(input_info.num_samples*input_info.num_channels)/2) && (i>((input_info.num_samples*input_info.num_channels)/2) - 10000))
		{
			printf("%f\n", input_fdata[i]);
		}
		*/

		/*
		if (i < input_info.num_samples && count <= 1000) 
		{
			if (count == 0)
			{
				printf("CHANNEL 1\n");
			}

			printf("%f\n", input_fdata[i]);
			count += 1;
		}
		else if (i > input_info.num_samples && count2 <= 1000) 
		{
		if (count2 == 0)	
			{
				printf("CHANNEL 2\n");
			}

			printf("%f\n", input_fdata[i]);
			count2 +=1;
		}
		*/
	}

	ff_comb_filter* FFCF;
	ff_comb_filter* FFCF2;

	size_t delay_length = 10500;
	
	float b0 = 1.0;
	float bm = 0.9;

	float output = 0.0;
	
	FFCF = init_ff_comb_filter(delay_length, b0, bm);

	struct wav_info output_info;

    output_info.num_channels = 2;
    output_info.bits_per_sample = 16;
    output_info.sample_rate = 48000;
    output_info.num_samples = input_info.num_samples;	//+delay_length;

	output_fdata = (float *)malloc(sizeof(float)*((output_info.num_samples*output_info.num_channels)));

	for(int i=0; i<output_info.num_samples; i++) {
		output_fdata[i] = 0.0;
	}

	for (int i=0; i<output_info.num_samples; ++i)
	{
		step_ff_comb_filter(FFCF, input_fdata[i], &output);

		/*
		if (output > 1.0)
		{
			output /= 2.0;
		}
		*/

		/*
		if (i < delay_length) 
		{
			printf("%f, ", output);
		}
		*/

		//normalize_comb(&output, FFCF->b0, FFCF->bm);

		output_fdata[i] = output;

		/*
		if (i < delay_length) 
		{
			printf("%f\n", output);
		}
		*/
	}

	printf("\n");

	delete_ff_comb_filter(FFCF);

	FFCF2 = init_ff_comb_filter(delay_length, b0, bm);

	for (int i=output_info.num_samples; i<(output_info.num_samples*output_info.num_channels); ++i)
	{
		step_ff_comb_filter(FFCF2, input_fdata[i], &output);

		/*
		if (output > 1.0)
		{
			output /= 2.0;
		}
		*/

		/*
		if (i < output_info.num_samples + delay_length) 
		{
			printf("%f, ", output);
		}
		*/

		//normalize_comb(&output, FFCF2->b0, FFCF2->bm);

		/*
		if (i < output_info.num_samples + delay_length) 
		{
			printf("%f\n", output);
		}
		*/

		output_fdata[i] = output;
	}

	for (int i=0; i<output_info.num_samples*output_info.num_channels; i++)
	{
		output_fdata[i] /= 2.0;
		/*
		if (output_fdata[i] > 1.0)
		{
			output_fdata[i] = 1.0;
		}
		*/
	}

	printf("\n");

	delete_ff_comb_filter(FFCF2);

	char* output_file = "data/FFCF_M10000_b01_bm09.wav";	// filename to write to
	printf("Preparing to write to %s:\n",output_file);
    print_wav_info(&output_info);
	printf("\n");

    FILE* output_ptr = fopen(output_file,"wb");
	if(!output_ptr) {
        fprintf(stderr,"Error opening %s for output!\n",output_file);
    }

	int max_output_int;
	find_max_int(&output_info, &max_output_int);

	float max_output_float = (float)max_output_int;
	printf("%f\n", max_output_float);

    write_wav_hdr(&output_info,output_ptr);

	int_fast16_t *sample = (int_fast16_t *)malloc(sizeof(int_fast16_t) * output_info.num_channels);
	float *fsample = (float *)malloc(sizeof(float) * output_info.num_channels);

	for(int i=0; i < output_info.num_samples; i++) {
		//fsample[0] = input_fdata[i];//*max_output_float;
		//fsample[1] = input_fdata[i+output_info.num_samples];//*max_output_float;

		if (output_fdata[i] > 1.0 || output_fdata[i] < -1.0)
		{
			printf("%f\n", output_fdata[i]);
		}

		sample[0] = (int_fast16_t)(output_fdata[i]*max_output_float);
		sample[1] = (int_fast16_t)(output_fdata[i+(output_info.num_samples)]*max_output_float);

		//sample[0] = (int_fast16_t)input_idata[i];
		//sample[1] = (int_fast16_t)input_idata[i+(output_info.num_samples)];

		//printf("%f\n", output_fdata[i]);
		//printf("%f, %f\n", fsample[0], fsample[1]);
		//printf("%d, %d\n", sample[0], sample[1]);

		/*
		if (i<10000) 
		{
			printf("%d, %d\n", (int_fast16_t)(output_ddata[i]*max_output_double), (int_fast16_t)(output_fdata[i]*max_output_float));
		}
		*/

		write_sample(&output_info, output_ptr, sample);
	}

	fclose(output_ptr);
	free(sample);
	
	// Make sure everything is okay
	FILE* fp = fopen(output_file,"rb");
    read_wav_info(&output_info,fp);
    printf("\nReading back info from %s header:\n",output_file);
    print_wav_info(&output_info);
    fclose(fp);

	printf("\n");

	/*
	for (int k=0; k<output_length; k++) {
		printf("%f \n", output[k]);
	}
	*/

	free(input_idata);
	free(input_fdata);
	free(output_fdata);
}

void find_max_int(struct wav_info* w, int* max_int) {
/* determine maximum integer expressable in w.bits_per_sample bits, assuming
twos complement encoding of signed integers */

    switch(w->bits_per_sample) {
    case 8:
      *max_int=0x7f;		/* 127 */
      break;
    case 16:
	  //0x10000;
      *max_int=0x7fff;		/* 32767 */
      break;
    case 24:
      *max_int=0x7fffff;	/* 8388607 */
      break;
    case 32:
      *max_int=0x7fffffff;	/* 2147483647 */
      break;
    default:
      printf("error: bits_per_sample must be 8,16,24, or 32.\n");
    }
}

void normalize_comb(float* value, float b0, float bm) {
	
	float max = b0 + bm;
	*value = *value / max;
}
