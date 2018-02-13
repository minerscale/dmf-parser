#include "dmf-parser.h"
#include "miniz.h"

int openFileIntoBuffer(char *filename, unsigned char *dest, size_t *length){
    FILE *fp = fopen(filename, "rb");

    // Get size of file
    fseek(fp, 0, SEEK_END);
    size_t lengthOfFile = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Read the file into a buffer
    if(fread(dest, lengthOfFile, 1, fp) == 0){
        fclose(fp);
        return 1; // Unable to open file
    }
    fclose(fp);
    *length = lengthOfFile;

    return 0;
}

int decompressDMF(unsigned char *src, size_t src_length, unsigned char *dest)
{
    // Max size of decompressed DMF is 16MiB
    // Shouldn't be a problem though
    mz_ulong dest_len = MAX_DMF_SIZE;

    // Decompress the dmf and store it in the dest
    int cmp_status = uncompress(dest, &dest_len, src, src_length);
    if (cmp_status) return 2; // Not valid DMF

    return 0; // Successful
}

int openDMF(char *filename, unsigned char *dest)
{
    // Decompress the dmf
    unsigned char *compressedBuffer = (unsigned char *)malloc(MAX_DMF_SIZE);
    size_t bufferLength;
    int status = openFileIntoBuffer(filename, compressedBuffer, &bufferLength);
    if(status) return status;

    status = decompressDMF(compressedBuffer, bufferLength, dest);
    if(status) return status;

    return 0;
}

