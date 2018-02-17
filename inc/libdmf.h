/*! \file libdmf.h
    \brief Main library file.
*/

/*! \mainpage
 * # <center>🤘 👇 Useful links 👇 🤘</center>
 * # <center>👉 libdmf.h 👈</center>
 * # <center>👉 #dmf 👈</center>
 */

#ifndef _DMF_PARSER_H_
#define _DMF_PARSER_H_

#include <stdlib.h>

#define MAX_DMF_SIZE           16777216 //!< The maximum size (in bytes) of the decompressed DMF.

// The system in the DMF
#define SYSTEM_GENESIS         0x02 //!< (SYSTEM_TOTAL_CHANNELS 10)
#define SYSTEM_GENESIS_EXT_CH3 0x12 //!< (SYSTEM_TOTAL_CHANNELS 13)
#define SYSTEM_SMS             0x03 //!< (SYSTEM_TOTAL_CHANNELS 4)
#define SYSTEM_GAMEBOY         0x04 //!< (SYSTEM_TOTAL_CHANNELS 4)
#define SYSTEM_PCENGINE        0x05 //!< (SYSTEM_TOTAL_CHANNELS 6)
#define SYSTEM_NES             0x06 //!< (SYSTEM_TOTAL_CHANNELS 5)
#define SYSTEM_C64_SID_8580    0x07 //!< (SYSTEM_TOTAL_CHANNELS 3)
#define SYSTEM_C64_SID_6581    0x47 //!< (SYSTEM_TOTAL_CHANNELS 3) Supposed to be 0x17 was 71
#define SYSTEM_YM2151          0x08 //!< (SYSTEM_TOTAL_CHANNELS 13)

#define REIGON_PAL             0x00 //!< The PAL Reigon. (Perfection At Last)
#define REIGON_NTSC            0x01 //!< The NTSC Reigon. (Never The Same Colour)

#define MODE_STD               0x00 //!< Standard mode
#define MODE_FM                0x01 //!< Frequency Modulation mode.

// Helper structs:
//!
//! All of the information to reconstruct an FM_operator
//!
typedef struct {
    unsigned char AM; //!< Amplitude Modulation enable, whether or not this operator will allow itself to be modified by the LFO.
    unsigned char AR; //!< Attack rate, the angle of initial amplitude increase.
    unsigned char DR; //!< The angle of initial amplitude decrease.
    unsigned char MULT; //!< A multiplier to the frequency.
    unsigned char RR; //!< The final angle of amplitude decrease, after ‘key off’.
    unsigned char SL; //!< The amplitude at which the slower amplitude decrease starts.
    unsigned char TL; //!< Total level, the highest amplitude of the waveform.
    unsigned char DT2; //!< Detune 2 used only on the YM2151 see #DT
    unsigned char RS; //!< Rate scaling, the degree to which envelopes become shorter as frequencies become higher.
    unsigned char DT; //!< Detune, a number between -3 and 3. Positive is a sharper detune, whilst negative is lower.
    unsigned char D2R;  //!< The angle of secondary amplitude decrease. This will continue indefinitely unless ‘key off’ occurs.
    unsigned char SSGMODE; //!< The SSG mode (BIT 4 = 0 Disabled, 1 Enabled, BITS 0,1,2 SSG_MODE)
} FM_operator;

