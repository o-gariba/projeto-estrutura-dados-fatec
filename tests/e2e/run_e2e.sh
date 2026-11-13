#!/usr/bin/env bash
set -euo pipefail

BIN="${1:-bin/rede}"
CASOS_DIR="tests/e2e/casos"

if [ ! -x "$BIN" ]; then
    echo "Erro: binario '$BIN' nao encontrado ou nao executavel." >&2
    exit 1
fi

total=0
passaram=0
falharam=0

echo ""
echo "=== Executando Testes End-to-End (E2E) ==="

for entrada in "$CASOS_DIR"/*.in; do
    nome="$(basename "$entrada" .in)"
    esperado="$CASOS_DIR/$nome.esperado"
    args_file="$CASOS_DIR/$nome.args"
    total=$((total + 1))

    if [ ! -f "$esperado" ]; then
        printf "  [E2E] %-35s [FALHA: falta %s]\n" "$nome" "$esperado"
        falharam=$((falharam + 1))
        continue
    fi

    extra_args=()
    if [ -f "$args_file" ]; then
        read -r -a extra_args < "$args_file"
    fi

    saida_tmp="$(mktemp)"
    "$BIN" "${extra_args[@]}" < "$entrada" > "$saida_tmp"

    if diff -u "$esperado" "$saida_tmp"; then
        printf "  [E2E] %-35s [OK]\n" "$nome"
        passaram=$((passaram + 1))
    else
        printf "  [E2E] %-35s [FALHA]\n" "$nome"
        falharam=$((falharam + 1))
    fi

    rm -f "$saida_tmp"
done

echo "----------------------------------------"
echo "Total E2E: $total | Passaram: $passaram | Falharam: $falharam"
echo "----------------------------------------"

if [ "$falharam" -ne 0 ]; then
    exit 1
fi
