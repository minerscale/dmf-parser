#ifndef _DMF_PARSER_H_
#define _DMF_PARSER_H_

#include <stdio.h>

#define u8 unsigned char
#define s8 signed char
#define u16 unsigned short
#define s16 signed short

#define MAX_DMF_SIZE           16777216

// The system in the DMF
#define SYSTEM_GENESIS         0x02 // (SYSTEM_TOTAL_CHANNELS 10)
#define SYSTEM_GENESIS_EXT_CH3 0x12 // (SYSTEM_TOTAL_CHANNELS 13)
#define SYSTEM_SMS             0x03 // (SYSTEM_TOTAL_CHANNELS 4)
#define SYSTEM_GAMEBOY         0x04 // (SYSTEM_TOTAL_CHANNELS 4)
#define SYSTEM_PCENGINE        0x05 // (SYSTEM_TOTAL_CHANNELS 6)
#define SYSTEM_NES             0x06 // (SYSTEM_TOTAL_CHANNELS 5)
#define SYSTEM_C64_SID_8580    0x07 // (SYSTEM_TOTAL_CHANNELS 3)
#define SYSTEM_C64_SID_6581    0x47 // (SYSTEM_TOTAL_CHANNELS 3) // Suposed to be 0x17 was 71
#define SYSTEM_YM2151          0x08 // (SYSTEM_TOTAL_CHANNELS 13)

#define REIGON_PAL             0x00
#define REIGON_NTSC            0x01

#define MODE_STD               0x00
#define MODE_FM                0x01

// Helper structs:
typedef struct {
    u8 AM;
    u8 AR;
    u8 DR;
    u8 MULT;
    u8 RR;
    u8 SL;
    u8 TL;
    u8 DT2;
    u8 RS;
    u8 DT;
    u8 D2R;
    u8 SSGMODE; // (BIT 4 = 0 Disabled, 1 Enabled, BITS 0,1,2 SSG_MODE)
} FM_operator;

typedef struct {
    u8 name_length;
    u8 *name;
    u8 mode;

    // FM Info
        u8 ALG;
        u8 FB;
        u8 LFO; // (FMS on YM2612, PMS on YM2151)
        u8 LFO2; // (AMS on YM2612, AMS on YM2151)

        // A list of bits for the FM operators, if there is no FM
        // This will be NULL. BEWARE!
        FM_operator *FM_operators;

    // STD Info
        // Volume Macro:
            // volume_envelope is void pointer
            // when gameboy, BEWARE!!!
            u8 volume_envelope_size; // 0 - 127
            unsigned int *volume_envelope; // WILL BE NULL IF GAMEBOY
            s8 volume_loop_position; // -1 = NO_LOOP

        // Arpeggio Macro:
            u8 arpeggio_envelope_size;
            signed int *arpeggio_envelope;
            s8 arpeggio_loop_position; // -1 = NO_LOOP
            u8 arpeggio_macro_mode; // (0 = Normal, 1 = Fixed)

        // Duty/Noise Macro:
            u8 duty_noise_envelope_size;
            unsigned int *duty_noise_envelope;
            s8 duty_noise_loop_position; // -1 = NO_LOOP

        // Wavetable Macro:
            u8 wavetable_envelope_size;
            unsigned int *wavetable_envelope;
            s8 wavetable_loop_position; // -1 = NO_LOOP

        // PER SYSTEM DATA:
            // C64:
                u8 c64_triangle_wave_enabled;
                u8 c64_saw_wave_enabled;
                u8 c64_pulse_wave_enabled;
                u8 c64_noise_wave_enabled;
                u8 c64_attack;
                u8 c64_decay;
                u8 c64_sustain;
                u8 c64_release;
                u8 c64_pulse_width;
                u8 c64_ring_modulation_enabled;
                u8 c64_sync_modulation_enabled;
                u8 c64_to_filter;
                u8 c64_volume_macro_to_filter_cutoff_enabled;
                u8 c64_use_filter_values_from_instrument;
                //filter_globals
                u8 c64_filter_resonance;
                u8 c64_filter_cutoff;
                u8 c64_filter_high_pass;
                u8 c64_filter_band_pass;
                u8 c64_filter_low_pass;
                u8 c64_filter_ch2_off;
            // GAMEBOY
                u8 gb_envelope_volume;
                u8 gb_envelope_direction;
                u8 gb_envelope_length;
                u8 gb_sound_length;
}instrument;

// Wavetable shite
typedef struct {
    unsigned int size;
    unsigned int *data;
} wavetable;

// Notes
typedef struct {
    s16 code;
    s16 value;
} note_command;

typedef struct {
    u16 note;
    u16 octave;
    s16 volume;
    note_command *commands;
    s16 instrument;
} note_row;

typedef struct {
    u8 effect_columns_count;
    note_row **rows;
} note_channel;

// Samples
typedef struct {
    unsigned int size; // 4 bytes much?
    u8 name_length;
    u8 *name;

    u8 rate;
    u8 pitch;
    u8 amp;
    u8 bits; // 8 or 16

    u16 *data;
} sample;

// Actual DMF
typedef struct {
    // SYSTEM SET
    u8 system;
    u8 system_total_channels;

    // VISUAL INFORMATION
    u8 name_length;
    char *name;

    u8 author_length;
    char *author;

    u8 highlight_A;
    u8 highlight_B;

    // MODULE INFORMATION
    u8 time_base;
    u8 tick_time_1;
    u8 tick_time_2;

    u8 reigon; // 0 = PAL, 1 = NTSC
    u8 custom_hz_on;
    u16 custom_hz;

    int total_rows_per_pattern;
    u8 total_rows_in_pattern_matrix;
    u8 **pattern_matrix_value;

    u8 total_instruments;
    instrument *instruments;

    u8 total_wavetables;
    wavetable *wavetables;

    note_channel *channels;

    u8 total_samples;
    sample *samples;
} dmf;

// Reading files
int openFileIntoBuffer(char *filename, unsigned char *dest, size_t *length);
int decompressDMF(unsigned char *src, size_t src_length, unsigned char *dest);
int openDMF(char *filename, unsigned char *dest);
int parseDMF(unsigned char *decompressed_dmf, dmf *dest);
int fileToDmfType(char *filename, dmf *dest);

// Writing files
int dmfToBuffer(dmf src, unsigned char *dest, size_t *size);
int compressDMF(const unsigned char *src, size_t src_length, unsigned char *dest, size_t *dest_length);
int writeDMF(char *filename, dmf src);

#endif // _DMF_PARSER_H_
