# client

Программа клиент запрашивает у сервера информацию о компьютере на котором он (сервер) запущен. Программа сервер присылает ответ в виде зашифрованной последовательности байт алгоритмом AES128 (моя реализация https://github.com/vasko-da-gama/My-AES128). Клиент запрашивает маску для ключа шифрования, которую использовал сервер перед шифрованием.

## пример вывода

```
waiting.
Recieve 256 bytes from server
##### Message Begin #####


2 : disks found
[C:\] free space: 39.548401 gb          | DRIVE_FIXED
[D:\] free space: 547.177795 gb         | DRIVE_FIXED
[F:\] free space: 8.807281 gb           | DRIVE_REMOVABLE
Windows 10
19% of global memory is loaded
Physical RAM: 15.951569 gb
12.08.2019 01:26:59
##### Message End #####
```