//!
//! Instrument info
//!
typedef struct {
    unsigned char name_length; //!< The length of the name of the instrument
    unsigned char *name; //!< The name of the instrument
    unsigned char mode; //!< The mode (0 = STD, 1 = FM)

    // FM Info
        unsigned char ALG; //!< The FM Algorithm
        unsigned char FB; //!< The feedback for operator 1
        unsigned char LFO; //!< Low Frequency Oscillator 1 (FMS on YM2612, PMS on YM2151)
        unsigned char LFO2; //!< Low Frequency Oscillator 2 (AMS on YM2612, AMS on YM2151)

        //!
        //! A list of bits for the FM operators, if there is no FM
        //! This will be NULL. BEWARE!
        //!
        FM_operator *FM_operators; //!< The operator data

    // STD Info
        // Volume Macro:
            unsigned char volume_envelope_size; //!< Size of volume envelope. Ranges from [0 - 127]
            //!
            //! WILL BE A NULL POINTER IF GAMEBOY! DO NOT DEREFRENCE IF THE SYSTEM IS GAMEBOY!
            //!
            unsigned int *volume_envelope; //!< Volume envelope data
            signed char volume_loop_position; //!< The loop position of the volume envelope. -1 = NO_LOOP

        // Arpeggio Macro:
            unsigned char arpeggio_envelope_size; //!< Size of arpeggio envelope. Ranges from [0 - 127]
            signed int *arpeggio_envelope; //!< Actual arpeggio envelope
            signed char arpeggio_loop_position; //!< The loop position of the arpeggio envelope. -1 = NO_LOOP
            unsigned char arpeggio_macro_mode; //!< The arpeggio macro mode (0 = Normal, 1 = Fixed)

        // Duty/Noise Macro:
            unsigned char duty_noise_envelope_size; //!< Size of the duty/noise envelope. Ranges from [0 - 127]
            unsigned int *duty_noise_envelope; //!< Actual duty/noise envelope
            signed char duty_noise_loop_position; //!< The loop position of the duty/noise envelope. -1 = NO_LOOP

        // Wavetable Macro:
            unsigned char wavetable_envelope_size; //!< Size of the wavetable envelope. Ranges from [0 - 127]
            unsigned int *wavetable_envelope; //!< Actual wavetable envelope
            signed char wavetable_loop_position; //!< The loop position of the wavetable envelope. -1 = NO_LOOP

        // PER SYSTEM DATA:
            // C64:
                unsigned char c64_triangle_wave_enabled; //!< C64 shite, see the name of this member
                unsigned char c64_saw_wave_enabled; //!< C64 shite, see the name of this member
                unsigned char c64_pulse_wave_enabled; //!< C64 shite, see the name of this member
                unsigned char c64_noise_wave_enabled; //!< C64 shite, see the name of this member
                unsigned char c64_attack; //!< C64 shite, see the name of this member
                unsigned char c64_decay; //!< C64 shite, see the name of this member
                unsigned char c64_sustain; //!< C64 shite, see the name of this member
                unsigned char c64_release; //!< C64 shite, see the name of this member
                unsigned char c64_pulse_width; //!< C64 shite, see the name of this member
                unsigned char c64_ring_modulation_enabled; //!< C64 shite, see the name of this member
                unsigned char c64_sync_modulation_enabled; //!< C64 shite, see the name of this member
                unsigned char c64_to_filter; //!< C64 shite, see the name of this member
                unsigned char c64_volume_macro_to_filter_cutoff_enabled; //!< C64 shite, see the name of this member
                unsigned char c64_use_filter_values_from_instrument; //!< C64 shite, see the name of this member
                //filter_globals
                unsigned char c64_filter_resonance; //!< C64 shite, see the name of this member
                unsigned char c64_filter_cutoff; //!< C64 shite, see the name of this member
                unsigned char c64_filter_high_pass; //!< C64 shite, see the name of this member
                unsigned char c64_filter_band_pass; //!< C64 shite, see the name of this member
                unsigned char c64_filter_low_pass; //!< C64 shite, see the name of this member
                unsigned char c64_filter_ch2_off; //!< C64 shite, see the name of this member
            // GAMEBOY
                unsigned char gb_envelope_volume; //!< GB shite, see the name of this member
                unsigned char gb_envelope_direction; //!< GB shite, see the name of this member
                unsigned char gb_envelope_length; //!< GB shite, see the name of this member
                unsigned char gb_sound_length; //!< GB shite, see the name of this member
}instrument;

// Wavetable shite
//!
//! Wavetable info
//!
typedef struct {
    unsigned int size; //!< Size of the wavetable
    unsigned int *data; //!< Array of ints containing the wavetable data
} wavetable;

// Notes
//!
//! A command for a note
//!
typedef struct {
    signed short code; //!< The command for a row
    signed short value; //!< The value of that command
} note_command;

//!
//! Row information
//!
typedef struct {
    unsigned short note; //!< The note. Ranges from [1-12]. 1 = C#, 12 = C
    unsigned short octave; //!< The octave of the note.
    signed short volume; //!< The volume of the note. -1 if empty.
    note_command *commands; //!< A list of commands.
    signed short instrument; //!< The instrument of the note. -1 if empty
} note_row;

//!
//! A channles worth of information
//!
typedef struct {
    unsigned char effect_columns_count; //!< Number of effect columns for that channel
    note_row **rows; //!< An array of array of rows, first go down the pattern matrix, then the pattern
} note_channel;

// Samples
//!
//! Sample info!
//!
typedef struct {
    unsigned int size; //!< The size of the sample in unsigned ints
    unsigned char name_length; //!< Length of the sample name
    unsigned char *name; //!< The name of the sample

    unsigned char rate; //!< Bitrate of the sample
    unsigned char pitch; //!< Deflemasks weird pitch thing
    unsigned char amp; //!< The volume of the sample
    unsigned char bits; //!< Bit depth of sample. (8 or 16)

    unsigned short *data; //!< Actual sample data
} sample;

