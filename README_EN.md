Hello, dear software developers!
Today we present an open project: how to make your QTableView with new features.
We were prompted to do this by the need to output table rows with sections according to a certain template.
That is, as approximately in the picture below:
![image](https://github.com/PavelDorofeev/How-to-create-own-QTableView-with-new-capabilities/assets/13850002/be731312-aa0b-4d14-ad42-832ab513857f)

To implement our QpTableView, we spent about 5 weeks studying and changing the QTableView code, 
but it was worth it because we finally had to understand how rendering works in Qt.
(Qp is a prefix of the names of our files and classes) 
To use our new QpTableView, QpHorHeaderView and QpVertHeaderView classes today, 
we will have to add these classes to the sources of Qt itself and rebuild the gui source branch. 
It won't work out any other way yet.
instructions on how to do this are attached 
