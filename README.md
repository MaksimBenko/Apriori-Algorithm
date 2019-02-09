# Реализация алгоритма Apriori
## Описание
Программа принимает на входе 4 параметра:
- файл с данными; формат данных - матрица MxN, где M - количество транзакций, N - количество товаров;
- показатель минимальной поддержки, (0..100)%;
- тип сортировки (возрастающая, убывающая или никакая).

## Порядок вызова программы
./apriori \<rules filename\> \<support\> \<sort type\> \<max rules on screen\>
\<rules filename\>: path to file with rules
	\<support\>: double, (0..100)%
	\<sort type\>: 'u' - upscending, 'd' - downscending, 'n' - no sort
	\<max rules on screen\>: int, -1 - all

### Примеры: 
#### Обработать файл association_rules.data и вывести на экран все правила с поддержкой не меньше 0.2 в порядке возрастания
./apriori association_rules.data 0.2 u -1 

#### Обработать файл association_rules.data и вывести на экран первые 20 правил с поддержкой не меньше 2.3 в порядке убывания
./apriori association_rules.data 2.3 d 20 

