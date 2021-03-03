# c-ompiler

Analizador léxico y sintáctico para una gramática de expresiones aritméticas
para la asignatura Diseño de Compiladores.

Creado por: Jhan Sebastián Álvarez

## Uso

```bat
> type test.txt
1 + 2 * 3 ^ (4 * 5) > 6 ^ (7 / 8) - 9 & x <= y ^ (11 * (z - 3))
> c-ompiler test.txt
Analizando test.txt
--------------------------------
test.txt: Inicio procedimiento 1
test.txt: Pila -> {1, 0}
test.txt: Token -> `1` (tipo 1)
test.txt: Pila -> {3, 2, 0}
test.txt: Fin procedimiento 1
--------------------------------
test.txt: Inicio procedimiento 4
test.txt: Pila -> {3, 2, 0}
...
test.txt: Fin procedimiento 6
--------------------------------
test.txt: Inicio procedimiento 3
test.txt: Pila -> {2, 0}
test.txt: Fin de secuencia
test.txt: Pila -> {0}
test.txt: Fin procedimiento 3
--------------------------------
test.txt: Secuencia aceptada
--------------------------------
```
