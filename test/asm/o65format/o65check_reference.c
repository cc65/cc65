#include "o65check.h"

#include <stdlib.h>
#include <string.h>

// #define VERBOSE 1

#if VERBOSE
  #define VERBOSE_FPRINTF(...) \
      fprintf(__VA_ARGS__)
#else
  #define VERBOSE_FPRINTF(...) do { } while (0)
#endif

static char * reference_file_read_one_line(FILE * reference_file)
{
    static char line[4096];

    line[sizeof line - 1] = 0;
    if (!fgets(line, sizeof line - 1, reference_file)) {
        return NULL;
    }

    return line;
}

typedef enum reference_command_e {
    CMD_FILENAME,
    CMD_OS,
    CMD_EXTERNAL,
    CMD_EXTERNALSCOUNT,
    CMD_GLOBAL,
    CMD_GLOBALSCOUNT,
    CMD_RELOC_TEXT,
    CMD_RELOC_TEXT_RAW,
    CMD_RELOC_DATA,
    CMD_RELOC_DATA_RAW,
    CMD_UNKNOWN
} reference_command;

static
const char * CMD_STRINGS[] = {
    "FILENAME:",
    "OS:",
    "EXTERNAL:",
    "EXTERNALSCOUNT:",
    "GLOBAL:",
    "GLOBALSCOUNT:",
    "RELOC_TEXT:",
    "RELOC_TEXT_RAW:",
    "RELOC_DATA:",
    "RELOC_DATA_RAW:"
};

static reference_command get_command_from_string(const char * string)
{
    for (reference_command cmd = 0; cmd < (sizeof(CMD_STRINGS) / sizeof(CMD_STRINGS[0])); ++cmd) {
        if (strcmp(CMD_STRINGS[cmd], string) == 0) {
            return cmd;
        }
    }

    return CMD_UNKNOWN;
}

static o65_option_t * reference_find_header_option(size_t optiontype)
{
    o65_option_t * p = NULL;

    for (p = o65_param.options; p != NULL; p = p->next) {
        if (p->type == optiontype) {
            break;
        }
    }

    return p;
}

static const char * reference_find_external(const char * external_comp)
{
    const char * p = NULL;
    int i;

    for (i = 0; i < o65_param.externals_count; ++i) {
        if (strcmp(o65_param.externals[i], external_comp) == 0) {
            p = o65_param.externals[i];
            break;
        }
    }

    return p;
}

static const o65_global_t * reference_find_global(const char * global_comp)
{
    const o65_global_t * p = NULL;
    int i;

    for (i = 0; i < o65_param.globals_count; ++i) {
        if (strcmp(o65_param.globals[i].name, global_comp) == 0) {
            p = &o65_param.globals[i];
            break;
        }
    }

    return p;
}

static size_t reference_integer_argument(char ** arguments)
{
    long value = -1;

    value = strtol(arguments[0], NULL, 0);

    return value;
}

static void reference_check_no_of_arguments(size_t no_of_arguments, size_t expected_number_of_arguments)
{
    if (no_of_arguments != 1) {
        fprintf(stderr, "Wrong number of arguments to FILENAME command\n");
        exit(1);
    }
}

static char * reference_string_argument(size_t no_of_arguments, char ** arguments)
{
    reference_check_no_of_arguments(no_of_arguments, 1);

    return arguments[0];
}

static o65_relocation_t * reference_find_relocation_entry(o65_relocation_t * relocation_entries, size_t address )
{
    while (relocation_entries != NULL) {

        if (relocation_entries->base == address) {
            break;
        }
        relocation_entries = relocation_entries->next;
    }

    return relocation_entries;
}