int parseDMF(unsigned char *decompressed_dmf, dmf *dest)
{
    unsigned char *dmfp = decompressed_dmf; // Pointer to the current spot

    // Get the version and quit if it's invalid
    dmfp += 16;
    if (*dmfp != 24) return 3; // Invalid version

    // Get the system
    ++dmfp;
    dest->system = *dmfp;

    // Set the total channels based off system
    switch (dest->system){
        case SYSTEM_GENESIS:
            dest->system_total_channels = 10;
            break;
        case SYSTEM_GENESIS_EXT_CH3:
            dest->system_total_channels = 13;
            break;
        case SYSTEM_SMS:
            dest->system_total_channels = 4;
            break;
        case SYSTEM_GAMEBOY:
            dest->system_total_channels = 4;
            break;
        case SYSTEM_PCENGINE:
            dest->system_total_channels = 6;
            break;
        case SYSTEM_NES:
            dest->system_total_channels = 5;
            break;
        case SYSTEM_C64_SID_8580:
            dest->system_total_channels = 3;
            break;
        case SYSTEM_C64_SID_6581:
            dest->system_total_channels = 3;
            break;
        case SYSTEM_YM2151:
            dest->system_total_channels = 13;
            break;
        default:
            return 4; // Not a real system???
    }

    // Get the song name and the length
    ++dmfp;
    dest->name = (char *)calloc(*dmfp + 1, 1);
    memcpy (dest->name, dmfp + 1, *dmfp);
    dest->name_length = *dmfp;

    dmfp += *dmfp + 1;

    // Get the song author and the length
    dest->author = (char *)calloc(*dmfp + 1, 1);
    memcpy (dest->author, dmfp + 1, *dmfp);
    dest->author_length = *dmfp;

    dmfp += *dmfp + 1;

    // Get the highlights
    dest->highlight_A = *dmfp++;
    dest->highlight_B = *dmfp++;

    // Timing info
    dest->time_base = *dmfp++;
    dest->tick_time_1 = *dmfp++;
    dest->tick_time_2 = *dmfp++;

    // Frequency
    dest->reigon = *dmfp++;
    dest->custom_hz_on = *dmfp++;

    // Custom_hz in the file is stored in ASCII *why*
    dest->custom_hz = (*dmfp - 0x30 >= 0) ? (*dmfp - 0x30) * 100 : 0;
    ++dmfp;
    dest->custom_hz = (*dmfp - 0x30 >= 0) ? (dest->custom_hz + (*dmfp - 0x30) * 10) : dest->custom_hz / 10;
    ++dmfp;
    dest->custom_hz = (*dmfp - 0x30 >= 0) ? (dest->custom_hz + (*dmfp - 0x30)) : dest->custom_hz / 10;
    ++dmfp;

    // Pattern matrix stuff
    dest->total_rows_per_pattern = *dmfp;
    dmfp += 4;
    dest->total_rows_in_pattern_matrix = *dmfp++;

    // Create the instrument array, It's a dynamically allocated 2d array
    dest->pattern_matrix_value = (unsigned char **)malloc(dest->system_total_channels * sizeof(unsigned char *));

    for (int i = 0; i < dest->system_total_channels; i++){
        dest->pattern_matrix_value[i] = (unsigned char *)malloc(dest->total_rows_in_pattern_matrix * sizeof(unsigned char));
    }

    // Set the dmf data
    for (int i = 0; i < dest->system_total_channels; i++){
        for (int j = 0; j < dest->total_rows_in_pattern_matrix; j++){
            dest->pattern_matrix_value[i][j] = *dmfp++;
        }
    }

    // Instrument data
    dest->total_instruments = *dmfp++;

    dest->instruments = (instrument *)malloc(sizeof(instrument) * dest->total_instruments);
    for (int i = 0; i < dest->total_instruments; ++i){
        dest->instruments[i].name_length = *dmfp;
        // Create the name array
        dest->instruments[i].name = (unsigned char *)calloc(*dmfp + 1, 1);
        memcpy(dest->instruments[i].name, dmfp + 1, *dmfp);
        dmfp += *dmfp + 1;

        // Get the mode of the instrument (STD/FM)
        dest->instruments[i].mode = *dmfp++;

        if (dest->instruments[i].mode == MODE_FM){
            dest->instruments[i].ALG = *dmfp++;
            dest->instruments[i].FB = *dmfp++;
            dest->instruments[i].LFO = *dmfp++;
            dest->instruments[i].LFO2 = *dmfp++;

            // Allocate the FM operators memory
            dest->instruments[i].FM_operators = (FM_operator *)malloc(4 * sizeof(FM_operator));
            for (int j = 0; j < 4; j++){
                dest->instruments[i].FM_operators[j].AM = *dmfp++;
                dest->instruments[i].FM_operators[j].AR = *dmfp++;
                dest->instruments[i].FM_operators[j].DR = *dmfp++;
                dest->instruments[i].FM_operators[j].MULT = *dmfp++;
                dest->instruments[i].FM_operators[j].RR = *dmfp++;
                dest->instruments[i].FM_operators[j].SL = *dmfp++;
                dest->instruments[i].FM_operators[j].TL = *dmfp++;
                dest->instruments[i].FM_operators[j].DT2 = *dmfp++;
                dest->instruments[i].FM_operators[j].RS = *dmfp++;
                dest->instruments[i].FM_operators[j].DT = *dmfp++;
                dest->instruments[i].FM_operators[j].D2R = *dmfp++;
                dest->instruments[i].FM_operators[j].SSGMODE = *dmfp++;
            }
        }else if (dest->instruments[i].mode == MODE_STD){
            if (dest->system != SYSTEM_GAMEBOY){
                // Volume macro
                dest->instruments[i].volume_envelope_size = *dmfp++;

                dest->instruments[i].volume_envelope = (unsigned int *)malloc(dest->instruments[i].volume_envelope_size * sizeof(int));
                for (int k = 0; k < dest->instruments[i].volume_envelope_size; k++){
                    dest->instruments[i].volume_envelope[k] = *dmfp;
                    dmfp += 4;
                }

                if (dest->instruments[i].volume_envelope_size > 0){
                    dest->instruments[i].volume_loop_position = *dmfp++;
                }
            }

            // Arpeggio macro
            dest->instruments[i].arpeggio_envelope_size = *dmfp++;

            dest->instruments[i].arpeggio_envelope = (signed int *)malloc(dest->instruments[i].arpeggio_envelope_size * sizeof(int));
            for (int k = 0; k < dest->instruments[i].arpeggio_envelope_size; k++){
                dest->instruments[i].arpeggio_envelope[k] = *dmfp;
                dmfp += 4;
            }

            if (dest->instruments[i].arpeggio_envelope_size > 0){
                dest->instruments[i].arpeggio_loop_position = *dmfp++;
            }
            dest->instruments[i].arpeggio_macro_mode = *dmfp++;

            // Duty/Noise macro:
            dest->instruments[i].duty_noise_envelope_size = *dmfp++;

            dest->instruments[i].duty_noise_envelope = (unsigned int *)malloc(dest->instruments[i].duty_noise_envelope_size * sizeof(int));
            for (int k = 0; k < dest->instruments[i].duty_noise_envelope_size; k++){
                dest->instruments[i].duty_noise_envelope[k] = *dmfp;
                dmfp += 4;
            }

            if (dest->instruments[i].duty_noise_envelope_size > 0){
                dest->instruments[i].duty_noise_loop_position = *dmfp++;
            }

            // Wavetable macro:
            dest->instruments[i].wavetable_envelope_size = *dmfp++;

            dest->instruments[i].wavetable_envelope = (unsigned int *)malloc(dest->instruments[i].wavetable_envelope_size * sizeof(int));
            for (int k = 0; k < dest->instruments[i].wavetable_envelope_size; k++){
                dest->instruments[i].wavetable_envelope[k] = *dmfp;
                dmfp += 4;
            }

            if (dest->instruments[i].wavetable_envelope_size > 0){
                dest->instruments[i].wavetable_loop_position = *dmfp++;
            }

            // PER SYSTEM DATA
            if (dest->system == SYSTEM_C64_SID_8580 || dest->system == SYSTEM_C64_SID_6581){
                dest->instruments[i].c64_triangle_wave_enabled = *dmfp++;
                dest->instruments[i].c64_saw_wave_enabled = *dmfp++;
                dest->instruments[i].c64_pulse_wave_enabled = *dmfp++;
                dest->instruments[i].c64_noise_wave_enabled = *dmfp++;
                dest->instruments[i].c64_attack = *dmfp++;
                dest->instruments[i].c64_decay = *dmfp++;
                dest->instruments[i].c64_sustain = *dmfp++;
                dest->instruments[i].c64_release = *dmfp++;
                dest->instruments[i].c64_pulse_width = *dmfp++;
                dest->instruments[i].c64_ring_modulation_enabled = *dmfp++;
                dest->instruments[i].c64_sync_modulation_enabled = *dmfp++;
                dest->instruments[i].c64_to_filter = *dmfp++;
                dest->instruments[i].c64_volume_macro_to_filter_cutoff_enabled = *dmfp++;
                dest->instruments[i].c64_use_filter_values_from_instrument = *dmfp++;
                dest->instruments[i].c64_filter_resonance = *dmfp++;
                dest->instruments[i].c64_filter_cutoff = *dmfp++;
                dest->instruments[i].c64_filter_high_pass = *dmfp++;
                dest->instruments[i].c64_filter_band_pass = *dmfp++;
                dest->instruments[i].c64_filter_low_pass = *dmfp++;
                dest->instruments[i].c64_filter_ch2_off = *dmfp++;
            }else if (dest->system == SYSTEM_GAMEBOY){
                dest->instruments[i].gb_envelope_volume = *dmfp++;
                dest->instruments[i].gb_envelope_direction = *dmfp++;
                dest->instruments[i].gb_envelope_length = *dmfp++;
                dest->instruments[i].gb_sound_length = *dmfp++;
            }
        }else{
            return 2; // Not a valid dmf.
        }
    }

    // Wavetables
    dest->total_wavetables = *dmfp++;
    dest->wavetables = (wavetable *)malloc(dest->total_wavetables * sizeof(wavetable));
    for (int i = 0; i < dest->total_wavetables; ++i){
        dest->wavetables[i].size = *dmfp;
        dmfp += 4;
        dest->wavetables[i].data = (unsigned int *)malloc(dest->wavetables[i].size * sizeof(unsigned int));
        for (int j = 0; j < dest->wavetables[i].size; ++j){
            dest->wavetables[i].data[j] = *dmfp;
            dmfp += 4;
        }
    }

    // Notes!
    dest->channels = malloc(dest->system_total_channels * sizeof(note_channel));
    for (int i = 0; i < dest->system_total_channels; ++i){
        dest->channels[i].effect_columns_count = *dmfp++;

        // Allocate 2D array
        dest->channels[i].rows = (note_row **)malloc(dest->total_rows_in_pattern_matrix * sizeof(note_row *));
        for (int z = 0; z < dest->total_rows_in_pattern_matrix; z++){
            dest->channels[i].rows[z] = (note_row *)malloc(dest->total_rows_per_pattern * sizeof(note_row));
        }

        for (int j = 0; j < dest->total_rows_in_pattern_matrix; ++j){
            for (int k = 0; k < dest->total_rows_per_pattern; ++k){
                dest->channels[i].rows[j][k].note = *dmfp;
                dmfp += 2;
                dest->channels[i].rows[j][k].octave = *dmfp;
                dmfp += 2;
                dest->channels[i].rows[j][k].volume = *(signed short *)dmfp;
                dmfp += 2;

                dest->channels[i].rows[j][k].commands = (note_command *)malloc(dest->channels[i].effect_columns_count * sizeof(note_command));
                for (int m = 0; m < dest->channels[i].effect_columns_count; ++m){
                    dest->channels[i].rows[j][k].commands[m].code = *(signed short *)dmfp;
                    dmfp += 2;
                    dest->channels[i].rows[j][k].commands[m].value = *(signed short *)dmfp;
                    dmfp += 2;
                }
                dest->channels[i].rows[j][k].instrument = *(signed short *)dmfp;
                dmfp += 2;
            }
        }
    }

    dest->total_samples = *dmfp++;
    dest->samples = (sample *)malloc(dest->total_samples * sizeof(sample));
    for (int i = 0; i < dest->total_samples; ++i){
        dest->samples[i].size = *(unsigned int *)dmfp;
        dmfp += 4;

        dest->samples[i].name_length = *dmfp++;
        dest->samples[i].name = (unsigned char *)calloc(dest->samples[i].name_length + 1 * sizeof(unsigned char *), 1);
        memcpy (dest->samples[i].name, dmfp, dest->samples[i].name_length);
        dmfp += dest->samples[i].name_length;

        dest->samples[i].rate = *dmfp++;
        dest->samples[i].pitch = *dmfp++;
        dest->samples[i].amp = *dmfp++;
        dest->samples[i].bits = *dmfp++;

        dest->samples[i].data = (unsigned short *)malloc(dest->samples[i].size * sizeof(unsigned short));
        for (int j = 0; j < dest->samples[i].size; ++j){
            dest->samples[i].data[j] = *dmfp;
            dmfp += 2;
        }
    }

    return 0; // Successful
}

