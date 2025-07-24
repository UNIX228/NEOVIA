#!/bin/bash

echo "🟪 NEOVIA Build Script v1.0.0"
echo "================================"

if [ -z "$DEVKITPRO" ]; then
    echo "❌ DEVKITPRO не установлен!"
    exit 1
fi

echo "🔨 Сборка NEOVIA..."
make -j$(nproc)

if [ -f "NEOVIA.nro" ]; then
    echo "✅ Сборка успешна!"
    echo "📁 Файл: NEOVIA.nro"
else
    echo "❌ Ошибка сборки!"
fi
