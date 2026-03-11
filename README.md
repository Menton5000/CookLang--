# CookLang++

**CookLang++** es un lenguaje específico de dominio (DSL) diseñado para describir recetas de cocina de forma estructurada. Este proyecto implementa un compilador completo que transforma archivos `.cooklang++` en programas ejecutables que simulan el proceso de preparación.

---

## Características Principales

- **Sintaxis Intuitiva**: Diseñada para ser leída y escrita por humanos, similar a una receta real.
- **Análisis Semántico**: Valida que los ingredientes existan y que las unidades (tiempo/temperatura) sean correctas.
- **Transpilación a C**: El compilador genera código C intermedio, aprovechando `gcc` para producir binarios nativos optimizados.
- **Trazabilidad**: Los programas generados imprimen una traza detallada de la ejecución (p. ej., "Mixing eggs for 30s...").

---

## Instalación y Requisitos

Para compilar y ejecutar este proyecto, necesitas:
- Un compilador de C (como `gcc`).
- Un entorno compatible con Bash (Linux/macOS o WSL en Windows).

### Compilar el Compilador
Puedes compilar el núcleo del sistema ejecutando:
```bash
gcc -o cook main.c lexer.c parser.c semantics.c codegen.c
```

O simplemente utiliza el script automatizado:
```bash
chmod +x build_and_run.sh
./build_and_run.sh
```

---

## Uso del Lenguaje

### Ejemplo de Receta (`tortilla.cooklang++`)
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

### Cómo Compilar una Receta
Una vez que tengas el binario `cook`, utilízalo para procesar tu receta:
```bash
./cook mi_receta.cooklang++
./mi_receta.exe
```

---

## Arquitectura del Compilador

El proyecto sigue las fases clásicas de la ingeniería de compiladores:

1.  **Lexer (`lexer.c`)**: Escaneo de caracteres para generar tokens.
2.  **Parser (`parser.c`)**: Análisis sintáctico mediante descenso recursivo para construir un AST.
3.  **Semantics (`semantics.c`)**: Verificación de tipos, unidades y declaraciones de ingredientes.
4.  **Codegen (`codegen.c`)**: Generación de código C que incluye la lógica de ejecución y visualización de trazas.

Para un análisis técnico más profundo, consulta el [Informe Final](informe.md) y las [Especificaciones](specs.md).

---

## Estructura del Proyecto

- `main.c`: Punto de entrada del compilador.
- `lexer.h/c`: Analizador léxico.
- `parser.h/c`: Analizador sintáctico.
- `semantics.h/c`: Analizador semántico.
- `codegen.h/c`: Generador de código.
- `build_and_run.sh`: Script de utilidad para pruebas rápidas.

---
