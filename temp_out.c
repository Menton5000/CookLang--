#include <stdio.h>

int main() {
    printf("CookLang++ Executable for Tortilla\n");
    printf("%-20s %-20s\n", "CookLang++", "Codigo Maquina");
    printf("%-20s %-20s\n", "mix eggs for 30s", "MIX eggs 30");
    printf("%-20s %-20s\n", "heat oil at 180C", "HEAT oil 180");
    printf("%-20s %-20s\n", "bake mixture for 2min", "BAKE mixture 2");
    printf("%-20s %-20s\n", "serve dish", "SERVE");
    printf("\n--- Execution Trace ---\n");
    printf("> MIX eggs 30\n");
    printf("Mixing eggs for 30 seconds...\n");
    printf("> HEAT oil 180\n");
    printf("Heating oil to 180C...\n");
    printf("> BAKE mixture 2\n");
    printf("Baking mixture for 2 seconds...\n");
    printf("> SERVE\n");
    printf("Dish served!\n");
    return 0;
}
