# How-to-create-own-QTableView-with-new-capabilities

Всем привет дорогие разработчики программ!
Сегодня мы представляем открытый проект: как сделать свой QTableView с новыми возможностями.
Нас к этому побудила необходимость выводить строки таблицы с секциями по определенному шаблону.
То есть как примерно на картинке ниже:
![image](https://github.com/PavelDorofeev/How-to-create-own-QTableView-with-new-capabilities/assets/13850002/be731312-aa0b-4d14-ad42-832ab513857f)

Чтобы реализовать свой QpTableView мы потратили примерно 5 недель на  изучение и изменение кода QTableView, но оно того стоило ибо пришлось наконец-то понять как работает отрисовка в Qt.
(Qp это префикс названий наших файлов и классов) 
Чтобы воспользоваться нашими новыми классами QpTableView, QpHorHeaderView и QpVertHeaderView на сегодняшний день придется эти классы добавить в исходники самого Qt и пересобрать заново ветку исходников gui. По другому пока не получится.
инструкция как это сделать прилагается .