int fileToDmfType(char *filename, dmf *dest)
{
    unsigned char *buffer = (unsigned char *)malloc(MAX_DMF_SIZE);
    int status = openDMF(filename, buffer);
    if (status) return status;

    status = parseDMF(buffer, dest);
    if (status) return status;
    return 0;
}

int dmfToBuffer(dmf src, unsigned char *dest, size_t *size){
    unsigned char *bp = dest; // British Petrol
    // Mime type and version
    char *str = ".DelekDefleMask.\x18";
    memcpy(bp, str, strlen(str));
    bp += 17;

    // SYSTEM
    *bp = src.system;
    ++bp;

    // VISUAL INFORMATION
    *bp = src.name_length;
    ++bp;
    memcpy(bp, src.name, src.name_length);
    bp += src.name_length;

    *bp = src.author_length;
    ++bp;
    memcpy(bp, src.author, src.author_length);
    bp += src.author_length;

    *bp = src.highlight_A;
    ++bp;
    *bp = src.highlight_B;
    ++bp;

    // MODULE INFORMATION
    *bp = src.time_base;
    ++bp;
    *bp = src.tick_time_1;
    ++bp;
    *bp = src.tick_time_2;
    ++bp;
    *bp = src.reigon;
    ++bp;
    *bp = src.custom_hz_on;
    ++bp;

    // This is not convenient as now I can't bit-perfectly redo this :(
    // There's a check if there is nothing... Other than that, I'm fucked.
    // if custom hz is used sensibly this should work *most* of the time.
    if (src.custom_hz > 0){
        sprintf((char *)bp, "%d", src.custom_hz);
    }
    bp += 3;

    *(int *)bp = src.total_rows_per_pattern;
    bp += 4;

    *bp = src.total_rows_in_pattern_matrix;
    ++bp;

    // PATTERN MATRIX VALUES
    for (int i = 0; i < src.system_total_channels; ++i){
        for (int j = 0; j < src.total_rows_in_pattern_matrix; ++j){
            *bp = src.pattern_matrix_value[i][j];
            ++bp;
        }
    }

    *bp = src.total_instruments;
    ++bp;
    for (int i = 0; i < src.total_instruments; ++i){
        *bp = src.instruments[i].name_length;
        ++bp;

        memcpy(bp, src.instruments[i].name, (size_t)src.instruments[i].name_length);
        bp += src.instruments[i].name_length;

        *bp = src.instruments[i].mode;
        ++bp;

        // FM INSTRUMENTS DATA
        if (src.instruments[i].mode == MODE_FM){
            *bp = src.instruments[i].ALG;
            ++bp;
            *bp = src.instruments[i].FB;
            ++bp;
            *bp = src.instruments[i].LFO;
            ++bp;
            *bp = src.instruments[i].LFO2;
            ++bp;

            for (int j = 0; j < 4; j++){
                *bp = src.instruments[i].FM_operators[j].AM;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].AR;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].DR;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].MULT;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].RR;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].SL;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].TL;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].DT2;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].RS;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].DT;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].D2R;
                ++bp;
                *bp = src.instruments[i].FM_operators[j].SSGMODE;
                ++bp;
            }
        }else{
            if (src.system != SYSTEM_GAMEBOY){
                // Volume Macro
                *bp = src.instruments[i].volume_envelope_size;
                ++bp;
                for (int j = 0; j < src.instruments[i].volume_envelope_size; ++j){
                    *(int *)bp = src.instruments[i].volume_envelope[j];
                    bp += 4;
                }
                if (src.instruments[i].volume_envelope_size > 0){
                    *bp = src.instruments[i].volume_loop_position;
                    ++bp;
                }
            }
            // Arpeggio Macro
            *bp = src.instruments[i].arpeggio_envelope_size;
            ++bp;
            for (int j = 0; j < src.instruments[i].arpeggio_envelope_size; ++j){
                *(int *)bp = src.instruments[i].arpeggio_envelope[j];
                bp += 4;
            }
            if (src.instruments[i].arpeggio_envelope_size > 0){
                *bp = src.instruments[i].arpeggio_loop_position;
                ++bp;
            }
            *bp = src.instruments[i].arpeggio_macro_mode;
            ++bp;

            // Duty/Noise Macro
            *bp = src.instruments[i].duty_noise_envelope_size;
            ++bp;
            for (int j = 0; j < src.instruments[i].duty_noise_envelope_size; ++j){
                *(int *)bp = src.instruments[i].duty_noise_envelope[j];
                bp += 4;
            }
            if (src.instruments[i].duty_noise_envelope_size > 0){
                *bp = src.instruments[i].duty_noise_loop_position;
                ++bp;
            }

            // Wavetable Macro
            *bp = src.instruments[i].wavetable_envelope_size;
            ++bp;
            for (int j = 0; j < src.instruments[i].wavetable_envelope_size; ++j){
                *(int *)bp = src.instruments[i].wavetable_envelope[j];
                bp += 4;
            }
            if (src.instruments[i].wavetable_envelope_size > 0){
                *bp = src.instruments[i].wavetable_loop_position;
                ++bp;
            }

            // C64 stuff
            if (src.system == SYSTEM_C64_SID_6581 || src.system == SYSTEM_C64_SID_8580){
                *bp = src.instruments[i].c64_triangle_wave_enabled;
                ++bp;
                *bp = src.instruments[i].c64_saw_wave_enabled;
                ++bp;
                *bp = src.instruments[i].c64_pulse_wave_enabled;
                ++bp;
                *bp = src.instruments[i].c64_noise_wave_enabled;
                ++bp;
                *bp = src.instruments[i].c64_attack;
                ++bp;
                *bp = src.instruments[i].c64_decay;
                ++bp;
                *bp = src.instruments[i].c64_sustain;
                ++bp;
                *bp = src.instruments[i].c64_release;
                ++bp;
                *bp = src.instruments[i].c64_pulse_width;
                ++bp;
                *bp = src.instruments[i].c64_ring_modulation_enabled;
                ++bp;
                *bp = src.instruments[i].c64_sync_modulation_enabled;
                ++bp;
                *bp = src.instruments[i].c64_to_filter;
                ++bp;
                *bp = src.instruments[i].c64_volume_macro_to_filter_cutoff_enabled;
                ++bp;
                *bp = src.instruments[i].c64_use_filter_values_from_instrument;
                ++bp;
                // FILTER GLOBALS
                *bp = src.instruments[i].c64_filter_resonance;
                ++bp;
                *bp = src.instruments[i].c64_filter_cutoff;
                ++bp;
                *bp = src.instruments[i].c64_filter_high_pass;
                ++bp;
                *bp = src.instruments[i].c64_filter_band_pass;
                ++bp;
                *bp = src.instruments[i].c64_filter_low_pass;
                ++bp;
                *bp = src.instruments[i].c64_filter_ch2_off;
                ++bp;
            }else if (src.system == SYSTEM_GAMEBOY){
                *bp = src.instruments[i].gb_envelope_volume;
                ++bp;
                *bp = src.instruments[i].gb_envelope_direction;
                ++bp;
                *bp = src.instruments[i].gb_envelope_length;
                ++bp;
                *bp = src.instruments[i].gb_sound_length;
                ++bp;
            }
        }
    }

    *bp = src.total_wavetables;
    ++bp;
    for (int i = 0; i < src.total_wavetables; ++i){
        *(unsigned int *)bp = src.wavetables[i].size;
        bp += 4;
        for (int j = 0; j < src.wavetables[i].size; ++j){
            *(unsigned int *)bp = src.wavetables[i].data[j];
            bp += 4;
        }
    }

    for (int i = 0; i < src.system_total_channels; ++i){
        *bp = src.channels[i].effect_columns_count;
        ++bp;

        for (int j = 0; j < src.total_rows_in_pattern_matrix; ++j){
            for (int k = 0; k < src.total_rows_per_pattern; ++k){
                *(unsigned short *)bp = src.channels[i].rows[j][k].note;
                bp += 2;
                *(unsigned short *)bp = src.channels[i].rows[j][k].octave;
                bp += 2;
                *(signed short *)bp = src.channels[i].rows[j][k].volume;
                bp += 2;
                for (int l = 0; l < src.channels[i].effect_columns_count; ++l){
                    *(signed short *)bp = src.channels[i].rows[j][k].commands[l].code;
                    bp += 2;
                    *(signed short *)bp = src.channels[i].rows[j][k].commands[l].value;
                    bp += 2;
                }
                *(signed short *)bp = src.channels[i].rows[j][k].instrument;
                bp += 2;
            }
        }
    }

    *bp = src.total_samples;
    ++bp;

    for (int i = 0; i < src.total_samples; ++i){
        *(unsigned int *)bp = src.samples[i].size;
        bp += 4;
        *bp = src.samples[i].name_length;
        ++bp;
        memcpy(bp, src.samples[i].name, src.samples[i].name_length);
        bp += src.samples[i].name_length;

        *bp = src.samples[i].rate;
        ++bp;
        *bp = src.samples[i].pitch;
        ++bp;
        *bp = src.samples[i].amp;
        ++bp;
        *bp = src.samples[i].bits;
        ++bp;

        for (int j = 0; j < src.samples[i].size; ++j){
            *(unsigned short *)bp = src.samples[i].data[j];
            bp += 2;
        }
    }
    *size = bp - dest;

    return 0;
}

int compressDMF(const unsigned char *src, size_t src_length, unsigned char *dest, size_t *dest_length)
{
    // Decompress the dmf and store it in the dest
    int cmp_status = mz_compress (dest, (mz_ulong *)dest_length, src, src_length);
    if (cmp_status){
        return (cmp_status);
    }

    return 0; // Successful
}

int writeDMF(char *filename, dmf src)
{
    unsigned char *dest = (unsigned char *)calloc(MAX_DMF_SIZE, 1);

    size_t buffer_len;
    int status = dmfToBuffer(src, dest, &buffer_len);
    if (status) return status;

    unsigned char *comp_dest = malloc(MAX_DMF_SIZE);
    size_t comp_dest_length = MAX_DMF_SIZE;
    status = compressDMF(dest, buffer_len, comp_dest, &comp_dest_length);
    if (status) return status;
    

    FILE *fp = fopen(filename, "wb");
    fwrite(comp_dest, comp_dest_length, 1, fp);

    return 0;
}
