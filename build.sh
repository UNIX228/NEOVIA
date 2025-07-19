#!/bin/bash

# NEOVIA Build Script
# Автоматическая сборка NRO приложения для Nintendo Switch

echo "🟪 NEOVIA Build Script v1.0.0"
echo "================================"

# Проверка переменных окружения devkitPro
if [ -z "$DEVKITPRO" ]; then
    echo "❌ Ошибка: DEVKITPRO не установлен!"
    echo "Установите devkitPro и настройте переменные окружения:"
    echo "export DEVKITPRO=/opt/devkitpro"
    echo "export DEVKITARM=/opt/devkitpro/devkitARM"
    echo "export DEVKITPPC=/opt/devkitpro/devkitPPC"
    exit 1
fi

echo "✅ devkitPro найден: $DEVKITPRO"

# Проверка зависимостей
echo "🔍 Проверка зависимостей..."

if [ ! -d "$DEVKITPRO/libnx" ]; then
    echo "❌ Ошибка: libnx не найден!"
    echo "Установите libnx через pacman:"
    echo "sudo dkp-pacman -S switch-dev"
    exit 1
fi

echo "✅ libnx найден"

# Проверка libcurl
if [ ! -f "$DEVKITPRO/portlibs/switch/lib/libcurl.a" ]; then
    echo "⚠️  Предупреждение: libcurl не найден, устанавливаю..."
    sudo dkp-pacman -S switch-curl --noconfirm
fi

# Проверка jsoncpp
if [ ! -f "$DEVKITPRO/portlibs/switch/lib/libjsoncpp.a" ]; then
    echo "⚠️  Предупреждение: jsoncpp не найден, устанавливаю..."
    sudo dkp-pacman -S switch-jsoncpp --noconfirm
fi

echo "✅ Все зависимости проверены"

# Очистка предыдущей сборки
echo "🧹 Очистка предыдущей сборки..."
make clean

# Сборка проекта
echo "🔨 Сборка NEOVIA..."
make -j$(nproc)

# Проверка результата
if [ -f "NEOVIA.nro" ]; then
    echo "✅ Сборка успешно завершена!"
    echo "📁 Файл создан: NEOVIA.nro"
    
    # Получение размера файла
    SIZE=$(du -h NEOVIA.nro | cut -f1)
    echo "📊 Размер файла: $SIZE"
    
    # Создание папки для релиза
    mkdir -p release
    cp NEOVIA.nro release/
    
    echo ""
    echo "🎮 Инструкции по установке:"
    echo "1. Скопируйте NEOVIA.nro в папку /switch/ на SD-карте"
    echo "2. Запустите через Homebrew Menu"
    echo "3. При первом запуске произойдет автоматическая настройка"
    echo ""
    echo "🟪 NEOVIA готов к использованию!"
else
    echo "❌ Ошибка сборки!"
    echo "Проверьте логи выше для диагностики проблем"
    exit 1
fi