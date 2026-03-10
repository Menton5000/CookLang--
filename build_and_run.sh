#!/bin/bash

# 1. Compilar el compilador CookLang++ (cook)
# Se compilan todos los módulos: main, lexer, parser, semantics y codegen
echo "Compilando el compilador CookLang++..."
gcc -o cook main.c lexer.c parser.c semantics.c codegen.c

# Verificar si hubo error en la compilación
if [ $? -ne 0 ]; then
    echo "Error al compilar el compilador."
    exit 1
fi
echo "Compilador 'cook' generado con éxito."

# 2. Usar 'cook' para compilar la receta (tortilla.cooklang++)
# Esto generará el código C intermedio y lo compilará a tortilla.exe
echo "Compilando la receta 'tortilla.cooklang++'..."
./cook tortilla.cooklang++

if [ $? -ne 0 ]; then
    echo "Error al compilar la receta."
    exit 1
fi
echo "Receta compilada a 'tortilla.exe'."

# 3. Ejecutar el programa final (tortilla.exe)
echo "Ejecutando la receta generada..."
echo "-----------------------------------"
./tortilla.exe
echo "-----------------------------------"


# Para ejecutar el script pon en consola:
# ./build_and_run.sh
