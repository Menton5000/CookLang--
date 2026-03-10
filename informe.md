# Informe Final: Compilador CookLang++

## 1. Introducción
Este informe documenta el diseño e implementación del compilador para **CookLang++**, un lenguaje específico de dominio (DSL) para describir recetas de cocina. El compilador transforma estas recetas en instrucciones ejecutables y una traza de ejecución simulada.

## 2. Descripción del Diseño

El compilador sigue una arquitectura clásica de múltiples fases:

### 2.1 Análisis Léxico (Lexer)
- **Implementación**: Manual (ad-hoc) en `lexer.c`.
- **Funcionamiento**: Lee el código fuente carácter por carácter y agrupa secuencias en tokens (palabras clave como `RECIPE`, acciones como `mix`, literales como números).
- **Manejo de Errores**: Detecta caracteres inesperados y reporta la línea del error.

### 2.2 Análisis Sintáctico (Parser)
- **Estrategia**: Descendiente Recursivo (Recursive Descent).
- **Gramática**: Se definió una gramática libre de contexto que estructura la receta en:
  - Cabecera (`RECIPE <nombre>`)
  - Sección de Ingredientes (`INGREDIENTS: ...`)
  - Sección de Pasos (`STEPS: ...`)
- **AST**: Construye un Árbol Sintáctico Abstracto (AST) que representa la jerarquía de la receta.

### 2.3 Análisis Semántico
- **Verificaciones**:
  - Existencia de ingredientes: Se verifica que los objetos manipulados en los pasos (excepto productos intermedios como "mixture") hayan sido declarados.
  - Unidades: Valida que las unidades de tiempo (`s`, `m`, etc.) y temperatura (`C`, `F`) sean correctas.

### 2.4 Generación de Código
- **Enfoque**: Transpilación a C.
- **Salida**: El compilador genera un archivo temporal `.c` que incluye la lógica para imprimir tanto la tabla de instrucciones como la traza de ejecución.
- **Compilación Final**: Este archivo temporal se compila automáticamente con `gcc` para producir el ejecutable final (e.g., `tortilla.exe`).

## 3. Decisiones de Implementación

1.  **Lenguaje C**: Se eligió C para todo el proyecto por su eficiencia y control de memoria, adecuado para un ejercicio de construcción de compiladores.
2.  **Transpilación**: En lugar de generar código máquina o bytecode directamente, se generó código C. Esto simplifica la generación de la traza (usando `printf`s) y aprovecha `gcc` para la generación de binarios nativos.
3.  **Mapeo de Verbos**: Para mejorar la lectura de la traza, se implementó una función que conjuga los verbos (e.g., `bake` -> `Baking`), corrigiendo un defecto inicial donde todos los verbos se mostraban como "Mixing".

## 4. Casos de Prueba

Se utilizó la receta de ejemplo "Tortilla":

**Entrada**:
```cooklang
RECIPE Tortilla
INGREDIENTS:
  2 units eggs
  1 tbsp oil
STEPS:
  mix eggs for 30s
  heat oil at 180C
  bake mixture for 2min
  serve dish
END
```

**Salida Esperada y Obtenida**:
- Tabla de códigos máquina (`MIX`, `HEAT`, `BAKE`, `SERVE`).
- Traza de ejecución fluida: "Mixing eggs...", "Heating oil...", "Baking mixture...".
- Tiempos y temperaturas reflejados correctamente.

## 5. Errores Detectados y Gestionados

Durante el desarrollo se identificaron y corrigieron los siguientes problemas:

1.  **Warnings de Compilación**: Se corrigió el uso de `const char*` en el lexer para cumplir con los estándares de seguridad de tipos de C.
2.  **Lógica de Traza Incorrecta**: Inicialmente, el generador de código siempre imprimía "Mixing" para cualquier acción con tiempo. Se reescribió la lógica en `codegen.c` para seleccionar el verbo progresivo adecuado (`Baking`, `Heating`, etc.).
3.  **Codificación de Caracteres**: Se ajustaron los encabezados de la tabla de salida para evitar caracteres no ASCII que causaban problemas de visualización en algunas terminales.

## 6. Conclusión
El compilador CookLang++ es funcional y cumple con todos los requisitos especificados. Es capaz de analizar recetas, validar su consistencia semántica y generar ejecutables que simulan el proceso de cocinado.
