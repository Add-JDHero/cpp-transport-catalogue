# TransportСatalogue
Программная реализация системы маршрутизации, работающая с JSON запросами. Программа двухстадийная и имеет два варианта запросов, первый отвечает за создание базы данных, второй же выполняет всю нужную работу для того, чтобы выдать нужную информацию по любым оптимальным маршрутам, автобусам или остановкам. Формата вывода: .json и .svg.  

По запросу "make_base" из входного потока считывает маршруты автобусов, остановки и настройки отрисовки, обрабатывает эти данные и с помощью серилизации сохраняет состояние.  
По запросу "process_requests" из входного потока считывает требуемый запрос, и согласно нему в выходной поток выводит информацию:  
__для автобуса__
- длину маршрута автобуса
- колличество остановок
- колличество уникальных остановок
- извилистость маршрута. Она равна отношению длины дорожного расстояния маршрута к длине географического расстояния.  
 
__для остановки__
- автобусы проходящие через эту остановку  

__для маршрута__   
 _вывод информации повторяется для каждой остановки на которой нужно сесть/пересесть в автобус и для каждого требуемого автобуса
маршрут выбирается исходя из мимнимального времени в пути_ 
- название остановки начала маршрута
- время ожидания автобуса
- номер необходимого автобуса 
- колличество остановок необходимое проехать на этом автобусе до выхода 
- затрачивоемое время в движении на этом автобусе
- остановки для пересадок (если требуется) 

- общее время в пути  

__для карты__  
- строки с svg изображением

## Использование:
Программа должна запускаться с аргументом командной строки "make_base" или "process_requests".  
После запуска с аргументом командной строки "make_base" в стандортный поток ввода необходимо подать документ содержащий данные для создания справочника (пример запроса ниже).  
Чтобы ввести запрос программу необходимо запустить с аргументом командной строки "process_requests" и в стандортный поток ввода необходимо подать документ содержащий требуемые запросы после чего программа выведет данные согласно запросу (пример запроса ниже). 


## Сборка программы:
```
git clone git@github.com:Add-JDHero/cpp-transport-catalogue.git
cd cpp-transport-catalogue
mkdir build && cd build
cmake ..
cmake --build .

# Шаг 1.   Запуск первой части программы:
./transport_catalogue make_base  < ../json_input_files/input_make_base.txt > ../output.txt

# Шаг 2. Запуск второй части программы:
./transport_catalogue process_requests  < ../json_input_files/input_process_requests.txt > ../output.txt


# Итог: в папке build готово svg изображение дорожной карты "transport_catalogue.svg".
``` 



<details><summary>Пример ввода после команды "make_base"</summary>

