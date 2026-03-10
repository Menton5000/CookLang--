#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "codegen.h"

// Helper to uppercase string for output mapping (mix -> MIX)
static void to_upper(char *dest, const char *src) {
    while (*src) {
        *dest = toupper(*src);
        dest++;
        src++;
    }
    *dest = '\0';
}

static void get_verb_progressive(char *dest, const char *verb) {
    if (strcasecmp(verb, "mix") == 0) strcpy(dest, "Mixing");
    else if (strcasecmp(verb, "chop") == 0) strcpy(dest, "Chopping");
    else if (strcasecmp(verb, "heat") == 0) strcpy(dest, "Heating");
    else if (strcasecmp(verb, "bake") == 0) strcpy(dest, "Baking");
    else if (strcasecmp(verb, "boil") == 0) strcpy(dest, "Boiling");
    else if (strcasecmp(verb, "serve") == 0) strcpy(dest, "Serving");
    else {
        // Fallback
        strcpy(dest, "Processing");
    }
}

void generate_code(Node *program, const char *output_name) {
    // We will generate a temporary C file, then compile it.
    FILE *f = fopen("temp_out.c", "w");
    if (!f) {
        perror("Failed to create temporary output file");
        exit(1);
    }

    // Header of generated C file
    fprintf(f, "#include <stdio.h>\n");
    fprintf(f, "\nint main() {\n");
    fprintf(f, "    printf(\"CookLang++ Executable for %s\\n\");\n", program->data.program.name);
    
    // Header for Table
    fprintf(f, "    printf(\"%%-20s %%-20s\\n\", \"CookLang++\", \"Codigo Maquina\");\n"); // machine code
    // Iterate steps to print the "Table" part of the requirement (4.4)
    // Requirement says:
    // CookLang++         Código máquina
    // mix eggs for 30s   MIX eggs 30
    
    for (int i = 0; i < program->data.program.step_count; i++) {
        Node *step = program->data.program.steps[i];
        
        // Reconstruct CookLang string
        char original[256];
        char params[128] = "";
        
        if (step->data.step.has_time) {
            snprintf(params, 128, " for %.0f%s", step->data.step.time_val, step->data.step.time_unit);
        } else if (step->data.step.has_temp) {
             snprintf(params, 128, " at %.0f%s", step->data.step.temp_val, step->data.step.temp_unit);
        }
        
        snprintf(original, 256, "%s %s%s", step->data.step.action, step->data.step.object, params);

        // Reconstruct "Machine Code"
        char action_upper[32];
        to_upper(action_upper, step->data.step.action);
        
        char mach[256];
        if (step->data.step.has_time) {
             snprintf(mach, 256, "%s %s %.0f", action_upper, step->data.step.object, step->data.step.time_val);
        } else if (step->data.step.has_temp) {
             snprintf(mach, 256, "%s %s %.0f", action_upper, step->data.step.object, step->data.step.temp_val);
        } else {
             snprintf(mach, 256, "%s %s", action_upper, step->data.step.object); // e.g. SERVE dish
             // NOTE: Example showed "SERVE" for "serve dish". 
             // "serve dish -> SERVE"
             if (strcmp(action_upper, "SERVE") == 0) {
                 strcpy(mach, "SERVE");
             }
        }

        fprintf(f, "    printf(\"%%-20s %%-20s\\n\", \"%s\", \"%s\");\n", original, mach);
    }
    
    fprintf(f, "    printf(\"\\n--- Execution Trace ---\\n\");\n");

    // Execution Trace Logic (4.5)
    for (int i = 0; i < program->data.program.step_count; i++) {
        Node *step = program->data.program.steps[i];
        char action_upper[32];
        to_upper(action_upper, step->data.step.action);

        // Print command
        if (step->data.step.has_time) {
            char prog[64];
            get_verb_progressive(prog, step->data.step.action);
            fprintf(f, "    printf(\"> %s %s %.0f\\n\");\n", action_upper, step->data.step.object, step->data.step.time_val);
            fprintf(f, "    printf(\"%s %s for %.0f seconds...\\n\");\n", prog, step->data.step.object, step->data.step.time_val); 
            // NOTE: "Mixing" hardcoded? we need present continuous mapping: mix->Mixing, heat->Heating, bake->Baking
            // Simplification: just printf verb + ing.
            
        } else if (step->data.step.has_temp) {
            char prog[64];
            get_verb_progressive(prog, step->data.step.action); // e.g. "Heating" or "Baking" if temp used
            fprintf(f, "    printf(\"> %s %s %.0f\\n\");\n", action_upper, step->data.step.object, step->data.step.temp_val);
            fprintf(f, "    printf(\"%s %s to %.0f%s...\\n\");\n", prog, step->data.step.object, step->data.step.temp_val, step->data.step.temp_unit);
        } else {
            // Serve
            if (strcmp(action_upper, "SERVE") == 0) {
                 fprintf(f, "    printf(\"> SERVE\\n\");\n");
                 fprintf(f, "    printf(\"Dish served!\\n\");\n");
             } else {
                 fprintf(f, "    printf(\"> %s %s\\n\");\n", action_upper, step->data.step.object);
                 
                 // Get progressive text
                 char prog[64];
                 get_verb_progressive(prog, step->data.step.action);
                 fprintf(f, "    printf(\"%s %s...\\n\");\n", prog, step->data.step.object);
             }
        }
    }

    fprintf(f, "    return 0;\n");
    fprintf(f, "}\n");
    fclose(f);

    // Compile
    char command[512];
    // output_name usually is without extension or with .exe
    // Let's assume output_name is "tortilla" (no ext) or inputs.
    // User requested: `$ cook tortilla.cooklang++` -> generates `tortilla` (executable)
    
    // We should strip extension from input for the output name if passed "tortilla"
    snprintf(command, 512, "gcc temp_out.c -o %s", output_name);
    printf("Compiling generated code: %s\n", command);
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Error: Compilation of generated C code failed.\n");
    } else {
        printf("Successfully created executable: %s\n", output_name);
        // remove("temp_out.c"); // Keep for debug? Or remove.
    }
}
