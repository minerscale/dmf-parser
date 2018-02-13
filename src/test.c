#include <stdio.h>
#include <stdlib.h>
#include "libdmf.h"

void display_dmf(dmf song){
    printf("system: %d\n", song.system);
    printf("system_total_channels: %d\n", song.system_total_channels);
    
    printf("name_length: %d\n", song.name_length);
    printf("name: %s\n", song.name);
    
    printf("author_length: %d\n", song.author_length);
    printf("author: %s\n", song.author);
    
    printf("highlight_A: %d\n", song.highlight_A);
    printf("highlight_B: %d\n", song.highlight_B);
    
    printf("time_base: %d\n", song.time_base);
    printf("tick_time_1: %d\n", song.tick_time_1);
    printf("tick_time_2: %d\n", song.tick_time_2);
    
    printf("reigon: %d\n", song.reigon);
    printf("custom_hz_on: %d\n", song.custom_hz_on);
    printf("custom_hz: %d\n", song.custom_hz);
    
    printf("total_rows_per_pattern: %d\n", song.total_rows_per_pattern);
    printf("total_rows_in_pattern_matrix: %d\n\n", song.total_rows_in_pattern_matrix);
    
    printf("PATTERN MATRIX:\n  ");
    for (int y = 0; y < song.total_rows_in_pattern_matrix; y++){
        for (int x = 0; x < song.system_total_channels; x++){
            printf("%02x ", song.pattern_matrix_value[x][y]);
        }
        printf("\n  ");
    }

    printf("\nINSTRUMENTS:\n");
    printf("total_instruments: %d\n", song.total_instruments);
    for (int i = 0; i < song.total_instruments; i++){
        printf("Instrument %d: name_length: %d\n", i, song.instruments[i].name_length);
        printf("Instrument %d: name: %s\n", i, song.instruments[i].name);
        printf("Instrument %d: mode: %d\n", i, song.instruments[i].mode);

        if (song.instruments[i].mode == MODE_FM){
            printf("Instrument %d: ALG: %d\n", i, song.instruments[i].ALG);
            printf("Instrument %d: FB: %d\n", i, song.instruments[i].FB);
            printf("Instrument %d: LFO: %d\n", i, song.instruments[i].LFO);
            printf("Instrument %d: LFO2: %d\n", i, song.instruments[i].LFO2);

            for (int k = 0; k < 4; k++){
                printf("Instrument %d: Operator %d: AM: %d\n", i, k + 1, song.instruments[i].FM_operators[k].AM);
                printf("Instrument %d: Operator %d: AR: %d\n", i, k + 1, song.instruments[i].FM_operators[k].AR);
                printf("Instrument %d: Operator %d: DR: %d\n", i, k + 1, song.instruments[i].FM_operators[k].DR);
                printf("Instrument %d: Operator %d: MULT: %d\n", i, k + 1, song.instruments[i].FM_operators[k].MULT);
                printf("Instrument %d: Operator %d: RR: %d\n", i, k + 1, song.instruments[i].FM_operators[k].RR);
                printf("Instrument %d: Operator %d: SL: %d\n", i, k + 1, song.instruments[i].FM_operators[k].SL);
                printf("Instrument %d: Operator %d: TL: %d\n", i, k + 1, song.instruments[i].FM_operators[k].TL);
                printf("Instrument %d: Operator %d: DT2: %d\n", i, k + 1, song.instruments[i].FM_operators[k].DT2);
                printf("Instrument %d: Operator %d: RS: %d\n", i, k + 1, song.instruments[i].FM_operators[k].RS);
                printf("Instrument %d: Operator %d: DT: %d\n", i, k + 1, song.instruments[i].FM_operators[k].DT);
                printf("Instrument %d: Operator %d: D2R: %d\n", i, k + 1, song.instruments[i].FM_operators[k].D2R);
                printf("Instrument %d: Operator %d: SSGMODE: %d\n", i, k + 1, song.instruments[i].FM_operators[k].SSGMODE);
            }
        }else{
            if (song.system != SYSTEM_GAMEBOY){
                printf("Instrument %d: volume_envelope_size: %d\n", i, song.instruments[i].volume_envelope_size);

                if (song.instruments[i].volume_envelope_size > 0){
                    printf("Instrument %d: volume_envelope:", i);
                    for (int l = 0; l < song.instruments[i].volume_envelope_size; ++l){
                        if (l % 8 == 0) printf("\n  ");
                        printf("%02d ", song.instruments[i].volume_envelope[l]);
                    }
                    printf("\n");
                    printf("Instrument %d: volume_loop_position: %d\n", i, song.instruments[i].volume_loop_position);
                }
            }
            printf("Instrument %d: arpeggio_envelope_size: %d\n", i, song.instruments[i].arpeggio_envelope_size);
            if (song.instruments[i].arpeggio_envelope_size > 0){
                printf("Instrument %d: arpeggio_envelope:", i);
                for (int l = 0; l < song.instruments[i].arpeggio_envelope_size; ++l){
                    if (l % 8 == 0) printf("\n  ");
                    printf("%02d ", song.instruments[i].arpeggio_envelope[l]);
                }
                printf("\n");
                printf("Instrument %d: arpeggio_loop_position: %d\n", i, song.instruments[i].arpeggio_loop_position);
            }
            printf("Instrument %d: arpeggio_marcro_mode: %d\n", i, song.instruments[i].arpeggio_macro_mode);

            printf("Instrument %d: duty_noise_envelope_size: %d\n", i, song.instruments[i].duty_noise_envelope_size);
            if (song.instruments[i].duty_noise_envelope_size > 0){
                printf("Instrument %d: duty_noise_envelope:", i);
                for (int l = 0; l < song.instruments[i].duty_noise_envelope_size; ++l){
                    if (l % 8 == 0) printf("\n  ");
                    printf("%02d ", song.instruments[i].duty_noise_envelope[l]);
                }
                printf("\n");
                printf("Instrument %d: duty_noise_loop_position: %d\n", i, song.instruments[i].duty_noise_loop_position);
            }

            printf("Instrument %d: wavetable_envelope_size: %d\n", i, song.instruments[i].wavetable_envelope_size);
            if (song.instruments[i].wavetable_envelope_size > 0){
                printf("Instrument %d: wavetable_envelope:", i);
                for (int l = 0; l < song.instruments[i].wavetable_envelope_size; ++l){
                    if (l % 8 == 0) printf("\n  ");
                    printf("%02d ", song.instruments[i].wavetable_envelope[l]);
                }
                printf("\n");
                printf("Instrument %d: wavetable_loop_position: %d\n", i, song.instruments[i].wavetable_loop_position);
            }

            if (song.system == SYSTEM_C64_SID_6581 || song.system == SYSTEM_C64_SID_8580){
                printf("Instrument %d: c64_triangle_wave_enabled: %d\n", i, song.instruments[i].c64_triangle_wave_enabled);
                printf("Instrument %d: c64_saw_wave_enabled: %d\n", i, song.instruments[i].c64_saw_wave_enabled);
                printf("Instrument %d: c64_pulse_wave_enabled: %d\n", i, song.instruments[i].c64_pulse_wave_enabled);
                printf("Instrument %d: c64_noise_wave_enabled: %d\n", i, song.instruments[i].c64_noise_wave_enabled);
                printf("Instrument %d: c64_attack: %d\n", i, song.instruments[i].c64_attack);
                printf("Instrument %d: c64_decay: %d\n", i, song.instruments[i].c64_decay);
                printf("Instrument %d: c64_sustain: %d\n", i, song.instruments[i].c64_sustain);
                printf("Instrument %d: c64_release: %d\n", i, song.instruments[i].c64_release);
                printf("Instrument %d: c64_pulse_width: %d\n", i, song.instruments[i].c64_pulse_width);
                printf("Instrument %d: c64_ring_modulation_enabled: %d\n", i, song.instruments[i].c64_ring_modulation_enabled);
                printf("Instrument %d: c64_sync_modulation_enabled: %d\n", i, song.instruments[i].c64_sync_modulation_enabled);
                printf("Instrument %d: c64_to_filter: %d\n", i, song.instruments[i].c64_to_filter);
                printf("Instrument %d: c64_volume_macro_to_filter_cutoff_enabled: %d\n", i, song.instruments[i].c64_volume_macro_to_filter_cutoff_enabled);
                printf("Instrument %d: c64_use_filter_values_from_instrument: %d\n", i, song.instruments[i].c64_use_filter_values_from_instrument);
                // filter_globals
                printf("Instrument %d: c64_filter_resonance: %d\n", i, song.instruments[i].c64_filter_resonance);
                printf("Instrument %d: c64_filter_cutoff: %d\n", i, song.instruments[i].c64_filter_cutoff);
                printf("Instrument %d: c64_filter_high_pass: %d\n", i, song.instruments[i].c64_filter_high_pass);
                printf("Instrument %d: c64_filter_band_pass: %d\n", i, song.instruments[i].c64_filter_band_pass);
                printf("Instrument %d: c64_filter_low_pass: %d\n", i, song.instruments[i].c64_filter_low_pass);
                printf("Instrument %d: c64_filter_ch2_off: %d\n", i, song.instruments[i].c64_filter_ch2_off);
            }else if (song.system == SYSTEM_GAMEBOY){
                printf("Instrument %d: gb_envelope_volume;: %d\n", i, song.instruments[i].gb_envelope_volume);
                printf("Instrument %d: gb_envelope_direction;: %d\n", i, song.instruments[i].gb_envelope_direction);
                printf("Instrument %d: gb_envelope_length;: %d\n", i, song.instruments[i].gb_envelope_length);
                printf("Instrument %d: gb_sound_length;: %d\n", i, song.instruments[i].gb_sound_length);
            }
        }
        printf("\n");

        printf("total_wavetables: %d\n\n", song.total_wavetables);
        if (song.total_wavetables > 0) printf("WAVETABLES:\n");
        for (int i = 0; i < song.total_wavetables; ++i){
            printf("wavetable %d: size: %d\n", i, song.wavetables[i].size);
            printf("wavetable %d: data:", i);
            for (int j = 0; j < song.wavetables[i].size; ++j){
                if (j % 8 == 0) printf("\n  ");
                printf("%02d ", song.wavetables[i].data[j]);
            }
            printf("\n\n");
        }

        printf("PATTERN DATA:\n");
        for (int i = 0; i < song.system_total_channels; ++i){
            printf("Channel %d: effect_columns_count: %d\n", i, song.channels[i].effect_columns_count);
            for (int j = 0; j < song.total_rows_in_pattern_matrix; ++j){
                printf("Channel %d: pattern_no. %d:\n", i, j);
                for (int k = 0; k < song.total_rows_per_pattern; ++k){
                    char *noteStr;
                    switch (song.channels[i].rows[j][k].note){
                            case  1: noteStr = "C#"; break;
                            case  2: noteStr = "D-"; break;
                            case  3: noteStr = "D#"; break;
                            case  4: noteStr = "E-"; break;
                            case  5: noteStr = "F-"; break;
                            case  6: noteStr = "F#"; break;
                            case  7: noteStr = "G-"; break;
                            case  8: noteStr = "G#"; break;
                            case  9: noteStr = "A-"; break;
                            case 10: noteStr = "A#"; break;
                            case 11: noteStr = "B-"; break;
                            case 12: noteStr = "C-"; break;
                            default: noteStr = "--"; break;
                    }
                    //printf("%c\n", song.channels[i].rows[j][k].note - (song.channels[i].rows[j][k].note%2));
                    if (song.channels[i].rows[j][k].note == 0 && song.channels[i].rows[j][k].octave == 0)
                        printf("|---");
                    else if (song.channels[i].rows[j][k].note == 100)
                        printf("|OFF");
                    else
                        printf("|%s%d", noteStr, song.channels[i].rows[j][k].octave);

                    if (song.channels[i].rows[j][k].volume == -1)
                        printf(" --|");   
                    else
                        printf(" %02X|", song.channels[i].rows[j][k].volume);

                    if (song.channels[i].rows[j][k].instrument == -1)
                        printf("-- ");
                    else
                        printf("%02X ", song.channels[i].rows[j][k].instrument);

                    for (int l = 0; l < song.channels[i].effect_columns_count; ++l){
                        if (song.channels[i].rows[j][k].commands[l].code == -1) printf("--");
                        else printf("%02X", song.channels[i].rows[j][k].commands[l].code);
                        if (song.channels[i].rows[j][k].commands[l].value == -1) printf(" --|");
                        else printf(" %02X|", song.channels[i].rows[j][k].commands[l].value);
                    }
                    printf("\n");
                }
            }
        }

        // Samples
        printf("\nSAMPLES:\n");
        printf("total_samples: %d\n", song.total_samples);
        for (int i = 0; i < song.total_samples; ++i){
            printf("sample %d: size: %d\n", i, song.samples[i].size);
            printf("sample %d: name_length: %d\n", i, song.samples[i].name_length);
            printf("sample %d: name: %s\n", i, song.samples[i].name);

        }
    }
}

int verify_dmf(char *filename){
    unsigned char *original = malloc(MAX_DMF_SIZE);
    int status = openDMF(filename, original);
    if (status) return status;

    dmf song;
    status = parseDMF(original, &song);
    if (status) return status;

    unsigned char *parsed = malloc(MAX_DMF_SIZE);
    size_t sizeOfParsed;
    dmfToBuffer (song, parsed, &sizeOfParsed);

    for (int i = 0; i < sizeOfParsed; ++i){
        if (original[i] != parsed[i]){
            printf("Uh Oh! These aren't the same! D: PLS EMAIL ME ABOUT THIS MESSAGE at 'aaronjcottle (at) gmail (dot) cum'\n");
            return -1;
        }
    }
    printf("Verified Working!\n");

    return 0;
}
