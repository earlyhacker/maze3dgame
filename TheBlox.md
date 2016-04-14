# Введение #

И так мы подошли к тому что надо быстро собрать рабочую и пригодную для играния игру =) И встал вопрос о загрузке моделей.

# Основная концепция #

И так, у меня есть такой набросок кода -
```
class TheBlox
{
public:
	TheBlox
	{
		firstpath = "Data/Model/";
	}
	void OpenModelFile(const char* FilePath);
	void RenderIt(int ModelID);
	void CleanRender(int ModelID);
	
	int ModelID[128];
	const char* firstpath // начальная часть пути.
	
};	
```


---


Нам для моделей надо хранить полигоны, это три или более точек, плюс неплохо бы нормали, это одна точка, плюс, возможно, текстурные координаты, это два числа. Можно сделать нечто вроде:
```
# нормаль и три вершины
n(0.0, 1.0, 0.0) (1.0, 2.34, 1.2) (0.35, 1.458, 0.36) (4.32, -2.12, -3.1)
# новая строка значит новый полигон
```
а с текстурными координатами можно указывать не 3, а 5 координат на вершину. Не самый удобочитаемый формат, но лего распарсить. Можно что-нибудь ещё придумать, какие идеи?

<i>(...Сейчас просмотрел статью в вики о пресловутом OBJ, не поверишь, там абсолютно то же самое, только синтаксис немного разнится. В таком свете, лучше распарсить OBJ (а он несложен совсем) и иметь поддержку доброй половины 3D редакторов.)</i>