# CookLang++ Specification

## 1. Tokens

### Keywords
- `RECIPE`
- `INGREDIENTS`
- `STEPS`
- `END`
- `for`
- `at`

### Actions
- `chop`
- `mix`
- `heat`
- `bake`
- `boil`
- `serve`

### Literals
- `NUMBER`: Digits, optional decimal point.
- `IDENTIFIER`: Alphanumeric string start with letter.

### Separators
- `: (COLON)` - though not explicitly in example except after INGREDIENTS and STEPS, the example shows "INGREDIENTS:" and "STEPS:". I'll assume they are tokens or part of the keyword. Let's handle `INGREDIENTS:` as a single token or `INGREDIENTS` + `:`. The example has spaces. "INGREDIENTS:" looks like a label.

## 2. Grammar (BNF)

```
<program>       ::= RECIPE <identifier> <ingredients_sect> <steps_sect> END

<ingredients_sect> ::= INGREDIENTS_KW <ingredient_list>

<ingredient_list> ::= <ingredient> <ingredient_list>
                    | <ingredient>

<ingredient>    ::= <number> <identifier> <identifier> 
                    ; quantity unit ingredient_name

<steps_sect>    ::= STEPS_KW <step_list>

<step_list>     ::= <step> <step_list>
                  | <step>

<step>          ::= <action> <identifier> <time_opt> <temp_opt>

<action>        ::= CHOP | MIX | HEAT | BAKE | BOIL | SERVE

<time_opt>      ::= FOR <number> <identifier> 
                  | epsilon

<temp_opt>      ::= AT <number> <identifier> 
                  | epsilon
```

## 3. Semantics
- Ingredients used in steps must be declared in the ingredients section.
- Time units: 's', 'm', 'min', 'h'.
- Temp units: 'C', 'F'.
