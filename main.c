#include <stdio.h>
#include "mystr.h"
#include "brainfuck.h"

#define ERROR_CODE 1

typedef enum { COM, SIM } program_state_t;

void help() {
    puts("Usage: brainfuck [MODE] [SOURCE] [OPTIONS]");
    puts(" like: brainfuck sim [SOURCE] [OPTIONS]");
    puts("   or: brainfuck com [SOURCE] -o [OUTPUT FILE] [OPTIONS]");
    puts("Flags:");
    puts("   - '-r' flag removing asm and object file after completing executable");
}

int simulate_bf(FILE *source, FILE *out) {
    char *code = get_bf_code_str(source);
    opt_code_t opt_code = optimize_code(code);
    free(code);
    env_t env = {
            .mem_ptr = 0,
            .mem_size = 30000,
            .dup_output = out,
    };
    int result_code = interpret_opt_code(env, opt_code);
    free(opt_code.tokens);
    return result_code;
}

int compile_bf_linux(FILE *source, FILE *out) {
    char *code = get_bf_code_str(source);
    opt_code_t opt_code = optimize_code(code);
    free(code);
    env_t env = {
            .mem_size = 30000,
    };
    int result_code = compile_opt_code(env, opt_code, out);
    free(opt_code.tokens);
    return result_code;
}

int main(int argc, char *argv[]) {
    puts("BrainFuck in C! v2.0");
    puts(" - Implemented by Electro98.");
    program_state_t state = SIM;
    if (argc < 3) {
        puts("Not enough argument provided to CLI.");
        help();
        return 1;
    }
    if (strcomp("sim", argv[1]))
        state = SIM;
    else if (strcomp("com", argv[1])) {
        state = COM;
    } else {
        printf("Invalid mode: \"%s\". Known: sim/com.\n", argv[1]);
        help();
        return 1;
    }
    FILE *input_file = fopen(argv[2], "r");
    if (!input_file) {
        printf("Can't open file \"%s\" as input file.\n", argv[2]);
        return 1;
    }
    char output_file_str[256];
    output_file_str[0] = 0;
    int delete_flag = 0;
    for (int arg_counter = 3; arg_counter < argc; ++arg_counter) {
        if (argv[arg_counter][0] != '-' || argv[arg_counter][2]) {
            printf("Invalid argument: %s.\n", argv[arg_counter]);
            help();
            return 1;
        }
        switch (argv[arg_counter][1]) {
            case 'o':
                arg_counter++;
                strcopy(output_file_str, argv[arg_counter]);
                break;
            case 'r':
                delete_flag = 1;
                break;
            default:
                printf("Unknown argument: %s.\n", argv[arg_counter]);
                help();
                return 1;
        }
    }
    // If there is no string in buffer
    if (!output_file_str[0] && state == COM) {
        size_t len = strlen(argv[2]);
        int dot_point = rseek(argv[2], '.');
        sprintf(output_file_str, "%s", argv[2]);
        if (dot_point == -1)
            dot_point = len;
        sprintf((output_file_str + dot_point), ".asm");
        output_file_str[dot_point + 4] = 0;
    }
    char output_obj_str[256], output_exec_str[256];
    // If there is any string in buffer
    FILE *output_file = 0;
    if (output_file_str[0]) {
        output_file = fopen(output_file_str, "w");
        if (!output_file) {
            printf("Can't open file \"%s\" as output file.\n", output_file_str);
            return 1;
        }
        if (state == COM) {
            strcopy(output_obj_str, output_file_str);
            int dot_point = rseek(output_obj_str, '.');
            if (dot_point == -1)
                dot_point = strlen(output_obj_str);
            sprintf((output_obj_str + dot_point), ".o");
            output_obj_str[dot_point + 2] = 0;
            strcopy(output_exec_str, output_file_str);
            output_exec_str[dot_point] = 0;
        }
    }
    int result_code = 0;
    switch (state) {
        case SIM:
            result_code = simulate_bf(input_file, output_file);
            break;
        case COM:
            result_code = compile_bf_linux(input_file, output_file);
            fclose(output_file);
            output_file = 0;
            if (!result_code) {
                char system_command[1024];
                // nasm call
                sprintf(system_command, "nasm -f elf64 -o %s %s", output_obj_str, output_file_str);
                fputs("[INFO]: ", stdout);
                puts(system_command);
                system(system_command);
                // ld call
                sprintf(system_command, "ld -o %s %s", output_exec_str, output_obj_str);
                fputs("[INFO]: ", stdout);
                puts(system_command);
                system(system_command);
                if (delete_flag) {
                    // clean asm file
                    sprintf(system_command, "rm %s", output_file_str);
                    fputs("[INFO]: Cleaning ", stdout);
                    puts(system_command);
                    system(system_command);
                    // clean obj file
                    sprintf(system_command, "rm %s", output_obj_str);
                    fputs("[INFO]: Cleaning ", stdout);
                    puts(system_command);
                    system(system_command);
                }
                // result call
                sprintf(system_command, "./%s", output_exec_str);
                fputs("[INFO]: Running ", stdout);
                puts(system_command);
                putc('\n', stdout);
                system(system_command);
            }
            break;
        default:
            puts("Unreachable");
            return 1;
    }

    if (output_file)
        fclose(output_file);
    if (input_file)
        fclose(input_file);
    return result_code;
}
