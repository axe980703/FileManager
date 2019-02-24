#include <stdlib.h>
#include <memory.h>

// Файловый менеждер осуществляет операции с файлами и директориями.
// Оперирует с виртуальным деревом каталогов и файлов, каждый файл или директория имеют имя,
// состоящее из латинских прописных и заглавных букв, цифр и символов '_' и '.'. Длина имени
// Считаем, что директории размера не имеют, и их можно создавать в любом количестве.
// Корневая директория '/' создается автоматически и не может быть удалена.
// Для идентификации файла или директории используется путь, состоящий из имен вложенных директорий,
// содержащих данный файл, и знаков '/', например /dir1/dir2/file.txt.
// Любой файл или директория доступны по относительным и абсолютным путям. Относительный путь
// считается относительно текущей директории, абсолютный путь начинается с корневой директории, т.е.
// '/', например /my_dir1/my_dir2/file.txt и my_dir2/file.txt указывают на один и тот же файл при
// условии, что /my_dir1 - текущая директория.
// При создании файлового менеджера корневая    директория устанавливается текущей.
// Кроме имен директорий в пути могут присутствовать две специальные директории '.' и '..', первая из
// которых обозначает, что путь не меняется, а вторая означает, что путь должен пройти на директорию
// вверх, например /my_dir1/my_dir2/file.txt и ../my_dir2/file.txt являются одинаковыми при условии,
// что текущая директория имеет вид /my_dir1/my_dir3.
// В связи с этим запрещается создавать файлы или директории с именами . и .. (менеджер должен выдавать
// ошибку в случае попытки создать такой файл или директорию).
// При удалении или перемещении текущей директории корень становится текущей директорией

typedef struct
{
    // Создание файлового менеджера,
    // disk_size - размер диска
    // Возвращаемое значение: 1 - успех, 0 - неудача
    int (*create)(int disk_size);

    // Удаление файлового менеджера
    // Структура директорий, созданная на данный момент полностью удаляется
    // Возвращаемое значение: 1 - успех, 0 - неудача (например, менеждер не был создан)
    int (*destroy)();

    // Создает директорию с указанным именем.
    // В качестве аргумента принимает абсолютный или относительный путь
    // Создание директории возможно только внутри уже существующей директории
    // Возвращаемое значение: 1 - успех, 0 - неудача (например, менеждер не был создан)
    int (*create_dir)(const char* path);

    // Создает файл с указанным именем и размером.
    // В качестве аргумента принимает абсолютный или относительный путь
    // Создание файла возможно только внутри уже существующей директории
    // Возвращаемое значение: 1 - успех, 0 - неудача (например, не хватило места)
    int (*create_file)(const char* path, int file_size);

    // Удаляет файл или директорию с указанным именем.
    // В качестве аргумента принимает абсолютный или относительный путь
    // Если указана непустая директория и флаг recursive равен 0, то директория не удаляется и выдается ошибка.
    // Если указана директория и флаг recursive равен 1, то директория удаляется вместе со всем ее содержимым.
    // Если получилось так, что текущая директория была удалена - текущей становится корневая директория.
    // Возвращаемое значение: 1 - успех, 0 - неудача
    int (*remove)(const char* path, int recursive);

    // Меняет текущую директорию
    // В качестве аргумента принимает абсолютный или относительный путь новой текущей директории
    // Возвращаемое значение: 1 - успех, 0 - неудача
    int (*change_dir)(const char* path);

    // Возвращает полный путь текущей директории (кладет в буфер dst)
    void (*get_cur_dir)(char *dst);

    // Проверяет существование файла или директории.
    // В качестве аргумента принимает абсолютный или относительный путь
    // Возвращаемое значение: 1 - файл или директория существует, 0 - нет
    int (*exists)(const char* path);

    // Проверяет, является ли указанное имя файлом
    // Возвращаемое значение: 1 - файл, 0 - директория или нет такого пути
    int (*is_file)(const char* path);

    // Проверяет, является ли указанное имя файлом
    // Возвращаемое значение: 1 - директория, 0 - файл или нет такого пути
    int (*is_dir)(const char* path);

    // Для директорий выводит на экран содержимое директорий,
    // Для файлов - их путь (в том виде, в каком он подан на вход функции).
    // формат вывода директорий:
    // <путь к директории>: <содержимое директории в алфавитном порядке>
    // здесь имя директории - в точности тот путь, что был подан на вход функции.
    // Если установлен флаг dir_first, то сначала выводит все поддиректории,
    // а потом файлы.
    // При выводе содержимого нескольких директорий или имен нескольких файлов с помощью
    // входного шаблона * выводит их в алфавитном порядке.
    // Возвращаемое значение: 1 - успех, 0 - неудача
    int (*list)(const char* path, int dir_first);

    // Выводит на экран содержимое директории и всех поддиректорий. Обход дерева - в глубину, т.е. сначала выводится
    // содержимое верхней директории, потом содержимое первой поддиректории, потом содержимое первой поддиректории в ней
    // и т.д., когда доходим до директории, в которой больше нет поддиректорий, переходим к следующей (по алфавиту)
    // директории на этом же уровне и т.д.
    // Пример последовательности обхода директорий:
    // /
    // /a
    // /a/1
    // /b
    // /b/1
    // /b/1/1
    // /b/a
    // /с
    // Формат вывода для каждой из обходимых директорий:
    // <путь к директории>: <содержимое директории в алфавитном порядке>
    // Внутри каждой директории поддиректории обходятся и выводятся в алфавитном порядке.
    // Если указан флаг dir_first, сначала выводятся директории, а потом файлы.
    // Возвращаемое значение: 1 - успех, 0 - неудача
    int (*list_recursive)(const char* path, int dir_first);

    // Поиск файла или директории с указанным именем name в указанной директории path (и всех ее поддиректориях).
    // Выводит на экран все найденные файлы и директории с данным именем.
    // Обход производится аналогично функции list_recursive
    // Возвращаемое значение: количество найденных файлов - успех, -1 - неудача (неверный путь)
    int (*find)(const char *path, const char *name);

    // Перемещает (переименовывает) файл или директорию.
    // При попытке переместить файл или каталог в самого себя должен выдавать ошибку.
    // При попытке переименовать/переместить файл или каталог в существующий файл или каталог выдается ошибка.
    // Если получилось так, что текущая директория была перемещена, - текущей становится корневая директория
    // Возвращаемое значение: 1 - успех, 0 - неудача
    int (*move)(const char *old_path, const char *new_path);

    // Копирует файл или директорию в другое место.
    // При попытке скопировать файл или каталог в самого себя должен выдавать ошибку (-1)
    // При попытке скопировать в файл или несуществующую директорию выдавать ошибку
    // Возвращаемое значение: количество скопированных файлов - успех, -1 - неудача
    int (*copy)(const char *path, const char *to_path);

    // Вычисляет свободное пространство на диске в байтах
    int (*free_space)();

    // Подсчитывает размер директории или файла.
    // Размер директории - суммарный размер всех файлов в этой директории (в том числе и во вложенных поддиректориях)
    // Возвращаемое значение: размер файла или директории в байтах - успех, -1 - неудача
    int (*size)(const char *path);

    // Подсчитывает количество файлов в директории (в том числе и во вложенных поддиректориях)
    // Возвращаемое значение: количество файлов - успех, -1 - неудача
    int (*files_count)(const char *path);

} file_manager_t;

void setup_file_manager(file_manager_t *fm);