static int reference_check_reloc_text_or_data(char ** arguments, size_t o65_base, o65_relocation_t * relocation_entries)
{
    int error = 0;

    size_t expected_addr;
    size_t expected_type;
    size_t expected_seg;
    size_t expected_has_index = 0;
    size_t expected_index = 0;
    size_t expected_has_lower = 0;
    size_t expected_lower = 0;

    if (!arguments[0] || !arguments[1] || !arguments[2] || !arguments[4]) {
        fprintf(stderr, "Wrong number of arguments to RELOC_TEXT or RELOC_DATA command\n");
        exit(1);
    }
    expected_addr  = reference_integer_argument(&arguments[0]);
    expected_type  = reference_integer_argument(&arguments[1]);
    expected_seg   = reference_integer_argument(&arguments[2]);
    if (arguments[3][0] != '-') {
        expected_has_index = 1;
        expected_index = reference_integer_argument(&arguments[3]);
    }
    if (arguments[4][0] != '-') {
        expected_has_lower = 1;
        expected_lower = reference_integer_argument(&arguments[4]);
    }

    o65_relocation_t * reloc_entry = reference_find_relocation_entry(relocation_entries, expected_addr);

    if (reloc_entry == NULL) {
        fprintf(stderr, "Could not find relocation entry for address 0x%04zX.\n", expected_addr);
        exit(1);
    }

#define CHECK_RELOC_ENTRY_VALUES(_reloc, _expected, _text) \
    if (_reloc != _expected) { \
        fprintf(stderr, "Expected reloc value %s is wrong: content = %04zX, expected = %04zX\n", \
                _text, (size_t) _reloc, (size_t) _expected); \
        error = 1; \
    }
    CHECK_RELOC_ENTRY_VALUES(reloc_entry->typebyte,       expected_type,      "typebyte");
    CHECK_RELOC_ENTRY_VALUES(reloc_entry->segmentid,      expected_seg,       "segmentid");
    CHECK_RELOC_ENTRY_VALUES(reloc_entry->has_index,      expected_has_index, "has_index");
    CHECK_RELOC_ENTRY_VALUES(reloc_entry->index,          expected_index,     "index");
    CHECK_RELOC_ENTRY_VALUES(reloc_entry->has_low_offset, expected_has_lower, "has lower_offset");
    CHECK_RELOC_ENTRY_VALUES(reloc_entry->low_offset,     expected_lower,     "lower_offset");
#undef CHECK_RELOC_ENTRY_VALUES

    return error;
}

static o65_relocation_t * next_relocation_entry = NULL;

static int reference_check_reloc_text_or_data_raw(char ** arguments, size_t base, o65_relocation_t * relocation_entries)
{
    if (next_relocation_entry == NULL) {
        next_relocation_entry = relocation_entries;
    }

    if (next_relocation_entry == NULL) {
        fprintf(stderr, "There is no next relocation entry.");
        return 1;
    }

    int error = 0;

    size_t expected_count;
    size_t expected_type;

    if (!arguments[0]) {
        fprintf(stderr, "Wrong number of arguments to RELOC_TEXT or RELOC_DATA command\n");
        exit(1);
    }

    expected_count = reference_integer_argument(arguments++);
    if (next_relocation_entry->raw_data_size != expected_count) {
        fprintf(stderr, "Expected data size is wrong: content = %04zX, expected = %04zX\n",
                next_relocation_entry->raw_data_size, expected_count);
        error = 1;
    }

    for (int i = 0; i < expected_count; ++i) {
        if (arguments == 0) {
            fprintf(stderr, "Less data given than the size %zu tells us.\n",
                    expected_count);
            exit(1);
        }

        size_t expected_value = reference_integer_argument(arguments++);
        if (next_relocation_entry->raw_data[i] != expected_value) {
            fprintf(stderr, "Relocation data wrong: got 0x%02X, but expected 0x%02zX.\n",
                    next_relocation_entry->raw_data[i], expected_value);
            error = 1;
        }
    }
    if (arguments == 0) {
        fprintf(stderr, "More data given than the size %zu tells us.\n",
                expected_count);
        exit(1);
    }

    next_relocation_entry = next_relocation_entry->next;

    return error;
}

static int reference_check_reloc_text(char ** arguments)
{
    return reference_check_reloc_text_or_data(arguments, o65_param.tbase, o65_param.text_relocation);
}

static int reference_check_reloc_text_raw(char ** arguments)
{
    return reference_check_reloc_text_or_data_raw(arguments, o65_param.tbase, o65_param.text_relocation);
}

