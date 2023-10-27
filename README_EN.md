Hello everyone, dear program developers!
Today we present an open project: how to make your own QTableView with new features.
We were prompted to do this by the need to display table rows with sections according to a certain pattern.
That is, like in the picture below:
![image](https://github.com/PavelDorofeev/How-to-create-own-QTableView-with-new-capabilities/assets/13850002/be731312-aa0b-4d14-ad42-832ab513857f)

To implement our QpTableView, we spent about 5 weeks studying and changing the QTableView code, but it was worth it because we finally had to understand how rendering works in Qt.
(Qp is the prefix for the names of our files and classes)
To use our new classes QpTableView, QpHorHeaderView and QpVertHeaderView today, you will have to add these classes to the sources of Qt itself and rebuild the gui source branch. It won't work any other way for now.
instructions on how to do this are attached https://github.com/PavelDorofeev/How-to-create-own-QTableView-with-new-capabilities/blob/master/How%20to%20integrate%20our%20Qp%20classes%20with%20Qt %20sources.

The first introductory video in hot pursuit (first beta version 1.0.0) on YouTube: https://youtu.be/zgqkdXJhbpc?si=sXNdRZAOUsB56D8h

There is also a resource on our website https://kkmspb.ru/development/Qt/layouts-and-widgets/QTableView/developing/row-in-two-lines/

Habr https://habr.com/ru/articles/761530/
