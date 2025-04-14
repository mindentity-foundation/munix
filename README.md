# MUNIX

MUNIX (Mindentity Unix) - Is a small OS microkernel, that follows UNIX philosophy.


## Plans

Our development team has many plans on this kernel. The main one's are:
- Module and Driver support.
- FAT16/ext2 Filesystem support.
- ELF Executable format support.
- Own Hand-written and lovely made Coreutils.
- User-space Services.
- 

Не так багато, як хотілося б. Ось що вже готово:
- Невеликий VGA драйвер.
- Невелика бібліотека ядра (знаходиться в директорії `lib`)
- Підтримка взаємодії з портами. (COM-порти також підтримуються)
- Невелика підтримка TTY.
- Вивід тексту на екран. (з форматуванням!)


## Як мені це чудо скомпілювати?

Вам в любому випадку пригодиться [i686-elf-tools](https://github.com/lordmilko/i686-elf-tools). (Інструкція по встановленню там є)

Після того, як ви встановили `i686-elf-tools`, вам потрібно буде викликати скріпт в головній дерикторії цього репозиторія, який називається `build.sh`

```
chmod +x ./build.sh  # На всякий випадок

./build.sh     # Скомпілює бінарник ядра
./build.sh -c  # ЛИШЕ очистить директорію 'build'
./build.sh -t  # Скомпілює, та запустить бінарник ядра у QEMU. (Якщо у він вас є, і в нього є підтримка x86)
```


## Чому назва така тупа?

Я не вмію вигадувати гарні назви для чогось. Банально подивіться на мій нік.


## Куди я можу звернутись, щоб ви додали якусь фічу до ядра?

Всі мої соц.мережи (можливо) указані в моєму профілі GitHub/Codeberg. Але я продублюю їх тут:
- BlueSky - @voison.bsky.social
- Discord - @voisonlmao
- Telegram (Канал) - t.me/voisonsboard

Також, можливо в (недалекому) майбутньому, я зроблю або телеграм бота, або сервіс для ваших запропонувань по поводу ядра.

###### P.S. Не пишіть мені, якщо ви хочете потролити мене, ви відразу попадете до блоку.