static int reference_check_reloc_data(char ** arguments)
{
    return reference_check_reloc_text_or_data(arguments, o65_param.dbase, o65_param.data_relocation);
}

static int reference_check_reloc_data_raw(char ** arguments)
{
    return reference_check_reloc_text_or_data_raw(arguments, o65_param.dbase, o65_param.data_relocation);
}

static int reference_process_cmd(reference_command cmd, size_t no_of_arguments, char ** arguments)
{
    switch (cmd) {
        case CMD_FILENAME:
        {
            const char * filename_comp = reference_string_argument(no_of_arguments, arguments);
            o65_option_t * option = reference_find_header_option(O65_OPTION_FILENAME);
            if (!option) {
                fprintf(stderr, "Could not find the filename header\n");
                return 1;
            }
            if (strcmp(option->data, filename_comp) != 0) {
                fprintf(stderr, "File name does not match: '%s' != '%s'\n", option->data, filename_comp);
                return 1;
            }
            break;
        }

        case CMD_OS:
        {
            const size_t value_comp = reference_integer_argument(arguments);
            o65_option_t * option = reference_find_header_option(O65_OPTION_OS);
            if (!option) {
                fprintf(stderr, "Could not find the OS header\n");
                return 1;
            }
            if (option->data[0] != value_comp) {
                fprintf(stderr, "OS does not match: 0x%02zX != 0x%02zX\n", (size_t) option->data[0], value_comp);
                return 1;
            }
            break;
        }

        case CMD_EXTERNAL:
        {
            const char * external_comp = reference_string_argument(no_of_arguments, arguments);
            const char * external = reference_find_external(external_comp);
            if (!external) {
                fprintf(stderr, "Could not find the external '%s'\n", external_comp);
                return 1;
            }
            break;
        }

        case CMD_EXTERNALSCOUNT:
        {
            const size_t value_comp = reference_integer_argument(arguments);
            if (o65_param.externals_count != value_comp) {
                fprintf(stderr, "Count of externals does not match: 0x%02zX != 0x%02zX\n", o65_param.externals_count, value_comp);
                return 1;
            }
            break;
        }

        case CMD_GLOBAL:
        {
            const char * global_comp = reference_string_argument(no_of_arguments, arguments);
            const o65_global_t * global = reference_find_global(global_comp);
            if (!global) {
                fprintf(stderr, "Could not find the global '%s'\n", global_comp);
                return 1;
            }
            break;
        }

        case CMD_GLOBALSCOUNT:
        {
            const size_t value_comp = reference_integer_argument(arguments);
            if (o65_param.globals_count != value_comp) {
                fprintf(stderr, "Count of globals does not match: 0x%02zX != 0x%02zX\n", o65_param.globals_count, value_comp);
                return 1;
            }
            break;
        }

        case CMD_RELOC_TEXT:
            if (reference_check_reloc_text(arguments)) return 1;
            break;

        case CMD_RELOC_TEXT_RAW:
            if (reference_check_reloc_text_raw(arguments)) return 1;
            break;

        case CMD_RELOC_DATA:
            if (reference_check_reloc_data(arguments)) return 1;
            break;

        case CMD_RELOC_DATA_RAW:
            if (reference_check_reloc_data_raw(arguments)) return 1;
            break;

        case CMD_UNKNOWN:
            break;

        default:
            break;
    }

    return 0;
}