~~~
    <?xml version="1.0" encoding="UTF-8" ?>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1">
  <polyline points="399.983,528.273 345.993,553.659 317.894,581.093 276.547,657.748 317.894,581.093 345.993,553.659 399.983,528.273" fill="none" stroke="red" stroke-width="10" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="50,255.16 169.562,275.668 166.376,329.409 189.24,365.389 140.028,437.971 157.815,500.299 143.828,545.039 176.116,597.528 223.619,620.002 276.547,657.748 309.001,681.832 342.21,703.401 421.984,735.806 513.456,771.445 526.713,728.887 513.456,771.445 421.984,735.806 342.21,703.401 309.001,681.832 276.547,657.748 223.619,620.002 176.116,597.528 143.828,545.039 157.815,500.299 140.028,437.971 189.24,365.389 166.376,329.409 169.562,275.668 50,255.16" fill="none" stroke="green" stroke-width="10" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="176.116,597.528 220.309,564.344 242.069,506.38 186.926,476.273 214.642,414.551 232.487,386.851 182.57,253.965 159.142,225.261 132.778,190.476 96.5489,171.246 121.404,111.656 158.57,50 121.404,111.656 96.5489,171.246 132.778,190.476 159.142,225.261 182.57,253.965 232.487,386.851 214.642,414.551 186.926,476.273 242.069,506.38 220.309,564.344 176.116,597.528" fill="none" stroke="blue" stroke-width="10" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="309.001,681.832 357.276,630.48 382.695,606.23 357.276,630.48 309.001,681.832" fill="none" stroke="brown" stroke-width="10" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="649.071,733.732 526.713,728.887 546.151,796.914 613.415,827.883 682.67,865.912 749.743,900 826.282,776.282 749.743,900 682.67,865.912 613.415,827.883 546.151,796.914 526.713,728.887 649.071,733.732" fill="none" stroke="orange" stroke-width="10" stroke-linecap="round" stroke-linejoin="round"/>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="399.983" y="528.273" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">13</text>
  <text fill="red" x="399.983" y="528.273" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">13</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="276.547" y="657.748" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">13</text>
  <text fill="red" x="276.547" y="657.748" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">13</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="50" y="255.16" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">23</text>
  <text fill="green" x="50" y="255.16" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">23</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="526.713" y="728.887" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">23</text>
  <text fill="green" x="526.713" y="728.887" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">23</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="176.116" y="597.528" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">36</text>
  <text fill="blue" x="176.116" y="597.528" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">36</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="158.57" y="50" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">36</text>
  <text fill="blue" x="158.57" y="50" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">36</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="309.001" y="681.832" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">44к</text>
  <text fill="brown" x="309.001" y="681.832" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">44к</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="382.695" y="606.23" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">44к</text>
  <text fill="brown" x="382.695" y="606.23" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">44к</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="649.071" y="733.732" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">90</text>
  <text fill="orange" x="649.071" y="733.732" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">90</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="826.282" y="776.282" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">90</text>
  <text fill="orange" x="826.282" y="776.282" dx="7" dy="15" font-size="18" font-family="Verdana" font-weight="bold">90</text>
  <circle cx="242.069" cy="506.38" r="3" fill="white"/>
  <circle cx="132.778" cy="190.476" r="3" fill="white"/>
  <circle cx="182.57" cy="253.965" r="3" fill="white"/>
  <circle cx="649.071" cy="733.732" r="3" fill="white"/>
  <circle cx="214.642" cy="414.551" r="3" fill="white"/>
  <circle cx="749.743" cy="900" r="3" fill="white"/>
  <circle cx="826.282" cy="776.282" r="3" fill="white"/>
  <circle cx="176.116" cy="597.528" r="3" fill="white"/>
  <circle cx="159.142" cy="225.261" r="3" fill="white"/>
  <circle cx="223.619" cy="620.002" r="3" fill="white"/>
  <circle cx="186.926" cy="476.273" r="3" fill="white"/>
  <circle cx="166.376" cy="329.409" r="3" fill="white"/>
  <circle cx="309.001" cy="681.832" r="3" fill="white"/>
  <circle cx="143.828" cy="545.039" r="3" fill="white"/>
  <circle cx="345.993" cy="553.659" r="3" fill="white"/>
  <circle cx="317.894" cy="581.093" r="3" fill="white"/>
  <circle cx="682.67" cy="865.912" r="3" fill="white"/>
  <circle cx="513.456" cy="771.445" r="3" fill="white"/>
  <circle cx="613.415" cy="827.883" r="3" fill="white"/>
  <circle cx="189.24" cy="365.389" r="3" fill="white"/>
  <circle cx="140.028" cy="437.971" r="3" fill="white"/>
  <circle cx="50" cy="255.16" r="3" fill="white"/>
  <circle cx="546.151" cy="796.914" r="3" fill="white"/>
  <circle cx="169.562" cy="275.668" r="3" fill="white"/>
  <circle cx="220.309" cy="564.344" r="3" fill="white"/>
  <circle cx="157.815" cy="500.299" r="3" fill="white"/>
  <circle cx="421.984" cy="735.806" r="3" fill="white"/>
  <circle cx="276.547" cy="657.748" r="3" fill="white"/>
  <circle cx="526.713" cy="728.887" r="3" fill="white"/>
  <circle cx="382.695" cy="606.23" r="3" fill="white"/>
  <circle cx="399.983" cy="528.273" r="3" fill="white"/>
  <circle cx="357.276" cy="630.48" r="3" fill="white"/>
  <circle cx="232.487" cy="386.851" r="3" fill="white"/>
  <circle cx="158.57" cy="50" r="3" fill="white"/>
  <circle cx="96.5489" cy="171.246" r="3" fill="white"/>
  <circle cx="121.404" cy="111.656" r="3" fill="white"/>
  <circle cx="342.21" cy="703.401" r="3" fill="white"/>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="242.069" y="506.38" dx="7" dy="-3" font-size="13" font-family="Verdana">Автовокзал</text>
  <text fill="black" x="242.069" y="506.38" dx="7" dy="-3" font-size="13" font-family="Verdana">Автовокзал</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="132.778" y="190.476" dx="7" dy="-3" font-size="13" font-family="Verdana">Деревообр. комбинат</text>
  <text fill="black" x="132.778" y="190.476" dx="7" dy="-3" font-size="13" font-family="Verdana">Деревообр. комбинат</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="182.57" y="253.965" dx="7" dy="-3" font-size="13" font-family="Verdana">Кинотеатр Юбилейный</text>
  <text fill="black" x="182.57" y="253.965" dx="7" dy="-3" font-size="13" font-family="Verdana">Кинотеатр Юбилейный</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="649.071" y="733.732" dx="7" dy="-3" font-size="13" font-family="Verdana">Краево-Греческая улица</text>
  <text fill="black" x="649.071" y="733.732" dx="7" dy="-3" font-size="13" font-family="Verdana">Краево-Греческая улица</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="214.642" y="414.551" dx="7" dy="-3" font-size="13" font-family="Verdana">Магазин Быт</text>
  <text fill="black" x="214.642" y="414.551" dx="7" dy="-3" font-size="13" font-family="Verdana">Магазин Быт</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="749.743" y="900" dx="7" dy="-3" font-size="13" font-family="Verdana">Мацеста</text>
  <text fill="black" x="749.743" y="900" dx="7" dy="-3" font-size="13" font-family="Verdana">Мацеста</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="826.282" y="776.282" dx="7" dy="-3" font-size="13" font-family="Verdana">Мацестинская долина</text>
  <text fill="black" x="826.282" y="776.282" dx="7" dy="-3" font-size="13" font-family="Verdana">Мацестинская долина</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="176.116" y="597.528" dx="7" dy="-3" font-size="13" font-family="Verdana">Морской вокзал</text>
  <text fill="black" x="176.116" y="597.528" dx="7" dy="-3" font-size="13" font-family="Verdana">Морской вокзал</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="159.142" y="225.261" dx="7" dy="-3" font-size="13" font-family="Verdana">Новая Заря</text>
  <text fill="black" x="159.142" y="225.261" dx="7" dy="-3" font-size="13" font-family="Verdana">Новая Заря</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="223.619" y="620.002" dx="7" dy="-3" font-size="13" font-family="Verdana">Органный зал</text>
  <text fill="black" x="223.619" y="620.002" dx="7" dy="-3" font-size="13" font-family="Verdana">Органный зал</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="186.926" y="476.273" dx="7" dy="-3" font-size="13" font-family="Verdana">Отель Звёздный</text>
  <text fill="black" x="186.926" y="476.273" dx="7" dy="-3" font-size="13" font-family="Verdana">Отель Звёздный</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="166.376" y="329.409" dx="7" dy="-3" font-size="13" font-family="Verdana">Пансионат Нева</text>
  <text fill="black" x="166.376" y="329.409" dx="7" dy="-3" font-size="13" font-family="Verdana">Пансионат Нева</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="309.001" y="681.832" dx="7" dy="-3" font-size="13" font-family="Verdana">Пансионат Светлана</text>
  <text fill="black" x="309.001" y="681.832" dx="7" dy="-3" font-size="13" font-family="Verdana">Пансионат Светлана</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="143.828" y="545.039" dx="7" dy="-3" font-size="13" font-family="Verdana">Парк Ривьера</text>
  <text fill="black" x="143.828" y="545.039" dx="7" dy="-3" font-size="13" font-family="Verdana">Парк Ривьера</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="345.993" y="553.659" dx="7" dy="-3" font-size="13" font-family="Verdana">Пионерская улица, 111</text>
  <text fill="black" x="345.993" y="553.659" dx="7" dy="-3" font-size="13" font-family="Verdana">Пионерская улица, 111</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="317.894" y="581.093" dx="7" dy="-3" font-size="13" font-family="Verdana">Садовая</text>
  <text fill="black" x="317.894" y="581.093" dx="7" dy="-3" font-size="13" font-family="Verdana">Садовая</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="682.67" y="865.912" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Заря</text>
  <text fill="black" x="682.67" y="865.912" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Заря</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="513.456" y="771.445" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Металлург</text>
  <text fill="black" x="513.456" y="771.445" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Металлург</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="613.415" y="827.883" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Приморье</text>
  <text fill="black" x="613.415" y="827.883" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Приморье</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="189.24" y="365.389" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Радуга</text>
  <text fill="black" x="189.24" y="365.389" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Радуга</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="140.028" y="437.971" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Родина</text>
  <text fill="black" x="140.028" y="437.971" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Родина</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="50" y="255.16" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Салют</text>
  <text fill="black" x="50" y="255.16" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий Салют</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="546.151" y="796.914" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий им. Ворошилова</text>
  <text fill="black" x="546.151" y="796.914" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторий им. Ворошилова</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="169.562" y="275.668" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторная улица</text>
  <text fill="black" x="169.562" y="275.668" dx="7" dy="-3" font-size="13" font-family="Verdana">Санаторная улица</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="220.309" y="564.344" dx="7" dy="-3" font-size="13" font-family="Verdana">Сбербанк</text>
  <text fill="black" x="220.309" y="564.344" dx="7" dy="-3" font-size="13" font-family="Verdana">Сбербанк</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="157.815" y="500.299" dx="7" dy="-3" font-size="13" font-family="Verdana">Спортивная</text>
  <text fill="black" x="157.815" y="500.299" dx="7" dy="-3" font-size="13" font-family="Verdana">Спортивная</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="421.984" y="735.806" dx="7" dy="-3" font-size="13" font-family="Verdana">Стадион</text>
  <text fill="black" x="421.984" y="735.806" dx="7" dy="-3" font-size="13" font-family="Verdana">Стадион</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="276.547" y="657.748" dx="7" dy="-3" font-size="13" font-family="Verdana">Театральная</text>
  <text fill="black" x="276.547" y="657.748" dx="7" dy="-3" font-size="13" font-family="Verdana">Театральная</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="526.713" y="728.887" dx="7" dy="-3" font-size="13" font-family="Verdana">Улица Бытха</text>
  <text fill="black" x="526.713" y="728.887" dx="7" dy="-3" font-size="13" font-family="Verdana">Улица Бытха</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="382.695" y="606.23" dx="7" dy="-3" font-size="13" font-family="Verdana">Улица В. Лысая Гора</text>
  <text fill="black" x="382.695" y="606.23" dx="7" dy="-3" font-size="13" font-family="Verdana">Улица В. Лысая Гора</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="399.983" y="528.273" dx="7" dy="-3" font-size="13" font-family="Verdana">Улица Лизы Чайкиной</text>
  <text fill="black" x="399.983" y="528.273" dx="7" dy="-3" font-size="13" font-family="Verdana">Улица Лизы Чайкиной</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="357.276" y="630.48" dx="7" dy="-3" font-size="13" font-family="Verdana">Улица Лысая Гора</text>
  <text fill="black" x="357.276" y="630.48" dx="7" dy="-3" font-size="13" font-family="Verdana">Улица Лысая Гора</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="232.487" y="386.851" dx="7" dy="-3" font-size="13" font-family="Verdana">Хлебозавод</text>
  <text fill="black" x="232.487" y="386.851" dx="7" dy="-3" font-size="13" font-family="Verdana">Хлебозавод</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="158.57" y="50" dx="7" dy="-3" font-size="13" font-family="Verdana">Целинная улица</text>
  <text fill="black" x="158.57" y="50" dx="7" dy="-3" font-size="13" font-family="Verdana">Целинная улица</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="96.5489" y="171.246" dx="7" dy="-3" font-size="13" font-family="Verdana">Целинная улица, 5</text>
  <text fill="black" x="96.5489" y="171.246" dx="7" dy="-3" font-size="13" font-family="Verdana">Целинная улица, 5</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="121.404" y="111.656" dx="7" dy="-3" font-size="13" font-family="Verdana">Целинная улица, 57</text>
  <text fill="black" x="121.404" y="111.656" dx="7" dy="-3" font-size="13" font-family="Verdana">Целинная улица, 57</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="342.21" y="703.401" dx="7" dy="-3" font-size="13" font-family="Verdana">Цирк</text>
  <text fill="black" x="342.21" y="703.401" dx="7" dy="-3" font-size="13" font-family="Verdana">Цирк</text>
</svg>
~~~
</details>

## Cистемные требования:
- С++17 (STL);
- GCC.

## Стек технологий:
- CMake 3.22.0;
- Protobuf-cpp 3.18.1.
