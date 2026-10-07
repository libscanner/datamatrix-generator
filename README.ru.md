[English](https://github.com/libscanner/datamatrix-generator/blob/main/README.md) | **Русский**

# Библиотека создания DataMatrix (dmgen)

Генерация символов DataMatrix ECC 200 по ГОСТ Р ИСО/МЭК 16022 с проверкой GS1 и профиля «Честного знака» — PNG и JPEG для печати маркировки.

`dmgen` — коммерческая библиотека с закрытым исходным кодом: C++20 API, плоский C ABI, пакет для Python и утилита командной строки. Поддерживает все 24 квадратных размера, шесть схем кодирования (ASCII, C40, Text, X12, EDIFACT, Base256) и режим GS1, проверяющий строку по общим спецификациям GS1 и профилю «Честного знака». Каждый собранный символ библиотека сама читает обратно, прежде чем вернуть, — символ, не прошедший самопроверку, не выдаётся. Публичный API не бросает исключений: всё, что может не получиться, возвращает `Result<T>`.

В этом репозитории — публичные заголовки, примеры, ссылки на документацию и тексты лицензий. **Исходного кода библиотеки здесь нет.** Собранные библиотеки приложены к [выпускам на GitHub](https://github.com/libscanner/datamatrix-generator/releases/latest).

## Ключевые цифры

| | |
|---|---|
| **24 размера** | все квадратные символы ГОСТ, от 10×10 до 144×144 |
| **0,26 мс** | код и PNG символа 10×10; 144×144 — 7,3 мс |

Попробовать без установки: <https://libscanner.com/generate>

## Платформы

| Платформа | Статическая библиотека C++ + C ABI + утилита | Колесо Python |
|---|---|---|
| Windows x64 | `dmgen-1.0.0-windows-x64.zip` | `dmgen-1.0.0-py3-none-win_amd64.whl` |
| Linux x86-64 | `dmgen-1.0.0-linux-x64.tar.gz` | `dmgen-1.0.0-py3-none-manylinux_2_36_x86_64.whl` |
| Linux ARM64 (Raspberry Pi 3/4/5, 64 бит) | `dmgen-1.0.0-linux-arm64.tar.gz` | `dmgen-1.0.0-py3-none-manylinux_2_36_aarch64.whl` |
| Linux ARMv7 (Raspberry Pi 2/3/4/5, 32 бит) | `dmgen-1.0.0-linux-armhf.tar.gz` | `dmgen-1.0.0-py3-none-manylinux_2_36_armv7l.whl` |

Для каждой платформы есть и архив только с утилитой `dmgen` (`…-cli.zip` / `…-cli.tar.gz`).

- Сборки под Windows сделаны MinGW-w64 GCC (MSYS2, оболочка MINGW64). Из MSVC и других компиляторов — через C ABI (`dmgen_c.dll`).
- Сборки под Linux сделаны GCC 12 на Debian 12 (glibc 2.36): для C++ API нужен GCC 12 или новее; колёсам — glibc 2.36+ (Debian 12 / Raspberry Pi OS Bookworm и новее).
- Python 3.8 и новее, без зависимостей. Сторонний код вкомпонован в поставку, внешних зависимостей нет.

## Загрузка

- **Сборки:** [выпуски на GitHub → последний](https://github.com/libscanner/datamatrix-generator/releases/latest) — архивы для каждой платформы и колёса Python.
- **Python:** [PyPI → libscanner-dmgen](https://pypi.org/project/libscanner-dmgen/) — `pip install libscanner-dmgen`, импорт — `dmgen`.
- **Лицензии:** тарифы на <https://libscanner.com/> — год или бессрочно (<https://libscanner.com/products/datamatrix-generator-yearly>, <https://libscanner.com/products/datamatrix-generator-perpetual>). Ключ активации появляется в личном кабинете после покупки.

### Пробный период

Любая сборка работает **30 дней с первого запуска на машине** — без регистрации, без ключа и без обращения к сети. Дальше нужен ключ лицензии с <https://libscanner.com>; без него `encode()` возвращает `ErrorCode::Unlicensed`. Проверка GS1 и рисование уже готовой матрицы лицензии не требуют.

## Состав поставки

```text
include/dmgen/{Dmgen,Encoder,Gs1,Image,License,Options,Result,Symbol,Version}.h   C++ API
include/dmgen/dmgen_c.h                     C ABI
lib/libdmgen.a                              статическая библиотека C++
lib/pkgconfig/dmgen.pc
lib/libdmgen_c.so.1 | bin/dmgen_c.dll       библиотека C ABI (+ lib/dmgen_c.dll.a под Windows)
bin/dmgen                                   утилита командной строки
share/doc/dmgen/licenses/                   лицензии стороннего кода
```

Заголовки в [`include/`](https://github.com/libscanner/datamatrix-generator/tree/main/include/dmgen) этого репозитория побайтно совпадают с заголовками поставки 1.0.0.

## Быстрый старт

### C++

```cpp
#include <dmgen/Dmgen.h>
#include <iostream>

int main() {
    dmgen::EncodeOptions o;
    o.gs1 = dmgen::Gs1Mode::ChestnyZnak;          // проверка и FNC1 первым кодовым словом

    auto sym = dmgen::encode("(01)04601234567893(21)5Abc12Xyz!-(93)dGVz", o);
    if (!sym) {
        std::cerr << sym.error().message << "\n";   // по-русски
        return 1;
    }

    dmgen::RenderOptions r;                        // 8 px на модуль, свободная зона 2
    auto saved = dmgen::save(dmgen::render(sym->modules, r), "code.png", dmgen::ImageFormat::Png);
    return saved ? 0 : 1;
}
```

Статическая линковка через pkg-config — запрос **обязательно** с `--static`:

```sh
PKG_CONFIG_PATH=/путь/к/dmgen-1.0.0-linux-x64/lib/pkgconfig pkg-config --static --cflags --libs dmgen
```

В Meson:

```meson
dmgen_dep = dependency('dmgen', required: true, static: true)
executable('myprogram', 'main.cpp', dependencies: [dmgen_dep],
  link_args: ['-static'])   # Windows: рантайм MinGW внутрь программы
```

Полный пример: [`examples/cpp`](https://github.com/libscanner/datamatrix-generator/tree/main/examples/cpp). Пример на C ABI (MSVC, другие языки): [`examples/c`](https://github.com/libscanner/datamatrix-generator/tree/main/examples/c).

### Python

```sh
pip install libscanner-dmgen     # PyPI: Windows x64, Linux x86-64 / ARM64 / ARMv7 (glibc 2.36+)
```

```python
import dmgen

# код «Честного знака» → файл; формат — по расширению
dmgen.save("(01)04601234567893(21)5Abc12Xyz!-(93)dGVz", "code.png", gs1="cz")

# картинка в памяти
png = dmgen.make_image("Hello", module_px=8)
jpg = dmgen.make_image("Hello", fmt="jpeg", quality=100)
```

Колесо ставит и команду `dmgen`. Пример: [`examples/python`](https://github.com/libscanner/datamatrix-generator/tree/main/examples/python).

### Командная строка

```sh
dmgen "0104601234567893215Abc12Xyz!-<GS>93dGVz" --gs1=cz -o code.png
dmgen --check-gs1 --gs1=cz "(01)04601234567893(21)5Abc12Xyz!-(93)dGVz"
```

Другие команды: [`examples/cli`](https://github.com/libscanner/datamatrix-generator/tree/main/examples/cli).

## Документация

- C++ / C ABI / утилита: <https://libscanner.com/docs/datamatrix-generator/> (English: <https://libscanner.com/en/docs/datamatrix-generator/>)
- Python: <https://libscanner.com/docs/datamatrix-generator/?lang=python> (English: <https://libscanner.com/en/docs/datamatrix-generator/?lang=python>)

## Лицензия

Проприетарная — см. [LICENSE.txt](https://github.com/libscanner/datamatrix-generator/blob/main/LICENSE.txt) (лицензионное соглашение с пользователем; юридическую силу имеет русский текст, [LICENSE.en.txt](https://github.com/libscanner/datamatrix-generator/blob/main/LICENSE.en.txt) — перевод на английский для сведения). Начало использования ПО означает согласие с соглашением.

Сторонние компоненты: [THIRD-PARTY-NOTICES.ru.txt](https://github.com/libscanner/datamatrix-generator/blob/main/THIRD-PARTY-NOTICES.ru.txt) (на английском — [THIRD-PARTY-NOTICES.txt](https://github.com/libscanner/datamatrix-generator/blob/main/THIRD-PARTY-NOTICES.txt)) и тексты лицензий в [`licenses/`](https://github.com/libscanner/datamatrix-generator/tree/main/licenses).

Код в [`examples/`](https://github.com/libscanner/datamatrix-generator/tree/main/examples) — под лицензией MIT-0 ([examples/LICENSE](https://github.com/libscanner/datamatrix-generator/blob/main/examples/LICENSE)): копируйте в свои проекты свободно; сама библиотека — проприетарная (LICENSE.txt).

## Связь

- Почта: [libscanner@yandex.com](mailto:libscanner@yandex.com)
- Форма обратной связи: <https://libscanner.com/feedback>
- Уязвимости: см. [SECURITY.md](https://github.com/libscanner/datamatrix-generator/blob/main/SECURITY.md)

«GS1» — товарный знак GS1 AISBL. «Честный знак» — товарный знак ООО «Оператор-ЦРПТ». Продукт не связан с владельцами этих знаков и ими не сертифицирован; названия упоминаются только для описания поддерживаемого формата.