// Actual DMF
//!
//! And finally, the actual dmf struct. Can't believe it took so long to get here.
//!
typedef struct {

    // SYSTEM INFORMATION
    unsigned char system; //!< The console this dmf is written for
    unsigned char system_total_channels; //!< The number of channels in that system

    // VISUAL INFORMATION
    unsigned char name_length; //!< The length of the dmf's name
    char *name; //!< The name of the dmf

    unsigned char author_length; //!< The length of the dmf's author's name
    char *author; //!< The author of the dmf

    unsigned char highlight_A; //!< Row highlight A
    unsigned char highlight_B; //!< Row highlight B

    // MODULE INFORMATION
    unsigned char time_base; //!< Base Time
    unsigned char tick_time_1; //!< Speed variable 1. e.g. tick_time_1/tick_time_2
    unsigned char tick_time_2; //!< Speed variable 2. e.g. tick_time_1/tick_time_2

    unsigned char reigon; //!< The reigon of the dmf. 0 = PAL, 1 = NTSC
    unsigned char custom_hz_on; //!< Whether or not the dmf has a custom framerate
    unsigned short custom_hz; //!< The actual custom framerate

    int total_rows_per_pattern; //!< The total rows in a pattern (defualt 64)
    unsigned char total_rows_in_pattern_matrix; //!< The number of rows in the pattern matrix
    unsigned char **pattern_matrix_value; //!< The pattern matrix data, it runs down and then across

    unsigned char total_instruments; //!< Total number of instruments in the dmf
    instrument *instruments; //!< The instrument data

    unsigned char total_wavetables; //!< Total number of wavetables in the dmf
    wavetable *wavetables; //!< The wavetable data

    note_channel *channels; //!< Actual note data

    unsigned char total_samples; //!< Total number of samples in the dmf
    sample *samples; //!< Actual sample data
} dmf;

int openDmfFileIntoBuffer(char *filename, unsigned char *dest, size_t *length);
/*! \fn int openDmfFileIntoBuffer(char *filename, unsigned char *dest, size_t *length)
    \brief Use a filename to open a file into a memory buffer.
    \param filename The path to the file to open.
    \param dest The buffer write to.
    \param length A variable that gets filled with the size of the destination.
*/

int decompressDMF(unsigned char *src, size_t src_length, unsigned char *dest);
/*! \fn int decompressDMF(unsigned char *src, size_t src_length, unsigned char *dest)
    \brief Takes buffer from openDmfFileIntoBuffer() and decompresses it using miniz.h
    \param src The source buffer.
    \param src_length The length of the source buffer.
    \param dest A destination buffers.
*/

int openDMF(char *filename, unsigned char *dest);
/*! \fn int openDMF(char *filename, unsigned char *dest)
    \brief A combination of openDmfFileIntoBuffer() and decompressDMF()
    \param filename The path to the file to open.
    \param dest The destination buffer.
*/

int parseDMF(unsigned char *decompressed_dmf, dmf *dest);
/*! \fn int parseDMF(unsigned char *decompressed_dmf, dmf *dest)
    \brief Parse an uncompressed buffer into the #dmf type
    \param decompressed_dmf uncompressed buffer to use as input.
    \param dest destination #dmf.
*/

int fileToDmfType(char *filename, dmf *dest);
/*! \fn int fileToDmfType(char *filename, dmf *dest)
    \brief A combination of openDMF() and parseDMF(). Takes a file and puts it int the #dmf type.
    \param filename The path to the file to open. 
    \param dest destination #dmf.
*/

// Writing files
int dmfToBuffer(dmf src, unsigned char *dest, size_t *size);
/*! \fn int dmfToBuffer(dmf src, unsigned char *dest, size_t *size)
    \brief The opposite of parseDMF(). Converts #dmf type to uncompressed dmf file
    \param src The source #dmf
    \param dest destination buffer.
    \param size A variable that gets filled with the size of the destination.
*/

int compressDMF(const unsigned char *src, size_t src_length, unsigned char *dest, size_t *dest_length);
/*! \fn int compressDMF(const unsigned char *src, size_t src_length, unsigned char *dest, size_t *dest_length)
    \brief The opposite of decompressDMF(). Converts an uncompressed buffer to a compressed buffer
    \param src The source buffer.
    \param src_length The length of the source buffer
    \param dest destination buffer.
    \param dest_length A variable that gets filled with the size of the destination buffer.
*/

int writeDMF(char *filename, dmf src);
/*! \fn int writeDMF(char *filename, dmf src)
    \brief A combination of compressDMF() and dmfToBuffer(). Converts #dmf type to compressed buffer and writes it to file
    \param filename The path of the file to write to.
    \param src The source #dmf.
*/

// Testing.
void displayDMF(dmf song);
/*! \fn void display_dmf(dmf song)
    \brief Print out everything to do with a #dmf
    \param song the dmf to print
*/

int verifyDMF(char *filename);
/*! \fn int verify_dmf(char *filename);
    \brief Verify a DMF's integrety (or for that matter libdmf's :/)
    \param filename The path to open, verify and close.
*/

#endif // _DMF_PARSER_H_
