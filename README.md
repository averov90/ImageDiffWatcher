# Image Difference Watcher
[![License](https://img.shields.io/badge/LICENSE-The%20Unlicense-green?style=flat-square)](/LICENSE)  [![Version](https://img.shields.io/badge/VERSION-DEBUG%20--%20demonstrable-yellow?style=flat-square)](https://github.com/averov90/ImageDiffWatcher/releases/tag/1.0)
#### Находит отличия между статичным изображением и потоком с камеры в цветовом канале (с фильтрацией шума).
Данным кодом реализован поиск отличий текущего изображения (сохранённого) с каждым изображением из потока.
Общий смысл кода в том, чтобы найти значимые отличия между статичной картинкой и секвенцией (результаты поиска отличий выводятся).
Для более качественного сравнения в коде используется несколько алгоритмов, улучшающих результаты. А именно: 2 вида размытий, маска теней, маска бликов, маска шума, порог отлова отличий (на точечные отличия срабатывания не будет). Все паски работают в прогрессивном режиме (применяются к нескольким кадрам и утчитывают их показатели).
Важно упомянуть, что программа ищет различия именно в цветовой составляющей (это сделано для уменьшения влияния яркости освещения). 
Как известно, канал цвета довольно шумный, поэтому алгоритмы подавления шумов тут полезны. Алгоритмы, используемые в программе, имеют настройки (их можно изменить через окно консоли).

Вы можете посмотреть на этот код в работе - демонстрационная версия есть в release. Код в данном репозитории рассчитан на применение его (или принципов его работы) в других проектах.

##### За основу взята библиотека OpenCV версии 3.4.1