static int reference_split_commandline(char * cmdline, size_t max_no_arguments, char ** arguments)
{
    size_t no_arguments = 0;

    size_t readptr = 0;
    size_t writeptr = 0;

    if ((cmdline[0] == 0) || (cmdline[0] == '\r') || (cmdline[0] == '#')) {
        arguments[0] = NULL;
        cmdline[0] = 0;
        return 0;
    }

    while (1) {
        int is_quote_mode = 0;

        char * start_argument = NULL;
        int    end_argument = 0;
        char   ch;

        do {
            ch = cmdline[readptr++];

            switch (ch) {
                case '\t':
                    ch = ' ';
                    /* FALL THROUGH */

                case ' ':
                    if (start_argument != 0) {
                        if (!is_quote_mode) {
                            cmdline[writeptr++] = 0;
                            end_argument = 1;
                            break;
                        }
                    }
                    if (is_quote_mode) {
                        cmdline[writeptr++] = ch;
                    }
                    else if (start_argument != 0) {
                        cmdline[writeptr++] = 0;
                        end_argument = 1;
                    }
                    break;

                case '"':
                    if (start_argument == 0) {
                        /* this starts a new argument */
                        start_argument = &cmdline[writeptr];
                    }
                    else {
                        /* this ends an argument */
                        end_argument = 1;
                        cmdline[writeptr++] = 0;
                    }
                    is_quote_mode = ! is_quote_mode;
                    break;

                case '\\':
                    /* Take the next char literally, without more processing */
                    ch = cmdline[readptr++];
                    cmdline[writeptr++] = ch;
                    if (ch != 0) {
                        break;
                    }
                    /* FALL THROUGH */

                case '\r':
                    /* FALL THROUGH */
                case '\n':
                    ch = 0;
                    /* FALL THROUGH */

                case 0:
                    end_argument = 1;
                    cmdline[writeptr++] = 0;
                    break;

                default:
                    if (start_argument == 0) {
                        start_argument = &cmdline[writeptr];
                    }
                    cmdline[writeptr++] = ch;
                    break;
            }

        } while (end_argument == 0);

        if (start_argument != 0) {
            arguments[no_arguments++] = start_argument;

            if (no_arguments >= max_no_arguments) {
                fprintf(stderr, "Commandline is too long, aborting!\n");
                exit(1);
            }

            start_argument = NULL;
            end_argument = 0;
        }
        else {
            break;
        }

    };

    arguments[no_arguments] = NULL;

    return no_arguments;
}

int o65_check_reference(file_t * o65_file, const char * filename)
{
    char * inputline = NULL;

    FILE * reference_file = NULL;

    reference_file = fopen(filename, "rt");
    if (!reference_file) return 1;

    do {
        const int commandline_elements_count = 20;
        char * commandline_elements[commandline_elements_count] = { };

        inputline = reference_file_read_one_line(reference_file);
        if (!inputline) {
            continue;
        }

        size_t arguments_count = reference_split_commandline(inputline, commandline_elements_count, commandline_elements);
        if (arguments_count < 0) break;
        if (arguments_count == 0) continue;

#if 1
        if (arguments_count == 0) break;

        reference_command cmd = get_command_from_string(commandline_elements[0]);

        if (cmd != CMD_UNKNOWN) {
            VERBOSE_FPRINTF(stderr, "Recognized command %s.\n", CMD_STRINGS[cmd]);

            int status = reference_process_cmd(cmd, arguments_count - 1, &commandline_elements[1]);
            if (status != 0) return status;
        }
        else {
            fprintf(stderr, "Unrecognized command %s.\n", commandline_elements[0]);
            return 1;
        }
#else
        if ((inputline[0] == 0) || inputline[0] == '#' || inputline[0] == '\r') continue;

        char * command_string = inputline;
        char * argument = NULL;

        char * delimiter = strchr(command_string, ' ');
        if (!delimiter) {
            delimiter = strchr(command_string, ':');
        }

        if (delimiter) {
            *delimiter = 0;

            argument = string_trim(delimiter + 1);

            if (delimiter > command_string && delimiter[-1] == ':') delimiter[-1] = 0;
        }

        if (*command_string == '\n') continue;

        reference_command cmd = get_command_from_string(command_string);

        if (cmd != CMD_UNKNOWN) {
            VERBOSE_FPRINTF(stderr, "Recognized command %s.\n", CMD_STRINGS[cmd]);

            int status = reference_process_cmd(cmd, argument);
            if (status != 0) return status;
        }
        else {
            fprintf(stderr, "Unrecognized command %s.\n", command_string);
            return 1;
        }
#endif

    } while (!feof(reference_file));

    fclose(reference_file);

    return 0;
}

