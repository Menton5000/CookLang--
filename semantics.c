#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "semantics.h"

static int is_valid_time_unit(const char *u) {
    return (strcmp(u, "s") == 0 || strcmp(u, "m") == 0 || 
            strcmp(u, "min") == 0 || strcmp(u, "h") == 0);
}

static int is_valid_temp_unit(const char *u) {
    return (strcmp(u, "C") == 0 || strcmp(u, "F") == 0);
}

int check_semantics(Node *program) {
    if (program->type != NODE_PROGRAM) return 1;

    // 1. Gather declared ingredients
    int ing_count = program->data.program.ingredient_count;
    char **declared_ingredients = malloc(sizeof(char*) * ing_count);
    
    for (int i = 0; i < ing_count; i++) {
        declared_ingredients[i] = program->data.program.ingredients[i]->data.ingredient.name;
    }

    int errors = 0;

    // 2. Check steps
    for (int i = 0; i < program->data.program.step_count; i++) {
        Node *step = program->data.program.steps[i];
        
        // Check ingredient existence (object of the action)
        // NOTE: The object might be "mixture" or "dish" which are created, not initial ingredients.
        // For strict checking, we'd need to track produced items. 
        // For this simple case, we'll check if it's in ingredients OR if it's a known intermediate like "mixture", "dish".
        // Or strictly: "mixture" and "dish" are valid implied objects.
        
        char *obj = step->data.step.object;
        int found = 0;
        for (int j = 0; j < ing_count; j++) {
            if (strcmp(obj, declared_ingredients[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            // Allow magic words for now as per example "mixture", "dish"
            if (strcmp(obj, "mixture") != 0 && strcmp(obj, "dish") != 0) {
                 fprintf(stderr, "Semantic Error: Ingredient/Object '%s' used in step but not declared.\n", obj);
                 errors++;
            }
        }

        // Check units
        if (step->data.step.has_time) {
            if (!is_valid_time_unit(step->data.step.time_unit)) {
                fprintf(stderr, "Semantic Error: Invalid time unit '%s'.\n", step->data.step.time_unit);
                errors++;
            }
        }
        if (step->data.step.has_temp) {
            if (!is_valid_temp_unit(step->data.step.temp_unit)) {
                fprintf(stderr, "Semantic Error: Invalid temp unit '%s'.\n", step->data.step.temp_unit);
                errors++;
            }
        }
    }

    free(declared_ingredients);
    return errors;
}
