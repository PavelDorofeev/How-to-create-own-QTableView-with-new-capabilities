# How-to-create-own-QTableView-with-new-capabilities

Всем привет дорогие разработчики программ!
Сегодня мы представляем открытый проект: как сделать свой QTableView с новыми возможностями.
Нас к этому побудила необходимость выводить строки таблицы с секциями по определенному шаблону.
То есть как примерно на картинке ниже:
![image](https://github.com/PavelDorofeev/How-to-create-own-QTableView-with-new-capabilities/assets/13850002/bef1b0ad-3fed-4a04-8eed-4029994e3b68)

Чтобы реализовать свой QpTableView мы потратили примерно 5 недель на  изучение и изменение кода QTableView, но оно того стоило ибо пришлось наконец-то понять как работает отрисовка в Qt.
(Qp это префикс названий наших файлов и классов) 
Чтобы воспользоваться нашими новыми классами QpTableView, QpHorHeaderView, QpAbstractItemView QpVertHeaderView надо как обычно это делается просто добавить в папку к своему проекту на Qt.

Первое ознакомительное видео по горячим следам (первая бета версия 1.0.0) на ютьюб : https://youtu.be/zgqkdXJhbpc?si=sXNdRZAOUsB56D8h

Есть еще ресурс на нашем сайте https://kkmspb.ru/development/Qt/layouts-and-widgets/QTableView/developing/row-in-two-lines/ 

Ещё на Habr можно посмотреть цикл статей. https://habr.com/ru/articles/761530/

Ну и конечно же как без гитхаба:
![image](https://github.com/PavelDorofeev/How-to-create-own-QTableView-with-new-capabilities/assets/13850002/8ed95681-ea74-4efa-bcf4-f69c36e36d4e)

Начиная с версии 2.0.0 уже нет необходимости собирать наши файлы в составе Qt исходников. Это оказалось возможным с созданием своего клона класса QAbstractItemView. Именно он ограничивал удобство распространения нашего проекта.