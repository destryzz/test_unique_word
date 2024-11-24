# **Unique Words Extension for PostgreSQL**

## **Описание**

Расширение **Unique Words** добавляет пользовательскую функцию `count_unique_words` для PostgreSQL, позволяющую подсчитывать количество уникальных слов в строке. Функция разбивает строку на слова, игнорирует регистр символов и возвращает количество уникальных слов.

## **Основные функции**

- Подсчет уникальных слов в строке.
- Игнорирование регистра символов.
- Обработка `NULL` значений, пустых строк и строк, содержащих только пробелы.
- Простая хэш-таблица для обработки уникальности.

---

## **Установка**

### **Шаг 1: Клонирование репозитория**
Склонируйте этот репозиторий:
```bash
git clone https://github.com/destryzz/test_unique_word.git
cd unique_words
```

### **Шаг 2: Компиляция и установка**

1. Убедитесь, что установлены необходимые библиотеки PostgreSQL:
   ```bash
   sudo apt install postgresql-server-dev-14
   ```

2. Соберите расширение:
   ```bash
   make
   ```

3. Установите расширение:
   ```bash
   sudo make install
   ```

---

## **Использование**

### **Шаг 1: Подключение к базе данных**
Запустите клиент `psql` от имени пользователя PostgreSQL:
```bash
sudo -i -u postgres
psql
```

### **Шаг 2: Создание базы данных**
Создайте базу данных и подключитесь к ней:
```sql
CREATE DATABASE unique_words_db;
\c unique_words_db
```

### **Шаг 3: Установка расширения**
Установите расширение в базе данных:
```sql
CREATE EXTENSION unique_words;
```

### **Шаг 4: Использование функции**
Вызовите функцию `count_unique_words`:
```sql
SELECT count_unique_words('This is a test. This test is simple.');
-- Результат: 5

SELECT count_unique_words('Hello world Hello');
-- Результат: 2

SELECT count_unique_words(NULL);
-- Результат: 0

SELECT count_unique_words('');
-- Результат: 0
```

---

## **Примеры**

### **Пример с таблицей**
Создайте таблицу и выполните запрос:
```sql
CREATE TABLE phrases (id SERIAL PRIMARY KEY, phrase TEXT);
INSERT INTO phrases (phrase) VALUES
    ('This is a test'),
    ('Another example with unique words'),
    ('Words repeat words');

SELECT id, count_unique_words(phrase) AS unique_word_count FROM phrases;
```

### **Результат**
| ID | UNIQUE_WORD_COUNT |
|----|-------------------|
|  1 | 4                 |
|  2 | 5                 |
|  3 | 2                 |

---

## **Тестирование**

Для выполнения тестов запустите SQL-скрипт:
```bash
psql -d unique_words_db -f test.sql
```

Пример файла `test.sql`:
```sql
SELECT count_unique_words('A quick brown fox jumps over the lazy dog');
SELECT count_unique_words('Repeat repeat REPEAT');
SELECT count_unique_words('');
SELECT count_unique_words(NULL);
```

---

## **Структура проекта**

```plaintext
unique_words/
├── Makefile               # Файл для сборки расширения
├── unique_words.c         # Исходный код на C
├── unique_words.control   # Контрольный файл PostgreSQL
├── unique_words--1.0.sql  # SQL-скрипт для установки расширения
├── README.md              # Документация
├── test.sql               # SQL-скрипт для тестирования
```

---

## **Требования**

- PostgreSQL версии 14 или выше.
- Установленные библиотеки `postgresql-server-dev`.

---

## **Автор**

- Автор: Жуков Алексей
- Контакты: destryz@yandex.ru

---

## **Лицензия**

Этот проект распространяется под лицензией [MIT](LICENSE).